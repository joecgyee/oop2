#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "DeckGUI.h"
#include "PlaylistComponent.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public AudioAppComponent,
                      public Slider::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint(Graphics& g) override;
    void resized() override;

    //==============================================================================
    void sliderValueChanged(Slider* slider) override;


private:
    //==============================================================================
    // Your private member variables go here..
    AudioFormatManager formatManager;
    AudioThumbnailCache thumbCache{ 100 }; // store up to 100 waveforms

    DJAudioPlayer player1{ formatManager };
    DJAudioPlayer player2{ formatManager };
    MixerAudioSource mixerSource;

    Colour deckGUI1_colour = Colour::fromRGB(255, 219, 88);
    Colour deckGUI2_colour = Colour::fromRGB(62, 137, 137);
    WaveformDisplay waveformDisplay1{ formatManager, thumbCache, deckGUI1_colour };
    WaveformDisplay waveformDisplay2{ formatManager, thumbCache, deckGUI2_colour };

    DeckGUI deckGUI1{ &player1, formatManager, thumbCache, waveformDisplay1, false };
    DeckGUI deckGUI2{ &player2, formatManager, thumbCache, waveformDisplay2, true };

    Slider transitionSlider{ Slider::LinearHorizontal, Slider::NoTextBox };

    PlaylistComponent playlistComponent{ &player1, &player2, &waveformDisplay1, &waveformDisplay2 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
