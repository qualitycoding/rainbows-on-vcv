#pragma once
#include "plugin.hpp"
#include "dsp/Engine.hpp"

struct SpectralMesmerizer : Module {
    enum ParamId {
        PITCH_PARAM,
        PRIMARY_PARAM,
        SECONDARY_PARAM,
        TRACKING_PARAM,
        TONE_PARAM,
        MAGIC_PARAM,
        PITCHCV_ATT_PARAM,
        ACTIVE_PARAM,
        MAGIC_ENGAGE_PARAM,
        PARAMS_LEN
    };
    enum InputId {
        AUDIO_INPUT,
        PITCHCV_INPUT,
        INPUTS_LEN
    };
    enum OutputId {
        AUDIO_OUTPUT,
        OUTPUTS_LEN
    };
    enum LightId {
        ACTIVE_LIGHT,
        MAGIC_LIGHT,
        LIGHTS_LEN
    };

    rainbows::Engine engine;
    rainbows::FlexiSwitch activeSwitch;
    rainbows::FlexiSwitch magicSwitch;
    double uiTime = 0.0;
    dsp::ClockDivider paramDivider;
    bool bypassed = false;

    SpectralMesmerizer();

    void onSampleRateChange(const SampleRateChangeEvent& e) override;
    void process(const ProcessArgs& args) override;
    json_t* dataToJson() override;
    void dataFromJson(json_t* rootJ) override;
};
