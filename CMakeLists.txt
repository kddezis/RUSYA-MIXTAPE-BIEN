cmake_minimum_required(VERSION 3.22)
project(RusyaMixtapeMamaguevo VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "")

include(FetchContent)
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.4
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)

if(DEFINED ENV{CI})
    set(COPY_AFTER FALSE)
else()
    set(COPY_AFTER TRUE)
endif()

juce_add_plugin(RusyaMixtape
    PRODUCT_NAME "RUSYA MIXTAPE MAMAGUEVO"
    COMPANY_NAME "VNZLA"
    PLUGIN_MANUFACTURER_CODE Vnzl
    PLUGIN_CODE Rsmx
    FORMATS VST3
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    VST3_CATEGORIES Fx Distortion
    COPY_PLUGIN_AFTER_BUILD ${COPY_AFTER})

juce_add_binary_data(RusyaAssets SOURCES
    Assets/moscow.jpg
    Assets/kddezis_logo.png
    Assets/rusya_mixtape_logo.png
    Assets/suka_blyat.png
    Assets/bottle.png
    Assets/vnzla.png)

target_sources(RusyaMixtape PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h)

target_compile_definitions(RusyaMixtape PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(RusyaMixtape
    PRIVATE
        RusyaAssets
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

juce_generate_juce_header(RusyaMixtape)
