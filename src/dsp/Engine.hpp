// Planning stub: public pure-C++ interface. Implementation is forbidden during planning.
#pragma once
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>
namespace rainbows {
struct Parameters { float pitch{0}, primary{0}, secondary{0}, tracking{1}, tone{1}, magic{0}, pitchCv{0}, pitchAttenuverter{0}; bool active{true}, magicEngaged{false}; };
struct Metrics { double realtimeRatio{0}, peakRssBytes{0}, maxAdjacentStep{0}, pitchErrorCents{0}, wetLagMs{0}; };
class Engine {
public:
 void prepare(double sampleRate, std::size_t maximumBlock);
 void reset();
 void setParameters(const Parameters&);
 void process(const float* input, float* output, std::size_t frames);
 Metrics benchmark(const Parameters&, std::size_t seconds, std::size_t blockSize);
 static float pitchSemitones(float knob, float cvVolts, float attenuverter);
 static float secondaryRatio(float pitchSemitones);
 static float trackingMilliseconds(float knob);
 static std::vector<float> renderEventFile(const std::string& path, double sampleRate);
};
class FlexiSwitch {
public:
 void reset(bool initiallyActive=false);
 bool update(bool held, double timeSeconds);
 bool active() const;
};
struct PatchState { Parameters parameters; };
class PatchCodec {
public:
 static std::string serialize(const PatchState&);
 static PatchState deserialize(const std::string&);
};
struct ReferenceSet { static void validate(const std::string& manifestPath); };
namespace detail { [[noreturn]] inline void notImplemented() { throw std::logic_error("not implemented: planning stub"); } }
inline void Engine::prepare(double,std::size_t){detail::notImplemented();} inline void Engine::reset(){detail::notImplemented();} inline void Engine::setParameters(const Parameters&){detail::notImplemented();} inline void Engine::process(const float*,float*,std::size_t){detail::notImplemented();} inline Metrics Engine::benchmark(const Parameters&,std::size_t,std::size_t){detail::notImplemented();} inline float Engine::pitchSemitones(float,float,float){detail::notImplemented();} inline float Engine::secondaryRatio(float){detail::notImplemented();} inline float Engine::trackingMilliseconds(float){detail::notImplemented();} inline std::vector<float> Engine::renderEventFile(const std::string&,double){detail::notImplemented();} inline void FlexiSwitch::reset(bool){detail::notImplemented();} inline bool FlexiSwitch::update(bool,double){detail::notImplemented();} inline bool FlexiSwitch::active() const{detail::notImplemented();} inline std::string PatchCodec::serialize(const PatchState&){detail::notImplemented();} inline PatchState PatchCodec::deserialize(const std::string&){detail::notImplemented();} inline void ReferenceSet::validate(const std::string&){detail::notImplemented();}
}
