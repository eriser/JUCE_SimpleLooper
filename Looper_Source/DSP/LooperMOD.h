#ifndef LOOPERMOD_H_INCLUDED
#define LOOPERMOD_H_INCLUDED

class LooperMOD
{
public:
	LooperMOD();
	~LooperMOD();

	void SetLooperParams(float hLoopSwitch, float hBeatSwitch, float hEffectWet, double hBPM, double hSampleRate);
	void LooperProcessing(float* bufferPtr, int bufferSize);

private:
	//SetLooperParams
	float loopSwitch, beatSwitch, effectWet;
	int beat4length;
	//LooperProcessing
	int sampleBuffSize, tail, beat4Head, beat8Head;
	float sampleBuff[42100 * 10], currentDryInput, beat4LoopOut, beat8LoopOut, selectedLoopOut;	//42.1 kHz * 10 sec [samples]
};

#endif  // LOOPERMOD_H_INCLUDED
