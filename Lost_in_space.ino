#include "DaisyDuino.h"

#define MAX_DELAY static_cast<size_t>(48000 * 1.f)

static DaisyHardware petal;

// single delay line in SDRAM like the example
DelayLine<float, MAX_DELAY> DSY_SDRAM_BSS delMem;

struct SingleDelay
{
    DelayLine<float, MAX_DELAY>* del;
    float currentDelay;
    float delayTarget;
    float feedback;

    float Process(float in)
    {
        // Smooth delay time to avoid zipper noise/clicks
        fonepole(currentDelay, delayTarget, 0.0002f);
        del->SetDelay(currentDelay);

        float read = del->Read();

        // Safety: keep feedback tame (also prevents "burned" runaway)
        float fb = feedback;
        if(fb < 0.f) fb = 0.f;
        if(fb > 0.85f) fb = 0.85f;

        // Write input + feedback
        del->Write((fb * read) + in);

        return read;
    }
};

SingleDelay dly;
Parameter  timeParam;

bool effectOn = true; // effect engine enabled (FS1 toggles)
bool bypass   = true; // bypass state used for output mix (effect off = dry)

// Control processing
static void ProcessControls(float samplerate, float &drywet, bool &engagedMode)
{
    petal.ProcessAllControls();

    // FS1 toggles effect enable/bypass
    if(petal.buttons[0].RisingEdge())
        bypass = !bypass;

    // SW gating: only run delay when SW1 ON and SW2/3/4 OFF
    bool sw1 = petal.buttons[2].Pressed();
    bool sw2 = petal.buttons[3].Pressed();
    bool sw3 = petal.buttons[4].Pressed();
    bool sw4 = petal.buttons[5].Pressed();

    engagedMode = (sw1 && !sw2 && !sw3 && !sw4);

    // POT1 controls delay time (log-ish using Parameter like the example)
    dly.delayTarget = timeParam.Process();

    // POT2 feedback, clamp later in Process()
    dly.feedback = petal.controls[1].Value();

    // POT5 global dry/wet
    drywet = petal.controls[4].Value();
}

static void AudioCallback(float **in, float **out, size_t size)
{
    float drywet = 0.5f;
    bool  engagedMode = false;

    ProcessControls(DAISY.AudioSampleRate(), drywet, engagedMode);

    for(size_t i = 0; i < size; i++)
    {
        // mono in -> stereo out like the example
        float input = in[0][i];

        float wet = 0.f;

        // Only process delay if NOT bypassed and switches match the "delay mode"
        if(!bypass && engagedMode)
        {
            wet = dly.Process(input);
        }
        else
        {
            // Still keep delay line from going stale/popping when you re-enable:
            // write dry input with zero feedback when not active
            dly.del->Write(input);
            wet = dly.del->Read();
        }

        // Attenuate wet path to avoid overload/"burned" harshness
        float wetAtten = 0.30f;

        float mix = (bypass || !engagedMode)
                        ? input
                        : (drywet * wet * wetAtten + (1.0f - drywet) * input);

        out[0][i] = mix;
        out[1][i] = mix;
    }
}

void setup()
{
    petal = DAISY.init(DAISY_PETAL, AUDIO_SR_48K);

    // GPIO LEDs you discovered
    pinMode(22, OUTPUT);
    pinMode(23, OUTPUT);

    float samplerate = DAISY.AudioSampleRate();

    // init delay line
    delMem.Init();
    dly.del = &delMem;
    dly.currentDelay = samplerate * 0.25f; // start ~250ms at 48k
    dly.delayTarget  = dly.currentDelay;
    dly.feedback     = 0.35f;

    // Parameter for delay time (same idea as MultiDelay)
    // Min = 50ms, Max = 1s
    timeParam.Init(petal.controls[0],
                   samplerate * 0.05f,
                   MAX_DELAY,
                   Parameter::LOGARITHMIC);

    bypass = true;

    DAISY.begin(AudioCallback);
}

void loop()
{
    // LEDs follow effect/bypass state (active-low wiring)
    // LED ON when effect is active (not bypassed)
    digitalWrite(22, bypass ? LOW : HIGH);
    digitalWrite(23, bypass ? LOW : HIGH);

    delay(6);
}