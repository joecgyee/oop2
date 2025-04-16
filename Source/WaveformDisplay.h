/*
  ==============================================================================

    WaveformDisplay.h
    Created: 25 Feb 2025 2:54:53pm
    Author:  jcgy0

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class WaveformDisplay : public juce::Component,
                        public ChangeListener,
                        public MouseListener
{
public:
    WaveformDisplay(AudioFormatManager& formatManagerToUse,
                    AudioThumbnailCache& cacheToUse,
                    Colour waveformColour);
    ~WaveformDisplay() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void changeListenerCallback(ChangeBroadcaster* source) override;

    void loadURL(URL audioURL);

    /* Set the relative position of the playhead */
    void setPositionRelative(double pos);

    /* Set a callback to be triggered when the playhead is dragged */
    void setPlayheadMovedCallback(std::function<void(double)> callback);

    /* Mouse events for playhead */
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;

private:
    AudioThumbnail audioThumb;
    bool fileLoaded;
    double position;
    Colour waveformColour;

    std::function<void(double)> playheadMovedCallback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplay)
};