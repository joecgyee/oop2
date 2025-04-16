#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize(900, 600);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio)
        && !juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio,
            [&](bool granted) { setAudioChannels(granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels(0, 2);
    }

    addAndMakeVisible(deckGUI1);
    addAndMakeVisible(deckGUI2);
    addAndMakeVisible(waveformDisplay1);
    addAndMakeVisible(waveformDisplay2);

    // Configure the transition slider
    transitionSlider.setRange(0.0, 2.0); // 0.0: player1 only, 2.0: player2 only
    transitionSlider.setValue(1.0); // Start in the middle (both players at equal volume)
    transitionSlider.addListener(this);
    addAndMakeVisible(transitionSlider);
    
    addAndMakeVisible(playlistComponent);

    formatManager.registerBasicFormats();
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);
    mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    mixerSource.addInputSource(&player1, false);
    mixerSource.addInputSource(&player2, false);
}

void MainComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    mixerSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.
    player1.releaseResources();
    player2.releaseResources();
    mixerSource.releaseResources();
}

//==============================================================================
void MainComponent::paint(Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.fillAll(Colour::fromRGB(32, 32, 32));

    // You can add your drawing code here!
    transitionSlider.setColour(Slider::backgroundColourId, Colours::white);
    transitionSlider.setColour(Slider::thumbColourId, Colours::red);
    transitionSlider.setColour(Slider::trackColourId, Colours::white);
}

void MainComponent::resized()
{
    double rowW = getWidth() / 4;
    double rowH = getHeight() / 6;

    deckGUI1.setBounds(0, 0, rowW, rowH*3);
    deckGUI2.setBounds(rowW*3, 0, rowW, rowH*3);
    waveformDisplay1.setBounds(rowW, 0, rowW*2, rowH);
    waveformDisplay2.setBounds(rowW, rowH, rowW*2, rowH);
    transitionSlider.setBounds(rowW+ 0.5*rowW, rowH * 2, (rowW * 2) / 2, rowH);
    playlistComponent.setBounds(0, rowH*3, rowW*4, rowH*3);
}

void MainComponent::sliderValueChanged(Slider* slider)
{
    if (slider == &transitionSlider)
    {
        // Get the transitionSlider value (ranges from 0.0 to 2.0)
        double sliderValue = transitionSlider.getValue();
        
        // From DeckGUI's volSlider setting
        double defaultVol = 1.0;
        double maxVol = 2.0;
        // Calculate the relative gain for player1 and player2 based on the slider value 
        double player1Gain = defaultVol * (maxVol - sliderValue);   // slider moves left: p1 fade in, p2 fade out
        double player2Gain = defaultVol * sliderValue;              // slider moves right: p2 fade in, p1 fade out

        // Set the gain for both players
        player1.setGain(player1Gain);
        player2.setGain(player2Gain);
    }
}