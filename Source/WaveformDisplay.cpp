/*
  ==============================================================================

    WaveformDisplay.cpp
    Created: 25 Feb 2025 2:54:53pm
    Author:  jcgy0

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WaveformDisplay.h"

//==============================================================================
WaveformDisplay::WaveformDisplay(AudioFormatManager& formatManagerToUse,
                                 AudioThumbnailCache& cacheToUse,
                                 Colour waveformColour
                ) : audioThumb(1000, formatManagerToUse, cacheToUse),
                    fileLoaded(false),
                    position(0),
                    waveformColour(waveformColour)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

    audioThumb.addChangeListener(this);
    addMouseListener(this, true);
}

WaveformDisplay::~WaveformDisplay()
{
}

void WaveformDisplay::paint(juce::Graphics& g)
{
    g.fillAll(Colours::black);

    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);   // draw an outline around the component

    g.setColour(juce::Colours::orange);
    if (fileLoaded && audioThumb.getNumChannels() > 0 && audioThumb.getTotalLength() > 0)
    {
        g.setColour(waveformColour);
        // Draw waveform
        audioThumb.drawChannel(g,
            getLocalBounds(),
            0,
            audioThumb.getTotalLength(),
            0,
            1.0f
        );

        // Draw playhead
        g.setColour(Colours::white);
        int playheadX = static_cast<int>(position * getWidth());
        g.drawLine(playheadX, 0, playheadX, getHeight(), 1.0f);
    }
    else
    {
        g.setFont(FontOptions(20.0f));
        g.drawText("Load track...", getLocalBounds(),
            juce::Justification::centred, true);   // draw some placeholder text
    }
}

void WaveformDisplay::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void WaveformDisplay::loadURL(URL audioURL)
{
    audioThumb.clear();
    fileLoaded = audioThumb.setSource(new URLInputSource(audioURL));
    if (fileLoaded)
    {
        DBG("WaveformDisplay::loadURL loaded!");
        repaint();
    }
    else
    {
        DBG("WaveformDisplay::loadURL not loaded!");
    }
}

void WaveformDisplay::changeListenerCallback(ChangeBroadcaster* source)
{
    DBG("WaveformDisplay::changeListenerCallback: change received!");
    repaint();
}

void WaveformDisplay::setPositionRelative(double pos)
{
    if (pos != position)
    {
        position = pos;
        repaint();
    }
}

void WaveformDisplay::setPlayheadMovedCallback(std::function<void(double)> callback)
{
    playheadMovedCallback = callback;
}

void WaveformDisplay::mouseDown(const juce::MouseEvent& event)
{
    // Calculate the relative position of the mouse click
    double newPosition = static_cast<double>(event.x) / getWidth();
    setPositionRelative(newPosition);

    // Trigger the callback to update playback position
    if (playheadMovedCallback)
    {
        playheadMovedCallback(newPosition);
    }
}

void WaveformDisplay::mouseDrag(const juce::MouseEvent& event)
{
    // Calculate the relative position of the mouse drag
    double newPosition = static_cast<double>(event.x) / getWidth();
    setPositionRelative(newPosition);

    // Trigger the callback to update playback position
    if (playheadMovedCallback)
    {
        playheadMovedCallback(newPosition);
    }
}