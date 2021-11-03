cmake_minimum_required(VERSION 3.11)
project(TwiAnalyzer)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(ExternalAnalyzerSDK)

set(SOURCES
  src/TwiAnalyzer.cpp
  src/TwiAnalyzerSettings.cpp
  src/TwiAnalyzerResults.cpp
  src/TwiSimulationDataGenerator.cpp
)

add_analyzer_plugin(twi_analyzer SOURCES ${SOURCES})

# install_name_tool -change @executable_path/libAnalyzer.dylib @rpath/libAnalyzer.dylib libtwi_analyzer.so
