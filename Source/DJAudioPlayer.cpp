/*
  ==============================================================================

    DJAudioPlayer.cpp
    Created: 25 Feb 2025 2:54:12pm
    Author:  jcgy0

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DJAudioPlayer.h"

//==============================================================================
DJAudioPlayer::DJAudioPlayer(AudioFormatManager& _formatManager)
                            : formatManager(_formatManager),
                              isPlaying(false)
{

}
DJAudioPlayer::~DJAudioPlayer()
{

}

//==============================================================================
void DJAudioPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void DJAudioPlayer::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    resampleSource.getNextAudioBlock(bufferToFill);
}

void DJAudioPlayer::releaseResources()
{
    transportSource.releaseResources();
    resampleSource.releaseResources();
}

void DJAudioPlayer::loadURL(URL audioURL)
{
    auto* reader = formatManager.createReaderFor(audioURL.createInputStream(false));
    if (reader != nullptr) // good file!
    {
        std::unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader, true));
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());
    }
}

void DJAudioPlayer::setGain(double gain)
{
    if (gain < 0 || gain > 2.0)
    {
        DBG("DJAudioPlayer::setGain gain should be between 0 and 2.");
    }
    else
    {
        transportSource.setGain(gain);
    }
}

double DJAudioPlayer::getGain() 
{
    return transportSource.getGain();
}

void DJAudioPlayer::setSpeed(double ratio)
{
    if (ratio < 0.1 || ratio > 3.0)
    {
        DBG("DJAudioPlayer::setSpeed ratio should be between 0.1 and 3.");
    }
    else
    {
        resampleSource.setResamplingRatio(ratio);
    }
}

void DJAudioPlayer::setPosition(double posInSecs)
{
    transportSource.setPosition(posInSecs);
}

void DJAudioPlayer::setPositionRelative(double pos)
{
    if (pos < 0 || pos > 1.0)
    {
        DBG("DJAudioPlayer::setPositionRelative pos should be between 0 and 1.");
    }
    else
    {
        double posInSecs = transportSource.getLengthInSeconds() * pos;
        setPosition(posInSecs);
    }
}

void DJAudioPlayer::start()
{
    transportSource.start();
    isPlaying = true;
}

void DJAudioPlayer::stop()
{
    transportSource.stop();
    isPlaying = false;
}

double DJAudioPlayer::getPositionRelative()
{
    return transportSource.getCurrentPosition() / transportSource.getLengthInSeconds();
}