#include "DaisyDuino.h"
#include <math.h>

static DaisyHardware petal;

#define CHORUS_BUFFER 8192

float chorusBufferL[CHORUS_BUFFER];
float chorusBufferR[CHORUS_BUFFER];
int writeIndex = 0;

bool effectOn = false;

// low-band filter states
float lowStateL = 0.0f;
float lowStateR = 0.0f;

// very simple bright emphasis states
float prevHighL = 0.0f;
float prevHighR = 0.0f;

float Lowpass(float input, float &state, float amount)
{
  state += amount * (input - state);
  return state;
}

static void AudioCallback(float **in, float **out, size_t size)
{
  petal.ProcessAllControls();

  if(petal.buttons[0].RisingEdge())
  {
    effectOn = !effectOn;
  }

  float rateKnob      = petal.controls[0].Value(); // POT1
  float depthKnob     = petal.controls[1].Value(); // POT2
  float bassCleanKnob = petal.controls[2].Value(); // POT3
  float intensityKnob = petal.controls[3].Value(); // POT4
  float dryWet        = petal.controls[4].Value(); // POT5
  float widthKnob     = petal.controls[5].Value(); // POT6

  float sampleRate = DAISY.AudioSampleRate();

  // more active ranges
  float lfoRate      = 0.25f + rateKnob * 2.8f;
  float depthSamples = 6.0f + depthKnob * 28.0f;
  float baseDelay    = 8.0f + widthKnob * 30.0f;

  // higher knob = more bass stays clean
  float lowpassAmt   = 0.002f + bassCleanKnob * 0.030f;

  // stronger chorus blend
  float highDry      = 0.45f - intensityKnob * 0.30f;   // 0.45 -> 0.15
  float highWet      = 0.75f + intensityKnob * 0.95f;   // 0.75 -> 1.70

  // allow stronger final mix
  float finalMix     = 0.20f + dryWet * 0.80f;

  static float phase = 0.0f;

  for(size_t i = 0; i < size; i++)
  {
    float inL = in[0][i];
    float inR = in[1][i];

    float outLsig = inL;
    float outRsig = inR;

    if(effectOn)
    {
      // keep low end more stable
      float lowL = Lowpass(inL, lowStateL, lowpassAmt);
      float lowR = Lowpass(inR, lowStateR, lowpassAmt);

      float highL = inL - lowL;
      float highR = inR - lowR;

      // brighten the chorused band a bit
      float brightL = highL + 0.45f * (highL - prevHighL);
      float brightR = highR + 0.45f * (highR - prevHighR);

      prevHighL = highL;
      prevHighR = highR;

      float lfoL = sinf(phase);
      float lfoR = sinf(phase + 2.0943951f);

      float delayL = baseDelay + depthSamples * lfoL;
      float delayR = baseDelay + depthSamples * lfoR;

      int readIndexL = writeIndex - (int)delayL;
      int readIndexR = writeIndex - (int)delayR;

      if(readIndexL < 0) readIndexL += CHORUS_BUFFER;
      if(readIndexR < 0) readIndexR += CHORUS_BUFFER;

      float chorL = chorusBufferL[readIndexL];
      float chorR = chorusBufferR[readIndexR];

      chorusBufferL[writeIndex] = brightL;
      chorusBufferR[writeIndex] = brightR;

      float wetL = lowL + (brightL * highDry) + (chorL * highWet);
      float wetR = lowR + (brightR * highDry) + (chorR * highWet);

      // slight trim to avoid harsh overload
      wetL *= 0.78f;
      wetR *= 0.78f;

      outLsig = inL * (1.0f - finalMix) + wetL * finalMix;
      outRsig = inR * (1.0f - finalMix) + wetR * finalMix;

      phase += 2.0f * 3.14159265f * lfoRate / sampleRate;
      if(phase > 2.0f * 3.14159265f)
        phase -= 2.0f * 3.14159265f;
    }

    out[0][i] = outLsig;
    out[1][i] = outRsig;

    writeIndex++;
    if(writeIndex >= CHORUS_BUFFER)
      writeIndex = 0;
  }
}

void setup()
{
  petal = DAISY.init(DAISY_PETAL, AUDIO_SR_48K);

  pinMode(22, OUTPUT);
  pinMode(23, OUTPUT);

  for(int i = 0; i < CHORUS_BUFFER; i++)
  {
    chorusBufferL[i] = 0.0f;
    chorusBufferR[i] = 0.0f;
  }

  DAISY.begin(AudioCallback);
}

void loop()
{
  digitalWrite(22, effectOn ? LOW : HIGH);
  digitalWrite(23, effectOn ? LOW : HIGH);

  delay(6);
}
