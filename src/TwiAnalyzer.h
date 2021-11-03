#pragma once

#include<Analyzer.h>
#include "TwiAnalyzerResults.h"
#include "TwiSimulationDataGenerator.h"

class TwiAnalyzerSettings;
class TwiAnalyzer : public Analyzer2
{
  public:
      TwiAnalyzer();
      virtual ~TwiAnalyzer();
      virtual void SetupResults();
      virtual void WorkerThread();

      virtual U32 GenerateSimulationData(U64, U32, SimulationChannelDescriptor**);
      virtual U32 GetMinimumSampleRateHz();

      virtual const char* GetAnalyzerName() const;
      virtual bool NeedsRerun();

  protected:
      void ComputeSampleOffsets();

      std::unique_ptr<TwiAnalyzerSettings> mSettings;
      std::unique_ptr<TwiAnalyzerResults> mResults;
      AnalyzerChannelData* mTwi;

      TwiSimulationDataGenerator mSimulationDataGenerator;
      bool mSimulationInitialized;

      U32 mSampleRateHz;
      std::vector<U32> mSampleOffsets;
      U32 mParityBitOffset;
      U32 mStartOfStopBitOffset;
      U32 mEndOfStopBitOffset;
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer();
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer(Analyzer*);
