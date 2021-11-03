#pragma once

#include <AnalyzerHelpers.h>

class TwiAnalyzerSettings;

class TwiSimulationDataGenerator
{
  public:
      TwiSimulationDataGenerator();
      virtual ~TwiSimulationDataGenerator();

      void Initialize(U32, TwiAnalyzerSettings*);
      U32 GenerateSimulationData(U64, U32, SimulationChannelDescriptor**);

  protected:
      TwiAnalyzerSettings* mSettings;
      U32 mSimulationSampleRateHz;

      ClockGenerator mClockGenerator;
      SimulationChannelDescriptor mTwiSimulationData;

      U8 mValue;

      void CreateSignature();
      void CreateByte(U8);
};
