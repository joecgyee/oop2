/*
  ==============================================================================

    DeckGUI.cpp
    Created: 25 Feb 2025 2:54:24pm
    Author:  jcgy0

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DeckGUI.h"

//==============================================================================
DeckGUI::DeckGUI(DJAudioPlayer* _player,
                AudioFormatManager& formatManagerToUse,
                AudioThumbnailCache& cacheToUse,
                WaveformDisplay& _waveformDisplay,
                bool _layoutInverted
                ) : player(_player),
                    waveformDisplay(_waveformDisplay),
                    layoutInverted(_layoutInverted)
{
    // Load the play and pause icons
    File playSVGFile = File::getCurrentWorkingDirectory().getParentDirectory().getParentDirectory().getChildFile("Source/Assets/play.svg");
    File pauseSVGFile = File::getCurrentWorkingDirectory().getParentDirectory().getParentDirectory().getChildFile("Source/Assets/pause.svg");
    playIcon = Drawable::createFromSVGFile(playSVGFile);
    pauseIcon = Drawable::createFromSVGFile(pauseSVGFile);

    if (player->isPlaying == false)
    {
        playPauseButton.setImages(playIcon.get(), playIcon.get(), playIcon.get(), playIcon.get(),
            pauseIcon.get(), pauseIcon.get(), pauseIcon.get(), pauseIcon.get());
    }
    else
    {
        playPauseButton.setImages(pauseIcon.get(), pauseIcon.get(), pauseIcon.get(), pauseIcon.get(),
            playIcon.get(), playIcon.get(), playIcon.get(), playIcon.get());
    }

    playPauseButton.setClickingTogglesState(true); 

    addAndMakeVisible(playPauseButton);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(volSlider);
    addAndMakeVisible(speedSlider);

    // When the playhead is moved, the callback is called with the new relative position.
    waveformDisplay.setPlayheadMovedCallback([this](double newPosition) {
        player->setPositionRelative(newPosition);
        });

//================================================================================
    /* Adding Listeners */
    playPauseButton.addListener(this);
    loadButton.addListener(this);
    volSlider.addListener(this);
    speedSlider.addListener(this);

//== Sliders configurations ========================================================
    /* Set range of sliders */
    volSlider.setRange(0.0, 2.0);
    speedSlider.setRange(0.1, 3.0);

    /* Set default values for sliders */
    volSlider.setValue(1.0); 
    speedSlider.setValue(1.0); 

    /* Limit slider values to increments of 0.1, display only 1 decimal place */
    volSlider.setNumDecimalPlacesToDisplay(1); 
    speedSlider.setNumDecimalPlacesToDisplay(1); 

    /* Disable snapping to mouse position */
    volSlider.setSliderSnapsToMousePosition(false); 
    speedSlider.setSliderSnapsToMousePosition(false);
//===================================================================================

    startTimer(500);
}

DeckGUI::~DeckGUI()
{
    stopTimer();
}

