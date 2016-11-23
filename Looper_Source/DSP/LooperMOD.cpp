#define _USE_MATH_DEFINE
#include "LooperMOD.h"
#include <math.h>

LooperMOD::LooperMOD() {
	loopSwitch = beatSwitch = effectWet = 0.0f;
	beat4length = 0;
	tail = beat4Head = beat8Head = 0;
	sampleBuffSize = 42100 * 10;
	for (int a = 0; a < sampleBuffSize; a++) {
		sampleBuff[a] = 0.0f;
	}
	currentDryInput = beat4LoopOut = beat8LoopOut = selectedLoopOut = 0.0f;
}

LooperMOD::~LooperMOD() {}

void LooperMOD::SetLooperParams
(float hLoopSwitch, float hBeatSwitch, float hEffectWet, double hBPM, double hSampleRate)
{
	loopSwitch = hLoopSwitch; //1.0f -> ON, other -> OFF
	beatSwitch = hBeatSwitch; //1.0f -> 4 beat. other -> 8 beat
	effectWet = hEffectWet; //0 <-> 1.0f, Dry <-> Wet
	beat4length = (60.0 / hBPM) * hSampleRate; //[sec] * [samples] / [sec]
}

void LooperMOD::LooperProcessing(float* bufferPtr, int audioBuffSize)
{
	for (int audioBuffNum = 0; audioBuffNum < audioBuffSize; audioBuffNum++)
	{
		//Dry�����̕ێ�
		currentDryInput = bufferPtr[audioBuffNum];

		//LoopSwitch��ON/OFF�ŏꍇ����
		//LoopSwitch��ON�ɂȂ����� (Sampling & Loop�̊J�n, Wet�����̐���)
		if (loopSwitch == 1.0f) {
			//Sampling����
			sampleBuff[tail] = bufferPtr[audioBuffNum];
			//SampleBuff�����t�ɂȂ�܂ŁASampling�𑱂���
			//sampleBuffer�ɗ]�T������Ώ������݈ʒu���X�V
			if (tail + 1 < sampleBuffSize) {
				tail++;
			}
			//sampleBuff�ɋ󂫂�������΁A�ȍ~�̏������݈ʒu���S�~���i�z�񖖔��̗v�f�j�ɌŒ�
			else tail = sampleBuffSize - 1;
			
			//SampleBuff�����loopData��������
			//4beatLoop_sampleBuff����Sampling��������4�������̒������J��Ԃ����o��
			beat4LoopOut = sampleBuff[beat4Head];
			if (beat4Head + 1 < beat4length){
				beat4Head++;
			}
			else beat4Head = 0;
			//8beatLoop_ampleBuff����Sampling��������8�������̒������J��Ԃ����o��
			beat8LoopOut = sampleBuff[beat8Head];
			if (beat8Head + 1 < (beat4length / 2)) {
				beat8Head++;
			}
			else beat8Head = 0;
			
			//�ŏI�o�͉��ɑ���p��LoopData��I�����鏈��(beatSwitcing)
			if (beatSwitch == 1.0f){
				selectedLoopOut = beat4LoopOut;
			}
			else selectedLoopOut = beat8LoopOut;
		}
		//LoopSitch��Off�ɂȂ����� (Sampling Data�̃��Z�b�g & Loop�̒�~)
		else{
			//SampleBuff�ւ̏������݊J�n�ʒu�ALoopOut�ւ̎��o���J�n�ʒu�̃��Z�b�g
			tail = beat4Head = beat8Head = 0;
			selectedLoopOut = currentDryInput;
		}
		//�ŏI�o�͉��̐��� (Dry/Wet)
		bufferPtr[audioBuffNum] = (1.0f - effectWet)*currentDryInput + effectWet*selectedLoopOut;
	}
}
/*
LooperMOD::LooperMOD() {
	loopSwitch = beatSwitch = effectWet = 0.0f;
	beat4length = 0;
	tail = beat4Head = beat8Head = 0;
	sampleBuffSize = 42100 * 10;
	for (int a = 0; a < sampleBuffSize; a++){
		sampleBuff[a] = 0.0f;
	}
	currentDryInput = beat4LoopOut = beat8LoopOut = Output = 0.0f;
}

LooperMOD::~LooperMOD() {}

void LooperMOD::SetLooperParams
(float hLoopSwitch, float hBeatSwitch, float hEffectWet, double hBPM, double hSampleRate)
{
	loopSwitch = hLoopSwitch; //1.0f -> ON, other -> OFF
	beatSwitch = hBeatSwitch; //1.0f -> 4 beat. other -> 8 beat
	effectWet = hEffectWet; //0 <-> 1.0f, Dry <-> Wet
	beat4length = 21050;//(60.0 / hBPM) * hSampleRate; //[sec] * [samples] / [sec]
}

void LooperMOD::LooperProcessing(float* bufferPtr, int bufferSize)
{
	for (int audioBuffNum = 0; audioBuffNum < bufferSize; audioBuffNum++)
	{
		//���͂�Dry�����Ƃ��ĕێ�
		currentDryInput = bufferPtr[audioBuffNum];
		//loopSwitch��ON�ɂȂ������A���͂�sampleBuff�ɏ������ݎn�߂�i��o�b�t�@�ɂ͂��Ȃ��j
		if (loopSwitch == 1.0f && tail < sampleBuffSize){
			sampleBuff[tail] = bufferPtr[audioBuffNum];
			tail++;
		}
		//loopSwitch��OFF�̎��A�������݈ʒu��擪�ɌŒ�i�������݈ʒu�̏������j
		else if (loopSwitch < 1.0f) {
			tail = 0;
		}
		//4 beat Loop
		//4���������̒������J��Ԃ��A���o��. loopSwitch��OFF�ɂȂ�����A���ɌŒ�i�������j
		beat4LoopOut = sampleBuff[beat4Head];
		if (beat4Head + 1 == beat4length || loopSwitch < 1.0f){
			beat4Head = 0;
		}
		else beat4Head++;
		//8 beat Loop
		//8���������̒������J��Ԃ��A���o��. loopSwitch��OFF�ɂȂ�����A���ɌŒ�i������
		beat8LoopOut = sampleBuff[beat8Head];
		if (beat8Head + 1 == beat4length / 2 || loopSwitch < 1.0f){
			beat8Head = 0;
		}
		else beat8Head++;
		//Output�̐���
		if (loopSwitch == 1.0f && beatSwitch == 1.0f){
			Output = (1.0f - effectWet) * currentDryInput + effectWet * beat4LoopOut;
		}
		else if (loopSwitch == 1.0f && beatSwitch < 1.0f) {
			Output = (1.0f - effectWet) * currentDryInput + effectWet * beat8LoopOut;
		}
		else Output = currentDryInput;
		//Output�̏�������
		bufferPtr[audioBuffNum] = Output;
	}
}
*/