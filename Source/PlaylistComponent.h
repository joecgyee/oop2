/*
  ==============================================================================

    PlaylistComponent.h
    Created: 25 Feb 2025 2:55:19pm
    Author:  jcgy0

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>
#include <string>
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"

//==============================================================================
/*
*/
class PlaylistComponent : public juce::Component,
                          public TableListBoxModel,
                          public Button::Listener
{
public:
    PlaylistComponent(DJAudioPlayer* player1, 
                      DJAudioPlayer* player2, 
                      WaveformDisplay* waveformDisplay1, 
                      WaveformDisplay* waveformDisplay2);
    ~PlaylistComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    int getNumRows() override;
    void paintRowBackground(Graphics&,
                            int rowNumber,
                            int width,
                            int height,
                            bool rowIsSelected) override;
    void paintCell(Graphics&,
                    int rowNumber,
                    int columnId,
                    int width,
                    int height,
                    bool rowIsSelected) override;
    Component* refreshComponentForCell(int rowNumber,
                                        int columnId,
                                        bool isRowSelected,
                                        Component* existingComponentToUpdate) override;

    void buttonClicked(Button* button) override;

private:
    TableListBox tableComponent;

    /* Vectors to store tracks */
    std::vector<std::string> trackTitles;
    std::vector<std::string> trackDurations;
    std::vector<URL> trackURLs;

    /* Load tracks' titles, durations, URLs from "Tracks" directory */
    void loadTracksFromDirectory(const File& directory);

    /* Function to format seconds into string "HH:MM:SS" */
    std::string formatDuration(double durationInSeconds);
    
    URL getTrackURL(int rowNumber);

    DJAudioPlayer* player1;
    DJAudioPlayer* player2;
    WaveformDisplay* waveformDisplay1;
    WaveformDisplay* waveformDisplay2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistComponent)
};