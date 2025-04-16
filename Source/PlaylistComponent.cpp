/*
  ==============================================================================

    PlaylistComponent.cpp
    Created: 25 Feb 2025 2:55:19pm
    Author:  jcgy0

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PlaylistComponent.h"

//==============================================================================
PlaylistComponent::PlaylistComponent(DJAudioPlayer* player1, 
                                    DJAudioPlayer* player2, 
                                    WaveformDisplay* waveformDisplay1, 
                                    WaveformDisplay* waveformDisplay2)   
:   player1(player1), 
    player2(player2), 
    waveformDisplay1(waveformDisplay1), 
    waveformDisplay2(waveformDisplay2)
{
    // Load tracks from the "Tracks" directory
    juce::File tracksDirectory{ File::getCurrentWorkingDirectory() // "\OtoDecks\Builds\VisualStudio2022"
                                    .getParentDirectory()  // "\OtoDecks\Builds"
                                    .getParentDirectory() // "\OtoDecks"
                                    .getChildFile("Tracks") }; // "\OtoDecks\Tracks"
    DBG("Tracks Directory: " + tracksDirectory.getFullPathName());
    loadTracksFromDirectory(tracksDirectory);

    addAndMakeVisible(tableComponent);
    tableComponent.setModel(this);

    tableComponent.getHeader().addColumn("Track title", 1, 400);
    tableComponent.getHeader().addColumn("Duration", 2, 250);
    tableComponent.getHeader().addColumn("Load", 3, 250);
}

PlaylistComponent::~PlaylistComponent()
{
}

void PlaylistComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));   // clear the background
    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);   // draw an outline around the component
}

void PlaylistComponent::resized()
{
    tableComponent.setBounds(0, 0, getWidth(), getHeight());
}

int PlaylistComponent::getNumRows()
{
    return trackTitles.size();
}

void PlaylistComponent::paintRowBackground(Graphics& g,
                                            int rowNumber,
                                            int width,
                                            int height,
                                            bool rowIsSelected)
{
    if (rowIsSelected)
    {
        g.fillAll(Colours::orange);
    }
    else
    {
        g.fillAll(Colour::fromRGB(32, 32, 32));
    }
}

void PlaylistComponent::paintCell(Graphics& g,
                                    int rowNumber,
                                    int columnId,
                                    int width,
                                    int height,
                                    bool rowIsSelected)
{
    g.setColour(Colours::white);
    if (columnId == 1) // Track titles
    {
        g.drawText(trackTitles[rowNumber],
            2, 0,
            width - 4, height,
            Justification::centredLeft,
            true);
    }    
    if (columnId == 2) // Duration
    {
        g.drawText(trackDurations[rowNumber],
            2, 0,
            width - 4, height,
            Justification::centredLeft,
            true);
    }
}

Component* PlaylistComponent::refreshComponentForCell(int rowNumber,
                                                    int columnId,
                                                    bool isRowSelected,
                                                    Component* existingComponentToUpdate)
{
    if (columnId == 3) // Load column
    {
        if (existingComponentToUpdate == nullptr)
        {
            auto* buttonContainer = new Component();
            buttonContainer->setSize(250, 30);

            auto* loadToDeck1 = new ArrowButton ("Load to Deck 1", 0.5f, Colour::fromRGB(255, 219, 88));
            loadToDeck1->setSize(20, 20);
            loadToDeck1->setTopLeftPosition(250/2-20, 2);
            loadToDeck1->addListener(this);
            loadToDeck1->setComponentID("deck1_" + String(rowNumber)); // Store row number in ID
            buttonContainer->addAndMakeVisible(loadToDeck1);

            auto* loadToDeck2 = new ArrowButton("Load to Deck 2", 0.0f, Colour::fromRGB(62, 137, 137));
            loadToDeck2->setSize(20, 20);
            loadToDeck2->setTopLeftPosition(250/2+20, 2);
            loadToDeck2->addListener(this);
            loadToDeck2->setComponentID("deck2_" + String(rowNumber)); // Store row number in ID
            buttonContainer->addAndMakeVisible(loadToDeck2);

            existingComponentToUpdate = buttonContainer;
        }
    }

    return existingComponentToUpdate;
}

void PlaylistComponent::buttonClicked(Button* button)
{
    // Extract the row number and deck type from the button's ID
    String buttonID = button->getComponentID();
    int rowNumber = buttonID.getTrailingIntValue();

    if (buttonID.startsWith("deck1_")) // Load to Deck 1
    {
        URL trackURL = getTrackURL(rowNumber);
        player1->loadURL(trackURL);
        waveformDisplay1->loadURL(trackURL);
    }
    else if (buttonID.startsWith("deck2_")) // Load to Deck 2
    {
        URL trackURL = getTrackURL(rowNumber);
        player2->loadURL(trackURL);
        waveformDisplay2->loadURL(trackURL);
    }
}

URL PlaylistComponent::getTrackURL(int rowNumber)
{
    if (rowNumber >= 0 && rowNumber < trackURLs.size())
        return trackURLs[rowNumber];
    return URL();
}

std::string PlaylistComponent::formatDuration(double durationInSeconds)
{
    int totalSeconds = static_cast<int>(durationInSeconds); // Convert to integer
    int hours = totalSeconds / 3600;                       // Calculate hours
    int minutes = (totalSeconds % 3600) / 60;              // Calculate minutes
    int seconds = totalSeconds % 60;                       // Calculate seconds

    // Format into "HH:MM:SS"
    std::ostringstream stream;
    stream << std::setw(2) << std::setfill('0') << hours << ":"
        << std::setw(2) << std::setfill('0') << minutes << ":"
        << std::setw(2) << std::setfill('0') << seconds;

    return stream.str();
}

void PlaylistComponent::loadTracksFromDirectory(const File& directory)
{
    if (directory.exists() && directory.isDirectory())
    {
        // Set up the AudioFormatManager to recognize MP3 files
        AudioFormatManager formatManager;
        formatManager.registerBasicFormats(); 

        // Find all .mp3 files in the directory
        Array<juce::File> mp3Files;
        directory.findChildFiles(mp3Files, juce::File::findFiles, false, "*.mp3");

        for (const auto& file : mp3Files)
        {
            // Add their names to `trackTitles` vector
            trackTitles.push_back(file.getFileName().toStdString());

            // Add their durations to `trackDurations` vector
            std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
            if (reader)
            {
                double durationInSeconds = reader->lengthInSamples / reader->sampleRate;
                std::string formattedDuration = formatDuration(durationInSeconds); // Format as "HH:MM:SS"
                trackDurations.push_back(formattedDuration);
            }

            // Add their URLs to `trackURLs` vector
            trackURLs.push_back(URL{ file });
        }
        tableComponent.updateContent(); // Refresh the table
    }
    else
    {
        DBG("Directory does not exist or is not a directory: " + directory.getFullPathName());
    }
}
