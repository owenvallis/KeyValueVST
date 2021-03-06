/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
Key_valueAudioProcessorEditor::Key_valueAudioProcessorEditor (Key_valueAudioProcessor* ownerFilter)
    : AudioProcessorEditor (ownerFilter),
      performerMidiInputSelectorA("PerformerA"),
      performerMidiInputSelectorB("PerformerB"),
      mode("Mode"),
      performerMidiInputSelectorALabel(" ", "MIDI Channel PerformerA"),
      performerMidiInputSelectorBLabel(" ", "MIDI Channel PerformerB"),      
      modeLabel(" ", "Mode"),
      saveData("Save"),
      loadData("Load")

{
    
    for (int i = 1; i < 17; i++) 
    {        
        performerMidiInputSelectorA.addItem(String(i), i);        
        performerMidiInputSelectorB.addItem(String(i), i);
    }
    
    mode.addItem("Learning", 1);
    mode.addItem("Performance", 2);
    
    performerMidiInputSelectorALabel.attachToComponent (&performerMidiInputSelectorA, false);
    performerMidiInputSelectorALabel.setFont (Font (11.0f));
    
    performerMidiInputSelectorBLabel.attachToComponent (&performerMidiInputSelectorB, false);
    performerMidiInputSelectorBLabel.setFont (Font (11.0f));
    
    modeLabel.attachToComponent (&mode, false);
    modeLabel.setFont (Font (11.0f));
    
    // add listeners
    performerMidiInputSelectorA.addListener(this);
    performerMidiInputSelectorB.addListener(this);
    mode.addListener(this);
    saveData.addListener(this);
    loadData.addListener(this);
    
    // add all our components
    addAndMakeVisible(&performerMidiInputSelectorA);
    addAndMakeVisible(&performerMidiInputSelectorB);
    addAndMakeVisible(&mode);
    addAndMakeVisible(&saveData);
    addAndMakeVisible(&loadData);
    
    mode.setText(getProcessor()->getMode());
    
    // This is where our plugin's editor size is set.
    setSize (250, 130);
}

Key_valueAudioProcessorEditor::~Key_valueAudioProcessorEditor()
{
}

//==============================================================================
void Key_valueAudioProcessorEditor::paint (Graphics& g)
{

}

void Key_valueAudioProcessorEditor::resized()
{
    // hard set for right now
    performerMidiInputSelectorA.setBounds(10, 30, 100, 22);
    performerMidiInputSelectorB.setBounds(10, 80, 100, 22);
    mode.setBounds(130, 30, 100, 22);
    saveData.setBounds(130, 80, 45, 22);
    loadData.setBounds(185, 80, 45, 22);
}

void Key_valueAudioProcessorEditor::comboBoxChanged (ComboBox *comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &performerMidiInputSelectorA)
    {
        getProcessor()->setMidiChannelPerformerA(performerMidiInputSelectorA.getSelectedId());
    }
    else if (comboBoxThatHasChanged == &performerMidiInputSelectorB)
    {
        getProcessor()->setMidiChannelPerformerB(performerMidiInputSelectorB.getSelectedId());
    }
    else if (comboBoxThatHasChanged == &mode)
    {
        getProcessor()->setMode(mode.getText());
    }
}

void Key_valueAudioProcessorEditor::buttonClicked (Button* button)
{       
    if (button == &saveData) {
        getProcessor()->saveData();       
    } else if (button == &loadData) {
        getProcessor()->loadData();
    }
}