void DeckGUI::paint(Graphics& g)
{
    g.fillAll(Colours::black);

    g.setColour(Colours::grey);
    g.drawRect(getLocalBounds(), 1);   // draw an outline around the component

//== Buttons ================================================================================

    // Play Button
    playPauseButton.setColour(TextButton::buttonColourId, Colour::fromRGB(245, 245, 220));
    playPauseButton.setColour(DrawableButton::textColourId, Colours::black);
    // Pause Button
    playPauseButton.setColour(TextButton::buttonOnColourId, Colour::fromRGB(255, 102, 102));
    playPauseButton.setColour(DrawableButton::textColourOnId, Colours::white);

    g.setColour(Colours::white); // Set text for "DECK 1", "DECK 2"
    Font deckGUIfont = Font("Roboto", 20.0f, Font::plain);
    g.setFont(deckGUIfont);

    Rectangle<int> loadButtonBounds = loadButton.getBounds();

    if (!this->layoutInverted)
    {
        /* deckGUI1 */
        g.drawText("DECK 1",
            loadButtonBounds.getX(),
            loadButtonBounds.getY() - 50, // Position text 50 pixels above the slider
            loadButtonBounds.getWidth(),
            20,
            Justification::centred,
            true);

        loadButton.setColour(TextButton::buttonColourId, Colour::fromRGB(255, 219, 88));
        loadButton.setColour(TextButton::textColourOnId, Colours::black);
        loadButton.setColour(TextButton::textColourOffId, Colours::black);
    }
    else
    {
        /* deckGUI2 */
        g.drawText("DECK 2",
            loadButtonBounds.getX(),
            loadButtonBounds.getY() - 50, // Position text 50 pixels above the slider
            loadButtonBounds.getWidth(),
            20,
            Justification::centred,
            true);

        loadButton.setColour(TextButton::buttonColourId, Colour::fromRGB(62, 137, 137));
        loadButton.setColour(TextButton::textColourOnId, Colours::black);
        loadButton.setColour(TextButton::textColourOffId, Colours::black);
    }
    
//== Sliders ==========================================================================================
    
    volSlider.setColour(Slider::backgroundColourId, Colours::darkgrey);
    volSlider.setColour(Slider::thumbColourId, Colours::white);
    volSlider.setColour(Slider::trackColourId, Colours::grey);

    speedSlider.setColour(Slider::backgroundColourId, Colours::darkgrey);
    speedSlider.setColour(Slider::thumbColourId, Colours::white);
    speedSlider.setColour(Slider::trackColourId, Colours::grey);

    g.setColour(Colours::white); // Set text for sliders
    Font sliderFont = Font("Arial", 14.0f, Font::plain);
    g.setFont(sliderFont);

    Rectangle<int> speedSliderBounds = speedSlider.getBounds();
    Rectangle<int> volSliderBounds = volSlider.getBounds();

    g.drawText("SPEED",
        speedSliderBounds.getX(),
        speedSliderBounds.getY() - 20, // Position text 20 pixels above the slider
        speedSliderBounds.getWidth(),
        20,
        Justification::centred,
        true);

    g.drawText("VOL",
        volSliderBounds.getX(),
        volSliderBounds.getY() - 20, // Position text 20 pixels above the slider
        volSliderBounds.getWidth(),
        20,
        Justification::centred,
        true);
}

void DeckGUI::resized()
{
    double rowW = getWidth() / 4;
    double rowH = getHeight() / 3;
    double gap = 5;
    
    if (!layoutInverted)
    {
        speedSlider.setBounds(0, 4*gap, rowW, rowH * 3-4*gap);
        loadButton.setBounds(rowW+gap, rowH+gap, rowW*2-2*gap, rowH-2*gap);
        playPauseButton.setBounds(rowW+gap, rowH*2+gap, rowW*2-2*gap, rowH-2*gap);
        volSlider.setBounds(rowW*3, 4 * gap, rowW, rowH*3 - 4 * gap);
    }
    else
    {
        volSlider.setBounds(0, 4 * gap, rowW, rowH * 3 - 4* gap);
        loadButton.setBounds(rowW+gap, rowH+gap, rowW*2-2*gap, rowH-2*gap);
        playPauseButton.setBounds(rowW+gap, rowH*2+gap, rowW*2-2*gap, rowH-2*gap);
        speedSlider.setBounds(rowW * 3, 4 * gap, rowW, rowH * 3 - 4 * gap);
    }
}

void DeckGUI::buttonClicked(Button* button)
{
    if (button == &playPauseButton)
    {
        if (player->isPlaying == false)
        {
            player->start();
        }
        else
        {
            player->stop();
        }
    }
    if (button == &loadButton)
    {
        auto fileChooserFlags = FileBrowserComponent::canSelectFiles;
        fChooser.launchAsync(fileChooserFlags, [this](const FileChooser& chooser)
            {
                File chosenFile = chooser.getResult();
                player->loadURL(URL{ chosenFile });
                waveformDisplay.loadURL(URL{ chosenFile });
            });
    }
}

void DeckGUI::sliderValueChanged(Slider* slider)
{
    if (slider == &volSlider)
    {
        double roundedValue = round(volSlider.getValue() * 10) / 10; // Round to 1 decimal place
        player->setGain(roundedValue);
    }
    if (slider == &speedSlider)
    {
        double roundedValue = round(speedSlider.getValue() * 10) / 10; // Round to 1 decimal place
        player->setSpeed(roundedValue);
    }
}

bool DeckGUI::isInterestedInFileDrag(const StringArray& files)
{
    DBG("DeckGUI::isInterestedInFileDrag");
    return true;
}

void DeckGUI::filesDropped(const StringArray& files, int x, int y)
{
    DBG("DeckGUI::filesDropped");
    if (files.size() == 1)
    {
        player->loadURL(URL{ File{files[0]} });
    }
}

void DeckGUI::timerCallback()
{
    waveformDisplay.setPositionRelative(player->getPositionRelative());
}