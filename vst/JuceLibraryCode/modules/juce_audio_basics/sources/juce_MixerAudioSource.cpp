/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-11 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/


MixerAudioSource::MixerAudioSource()
    : tempBuffer (2, 0),
      currentSampleRate (0.0),
      bufferSizeExpected (0)
{
}

MixerAudioSource::~MixerAudioSource()
{
    removeAllInputs();
}

//==============================================================================
void MixerAudioSource::addInputSource (AudioSource* input, const bool deleteWhenRemoved)
{
    if (input != nullptr && ! inputs.contains (input))
    {
        double localRate;
        int localBufferSize;

        {
            const ScopedLock sl (lock);
            localRate = currentSampleRate;
            localBufferSize = bufferSizeExpected;
        }

        if (localRate > 0.0)
            input->prepareToPlay (localBufferSize, localRate);

        const ScopedLock sl (lock);

        inputsToDelete.setBit (inputs.size(), deleteWhenRemoved);
        inputs.add (input);
    }
}

void MixerAudioSource::removeInputSource (AudioSource* const input)
{
    if (input != nullptr)
    {
        ScopedPointer<AudioSource> toDelete;

        {
            const ScopedLock sl (lock);
            const int index = inputs.indexOf (input);

            if (index < 0)
                return;

            if (inputsToDelete [index])
                toDelete = input;

            inputsToDelete.shiftBits (index, 1);
            inputs.remove (index);
        }

        input->releaseResources();
    }
}

void MixerAudioSource::removeAllInputs()
{
    OwnedArray<AudioSource> toDelete;

    {
        const ScopedLock sl (lock);

        for (int i = inputs.size(); --i >= 0;)
            if (inputsToDelete[i])
                toDelete.add (inputs.getUnchecked(i));

        inputs.clear();
    }

    for (int i = toDelete.size(); --i >= 0;)
        toDelete.getUnchecked(i)->releaseResources();
}

void MixerAudioSource::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    tempBuffer.setSize (2, samplesPerBlockExpected);

    const ScopedLock sl (lock);

    currentSampleRate = sampleRate;
    bufferSizeExpected = samplesPerBlockExpected;

    for (int i = inputs.size(); --i >= 0;)
        inputs.getUnchecked(i)->prepareToPlay (samplesPerBlockExpected, sampleRate);
}

void MixerAudioSource::releaseResources()
{
    const ScopedLock sl (lock);

    for (int i = inputs.size(); --i >= 0;)
        inputs.getUnchecked(i)->releaseResources();

    tempBuffer.setSize (2, 0);

    currentSampleRate = 0;
    bufferSizeExpected = 0;
}

void MixerAudioSource::getNextAudioBlock (const AudioSourceChannelInfo& info)
{
    const ScopedLock sl (lock);

    if (inputs.size() > 0)
    {
        inputs.getUnchecked(0)->getNextAudioBlock (info);

        if (inputs.size() > 1)
        {
            tempBuffer.setSize (jmax (1, info.buffer->getNumChannels()),
                                info.buffer->getNumSamples());

            AudioSourceChannelInfo info2;
            info2.buffer = &tempBuffer;
            info2.numSamples = info.numSamples;
            info2.startSample = 0;

            for (int i = 1; i < inputs.size(); ++i)
            {
                inputs.getUnchecked(i)->getNextAudioBlock (info2);

                for (int chan = 0; chan < info.buffer->getNumChannels(); ++chan)
                    info.buffer->addFrom (chan, info.startSample, tempBuffer, chan, 0, info.numSamples);
            }
        }
    }
    else
    {
        info.clearActiveBufferRegion();
    }
}
