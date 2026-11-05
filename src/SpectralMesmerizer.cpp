#include "SpectralMesmerizer.hpp"

SpectralMesmerizer::SpectralMesmerizer() {
    config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
    configParam(PITCH_PARAM, 0.f, 1.f, 0.5f, "Pitch");
    configParam(PRIMARY_PARAM, 0.f, 1.f, 0.5f, "Primary");
    configParam(SECONDARY_PARAM, 0.f, 1.f, 0.5f, "Secondary");
    configParam(TRACKING_PARAM, 0.f, 1.f, 0.5f, "Tracking");
    configParam(TONE_PARAM, 0.f, 1.f, 1.f, "Tone");
    configParam(MAGIC_PARAM, 0.f, 1.f, 0.f, "Magic");
    configParam(PITCHCV_ATT_PARAM, -1.f, 1.f, 0.f, "Pitch CV amount");
    configParam(ACTIVE_PARAM, 0.f, 1.f, 1.f, "Active");
    configParam(MAGIC_ENGAGE_PARAM, 0.f, 1.f, 0.f, "Magic engage");
    configInput(AUDIO_INPUT, "Audio");
    configInput(PITCHCV_INPUT, "Pitch CV");
    configOutput(AUDIO_OUTPUT, "Audio");
    configLight(ACTIVE_LIGHT, "Active");
    configLight(MAGIC_LIGHT, "Magic engaged");
    configBypass(AUDIO_INPUT, AUDIO_OUTPUT);
    activeSwitch.reset(true);
    magicSwitch.reset(false);
    onSampleRateChange({});
}

void SpectralMesmerizer::onSampleRateChange(const SampleRateChangeEvent& e) {
    const double sr = APP ? APP->engine->getSampleRate() : 48000.0;
    const std::size_t block = APP ? static_cast<std::size_t>(APP->engine->getBlockFrames()) : 256;
    engine.prepare(sr > 0.0 ? sr : 48000.0, block > 0 ? block : 256);
}

void SpectralMesmerizer::process(const ProcessArgs& args) {
    // Flexi switch gestures (D-008): tap toggles, hold is momentary.
    const bool activeHeld = params[ACTIVE_PARAM].getValue() >= 0.5f;
    const bool magicHeld = params[MAGIC_ENGAGE_PARAM].getValue() >= 0.5f;
    uiTime += args.sampleTime;
    const bool active = activeSwitch.update(activeHeld, uiTime);
    const bool magic = magicSwitch.update(magicHeld, uiTime);

    rainbows::Parameters p;
    p.pitch = params[PITCH_PARAM].getValue();
    p.primary = params[PRIMARY_PARAM].getValue();
    p.secondary = params[SECONDARY_PARAM].getValue();
    p.tracking = params[TRACKING_PARAM].getValue();
    p.tone = params[TONE_PARAM].getValue();
    p.magic = params[MAGIC_PARAM].getValue();
    p.pitchAttenuverter = params[PITCHCV_ATT_PARAM].getValue();
    p.pitchCv = inputs[PITCHCV_INPUT].isConnected() ? inputs[PITCHCV_INPUT].getVoltage(0) : 0.f; // channel 0 (D-007)
    p.active = active;
    p.magicEngaged = magic; // Rack bypass (configBypass) carries audio; latched Magic state persists (D-008)
    engine.setParameters(p);

    // Mono sum of poly audio input (D-007).
    float inSum = 0.f;
    const int chans = inputs[AUDIO_INPUT].getChannels();
    for (int ch = 0; ch < chans; ++ch) inSum += inputs[AUDIO_INPUT].getVoltage(ch);
    float in = inSum * 0.1f; // Rack nominal +/-5 V -> engine +/-0.5 headroom
    if (!std::isfinite(in)) in = 0.f;
    float out = 0.f;
    engine.process(&in, &out, 1);

    outputs[AUDIO_OUTPUT].setChannels(1);
    outputs[AUDIO_OUTPUT].setVoltage(out * 10.f, 0); // engine +/-0.5 -> +/-5 V

    lights[ACTIVE_LIGHT].setBrightness(active ? 1.f : 0.f);
    lights[MAGIC_LIGHT].setBrightness(magic ? 1.f : 0.f);
}

json_t* SpectralMesmerizer::dataToJson() {
    rainbows::PatchState st;
    st.parameters.pitch = params[PITCH_PARAM].getValue();
    st.parameters.primary = params[PRIMARY_PARAM].getValue();
    st.parameters.secondary = params[SECONDARY_PARAM].getValue();
    st.parameters.tracking = params[TRACKING_PARAM].getValue();
    st.parameters.tone = params[TONE_PARAM].getValue();
    st.parameters.magic = params[MAGIC_PARAM].getValue();
    st.parameters.pitchAttenuverter = params[PITCHCV_ATT_PARAM].getValue();
    st.parameters.active = activeSwitch.active();
    st.parameters.magicEngaged = magicSwitch.active();
    const std::string s = rainbows::PatchCodec::serialize(st);
    json_error_t err;
    json_t* rootJ = json_loads(s.c_str(), 0, &err);
    return rootJ ? rootJ : json_object();
}

void SpectralMesmerizer::dataFromJson(json_t* rootJ) {
    if (!rootJ) return;
    char* s = json_dumps(rootJ, 0);
    if (!s) return;
    const rainbows::PatchState st = rainbows::PatchCodec::deserialize(s);
    free(s);
    params[PITCH_PARAM].setValue(st.parameters.pitch);
    params[PRIMARY_PARAM].setValue(st.parameters.primary);
    params[SECONDARY_PARAM].setValue(st.parameters.secondary);
    params[TRACKING_PARAM].setValue(st.parameters.tracking);
    params[TONE_PARAM].setValue(st.parameters.tone);
    params[MAGIC_PARAM].setValue(st.parameters.magic);
    params[PITCHCV_ATT_PARAM].setValue(st.parameters.pitchAttenuverter);
    activeSwitch.reset(st.parameters.active);
    magicSwitch.reset(st.parameters.magicEngaged);
}
