#include "SpectralMesmerizer.hpp"

struct SpectralMesmerizerWidget : ModuleWidget {
    explicit SpectralMesmerizerWidget(SpectralMesmerizer* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/SpectralMesmerizer.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        const float xL = 30.f, xR = 75.f;
        addParam(createParamCentered<RoundBlackKnob>(Vec(xL, 90.f), module, SpectralMesmerizer::PITCH_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(Vec(xR, 90.f), module, SpectralMesmerizer::TRACKING_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(Vec(xL, 150.f), module, SpectralMesmerizer::PRIMARY_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(Vec(xR, 150.f), module, SpectralMesmerizer::SECONDARY_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(Vec(xL, 210.f), module, SpectralMesmerizer::TONE_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(Vec(xR, 210.f), module, SpectralMesmerizer::MAGIC_PARAM));
        addParam(createParamCentered<Trimpot>(Vec(xL, 255.f), module, SpectralMesmerizer::PITCHCV_ATT_PARAM));
        addParam(createParamCentered<LEDButton>(Vec(xL, 300.f), module, SpectralMesmerizer::ACTIVE_PARAM));
        addParam(createParamCentered<LEDButton>(Vec(xR, 300.f), module, SpectralMesmerizer::MAGIC_ENGAGE_PARAM));
        addChild(createLightCentered<MediumLight<GreenLight>>(Vec(xL, 285.f), module, SpectralMesmerizer::ACTIVE_LIGHT));
        addChild(createLightCentered<MediumLight<RedLight>>(Vec(xR, 285.f), module, SpectralMesmerizer::MAGIC_LIGHT));

        addInput(createInputCentered<PJ301MPort>(Vec(xL, 345.f), module, SpectralMesmerizer::AUDIO_INPUT));
        addInput(createInputCentered<PJ301MPort>(Vec(xR, 345.f), module, SpectralMesmerizer::PITCHCV_INPUT));
        addOutput(createOutputCentered<PJ301MPort>(Vec((xL + xR) / 2.f, 385.f), module, SpectralMesmerizer::AUDIO_OUTPUT));
    }
};

Model* modelSpectralMesmerizer = createModel<SpectralMesmerizer, SpectralMesmerizerWidget>("SpectralMesmerizer");
