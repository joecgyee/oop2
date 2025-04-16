/*
  ==============================================================================

    DeckGUI.h
    Created: 25 Feb 2025 2:54:24pm
    Author:  jcgy0

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"

//==============================================================================
/*
*/
class DeckGUI : public juce::Component,
                public Button::Listener,
                public Slider::Listener,
                public FileDragAndDropTarget,
                public Timer
{
public:
    DeckGUI(DJAudioPlayer* player,
            AudioFormatManager& formatManagerToUse,
            AudioThumbnailCache& cacheToUse,
            WaveformDisplay& waveformDisplay,
            bool layoutInverted = false);
    ~DeckGUI() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    //==============================================================================
    /** implement Button::Listener */
    void buttonClicked(Button* button) override;

    /** implement Slider::Listener */
    void sliderValueChanged(Slider* slider) override;

    /* FileDragAndDropTarget */
    bool isInterestedInFileDrag(const StringArray& files) override;
    void filesDropped(const StringArray& files, int x, int y) override;

    void timerCallback() override;

private:
    TextButton loadButton{ "LOAD" };
    
    DrawableButton playPauseButton{ "playPauseButton", DrawableButton::ImageOnButtonBackground };
    std::unique_ptr<Drawable> playIcon;
    std::unique_ptr<Drawable> pauseIcon;

    Slider volSlider{ Slider::LinearVertical, Slider::TextBoxBelow };
    Slider speedSlider{ Slider::LinearVertical, Slider::TextBoxBelow };

    FileChooser fChooser{ "Select a file..." };
    DJAudioPlayer* player;
    WaveformDisplay& waveformDisplay;
    bool layoutInverted;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeckGUI)
};
