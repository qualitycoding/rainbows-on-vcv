RACK_DIR ?= .toolchain/Rack-SDK
FLAGS += -Isrc
SOURCES += src/plugin.cpp
SOURCES += src/SpectralMesmerizer.cpp
SOURCES += src/SpectralMesmerizerWidget.cpp
SOURCES += src/dsp/Engine.cpp
SOURCES += src/dsp/FlexiSwitch.cpp
DISTRIBUTABLES += res
include $(RACK_DIR)/plugin.mk
