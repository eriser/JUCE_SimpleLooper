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
		//Dry成分の保持
		currentDryInput = bufferPtr[audioBuffNum];

		//LoopSwitchのON/OFFで場合分け
		//LoopSwitchがONになった時 (Sampling & Loopの開始, Wet成分の生成)
		if (loopSwitch == 1.0f) {
			//Sampling処理
			sampleBuff[tail] = bufferPtr[audioBuffNum];
			//SampleBuffが満杯になるまで、Samplingを続ける
			//sampleBufferに余裕があれば書き込み位置を更新
			if (tail + 1 < sampleBuffSize) {
				tail++;
			}
			//sampleBuffに空きが無ければ、以降の書きこみ位置をゴミ箱（配列末尾の要素）に固定
			else tail = sampleBuffSize - 1;
			
			//SampleBuffからのloopData生成処理
			//4beatLoop_sampleBuffからSamplingした音を4分音符の長さ分繰り返し取り出す
			beat4LoopOut = sampleBuff[beat4Head];
			if (beat4Head + 1 < beat4length){
				beat4Head++;
			}
			else beat4Head = 0;
			//8beatLoop_ampleBuffからSamplingした音を8分音符の長さ分繰り返し取り出す
			beat8LoopOut = sampleBuff[beat8Head];
			if (beat8Head + 1 < (beat4length / 2)) {
				beat8Head++;
			}
			else beat8Head = 0;
			
			//最終出力音に送る用のLoopDataを選択する処理(beatSwitcing)
			if (beatSwitch == 1.0f){
				selectedLoopOut = beat4LoopOut;
			}
			else selectedLoopOut = beat8LoopOut;
		}
		//LoopSitchがOffになった時 (Sampling Dataのリセット & Loopの停止)
		else{
			//SampleBuffへの書きこみ開始位置、LoopOutへの取り出し開始位置のリセット
			tail = beat4Head = beat8Head = 0;
			selectedLoopOut = currentDryInput;
		}
		//最終出力音の生成 (Dry/Wet)
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
		//入力をDry成分として保持
		currentDryInput = bufferPtr[audioBuffNum];
		//loopSwitchがONになった時、入力をsampleBuffに書きこみ始める（環状バッファにはしない）
		if (loopSwitch == 1.0f && tail < sampleBuffSize){
			sampleBuff[tail] = bufferPtr[audioBuffNum];
			tail++;
		}
		//loopSwitchがOFFの時、書きこみ位置を先頭に固定（書きこみ位置の初期化）
		else if (loopSwitch < 1.0f) {
			tail = 0;
		}
		//4 beat Loop
		//4分音符分の長さを繰り返し、取り出す. loopSwitchがOFFになったら、頭に固定（初期化）
		beat4LoopOut = sampleBuff[beat4Head];
		if (beat4Head + 1 == beat4length || loopSwitch < 1.0f){
			beat4Head = 0;
		}
		else beat4Head++;
		//8 beat Loop
		//8分音符分の長さを繰り返し、取り出す. loopSwitchがOFFになったら、頭に固定（初期化
		beat8LoopOut = sampleBuff[beat8Head];
		if (beat8Head + 1 == beat4length / 2 || loopSwitch < 1.0f){
			beat8Head = 0;
		}
		else beat8Head++;
		//Outputの生成
		if (loopSwitch == 1.0f && beatSwitch == 1.0f){
			Output = (1.0f - effectWet) * currentDryInput + effectWet * beat4LoopOut;
		}
		else if (loopSwitch == 1.0f && beatSwitch < 1.0f) {
			Output = (1.0f - effectWet) * currentDryInput + effectWet * beat8LoopOut;
		}
		else Output = currentDryInput;
		//Outputの書き込み
		bufferPtr[audioBuffNum] = Output;
	}
}
*/