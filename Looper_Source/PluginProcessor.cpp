/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
SampleLooperAudioProcessor::SampleLooperAudioProcessor()
{
	UserParams[LoopSwitch] = 0.0f;
	UserParams[BeatSwitch] = 1.0f;
	UserParams[DryWet] = 1.0f;
}

SampleLooperAudioProcessor::~SampleLooperAudioProcessor()
{
}

//==============================================================================
const String SampleLooperAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SampleLooperAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SampleLooperAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double SampleLooperAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SampleLooperAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SampleLooperAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SampleLooperAudioProcessor::setCurrentProgram (int index)
{
}

const String SampleLooperAudioProcessor::getProgramName (int index)
{
    return String();
}

void SampleLooperAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void SampleLooperAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void SampleLooperAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SampleLooperAudioProcessor::setPreferredBusArrangement (bool isInput, int bus, const AudioChannelSet& preferredSet)
{
    // Reject any bus arrangements that are not compatible with your plugin

    const int numChannels = preferredSet.size();

   #if JucePlugin_IsMidiEffect
    if (numChannels != 0)
        return false;
   #elif JucePlugin_IsSynth
    if (isInput || (numChannels != 1 && numChannels != 2))
        return false;
   #else
    if (numChannels != 1 && numChannels != 2)
        return false;

    if (! AudioProcessor::setPreferredBusArrangement (! isInput, bus, preferredSet))
        return false;
   #endif

    return AudioProcessor::setPreferredBusArrangement (isInput, bus, preferredSet);
}
#endif

//============================================================================================
int SampleLooperAudioProcessor::getNumParameters() { return totalNumParams; }

float SampleLooperAudioProcessor::getParameter(int index) {
	if (index >= 0 && index < totalNumParams)
		return UserParams[index];
	else return 0;
}

void SampleLooperAudioProcessor::setParameter(int index, float newValue)
{
	if (index >= 0 && index < totalNumParams){
		UserParams[index] = newValue;
	}
	else return;
}

const String SampleLooperAudioProcessor::getParameterName(int index)
{
	switch (index) {
	case LoopSwitch: return "Loop Switch";
	case BeatSwitch: return "Beat Switch";
	case DryWet: return "Dry <-> Wet";
	default:return String::empty;
	}
}

const String SampleLooperAudioProcessor::getParameterText(int index) {
	switch (index) {
	case LoopSwitch: return UserParams[LoopSwitch] == 1.0f ? "ON" : "OFF";
	case BeatSwitch: return UserParams[BeatSwitch] == 1.0f ? "4 Beat" : "8 Beat";
	case DryWet: return String(UserParams[DryWet] * 100.0f, 1);
	default:return String::empty;
	}
}

void SampleLooperAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();

    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
		playHead = this->getPlayHead();
		playHead->getCurrentPosition(currentPositionInfo);
		double _BPM = currentPositionInfo.bpm;

		LooperMOD[channel].SetLooperParams
		(UserParams[LoopSwitch], UserParams[BeatSwitch], UserParams[DryWet], _BPM, getSampleRate());
		LooperMOD[channel].LooperProcessing(buffer.getWritePointer(channel), buffer.getNumSamples());
    }
}

//==============================================================================
bool SampleLooperAudioProcessor::hasEditor() const
{
    return false; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* SampleLooperAudioProcessor::createEditor()
{
    return new SampleLooperAudioProcessorEditor (*this);
}

//==============================================================================
void SampleLooperAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SampleLooperAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SampleLooperAudioProcessor();
}
