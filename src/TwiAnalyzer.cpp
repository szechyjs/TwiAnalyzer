#include "TwiAnalyzer.h"
#include "TwiAnalyzerSettings.h"
#include <AnalyzerChannelData.h>
#include <AnalyzerHelpers.h>
#include <cstdlib>

TwiAnalyzer::TwiAnalyzer()
: Analyzer2(),
  mSettings(new TwiAnalyzerSettings()),
  mSimulationInitialized(false)
{
    SetAnalyzerSettings(mSettings.get());
}

TwiAnalyzer::~TwiAnalyzer()
{
    KillThread();
}

void TwiAnalyzer::ComputeSampleOffsets()
{
    ClockGenerator clockGen;
    clockGen.Init(4000, mSampleRateHz);

    mSampleOffsets.clear();

    U32 numBits = 8;

    mSampleOffsets.push_back(clockGen.AdvanceByHalfPeriod(0.5)); // point to center of start bit
    numBits--;

    for (U32 i = 0; i <= numBits; i++)
    {
        mSampleOffsets.push_back(clockGen.AdvanceByHalfPeriod());
    }

    mParityBitOffset = clockGen.AdvanceByHalfPeriod();
    mStartOfStopBitOffset = clockGen.AdvanceByHalfPeriod();
    mEndOfStopBitOffset = clockGen.AdvanceByHalfPeriod(3);
}

void TwiAnalyzer::SetupResults()
{
    mResults.reset(new TwiAnalyzerResults(this, mSettings.get()));
    SetAnalyzerResults(mResults.get());
    mResults->AddChannelBubblesWillAppearOn(mSettings->mInputChannel);
}

void TwiAnalyzer::WorkerThread()
{
    mSampleRateHz = GetSampleRate();
    ComputeSampleOffsets();

    ClockGenerator clockGen;
    clockGen.Init(4000, mSampleRateHz);

    mTwi = GetAnalyzerChannelData(mSettings->mInputChannel);

    if (mTwi->GetBitState() == BIT_LOW)
        mTwi->AdvanceToNextEdge();

    for (;;)
    {
        // we start HIGH
        mTwi->AdvanceToNextEdge();  // should be LOW now

        // starting a frame
        U64 frameStartingSample = mTwi->GetSampleNumber();
        U64 data = 0;
        bool parityError = false;
        bool framingError = false;
        U8 frameType = 0;

        U32 timeSpan = mTwi->GetSampleOfNextEdge() - frameStartingSample;

        if (std::abs(int(timeSpan - clockGen.AdvanceByTimeS(0.100))) < 20)
        {
            // signature
            mTwi->Advance(clockGen.AdvanceByTimeS(0.05));
            U64 marker = mTwi->GetSampleNumber();
            mResults->AddMarker(marker, AnalyzerResults::Start, mSettings->mInputChannel);
            mTwi->AdvanceToNextEdge();
            frameType = SIGNATURE_TYPE;
        }
        else
        {
          // start bit
          DataBuilder dataBuilder;
          dataBuilder.Reset(&data, AnalyzerEnums::MsbFirst, 8);
          U64 markerLoc = frameStartingSample;
          frameType = DATA_TYPE;

          mTwi->Advance(mSampleOffsets[0]);
          markerLoc += mSampleOffsets[0];
          mResults->AddMarker(markerLoc, AnalyzerResults::Start, mSettings->mInputChannel);

          for (U32 i = 1; i <= 8; i++)
          {
              mTwi->Advance(mSampleOffsets[i]);
              dataBuilder.AddBit(mTwi->GetBitState());

              markerLoc += mSampleOffsets[i];
              mResults->AddMarker(markerLoc, AnalyzerResults::Dot, mSettings->mInputChannel);
          }

          parityError = false;
          mTwi->Advance(mParityBitOffset);
          bool isEven = AnalyzerHelpers::IsEven(AnalyzerHelpers::GetOnesCount(data));

          if (isEven)
          {
            if (mTwi->GetBitState() != BIT_LOW) // we expect a low bit to keep parity even
                parityError = true;
          }
          else
          {
            if (mTwi->GetBitState() != BIT_HIGH) // we expect a high bit, to force parity odd
                parityError = true;
          }

          markerLoc += mParityBitOffset;
          mResults->AddMarker(markerLoc, AnalyzerResults::Square, mSettings->mInputChannel);

          framingError = false;

          mTwi->Advance(mStartOfStopBitOffset);
          if (mTwi->GetBitState() != BIT_LOW)
          {
              framingError = true;
          }
          else
          {
              U32 numEdges = mTwi->Advance(mEndOfStopBitOffset);
              if (numEdges != 0)
                  framingError = true;
          }

          if (framingError)
          {
              markerLoc += mStartOfStopBitOffset;
              mResults->AddMarker(markerLoc, AnalyzerResults::ErrorX, mSettings->mInputChannel);

              markerLoc += mEndOfStopBitOffset;
              mResults->AddMarker(markerLoc, AnalyzerResults::ErrorX, mSettings->mInputChannel);
          }
        }

        Frame frame;
        frame.mStartingSampleInclusive = frameStartingSample;
        frame.mEndingSampleInclusive = mTwi->GetSampleNumber();
        frame.mData1 = data;
        frame.mFlags = 0;
        frame.mType = frameType;

        if (parityError)
            frame.mFlags |= PARITY_ERROR_FLAG | DISPLAY_AS_ERROR_FLAG;
        if (framingError)
            frame.mFlags |= FRAMING_ERROR_FLAG | DISPLAY_AS_ERROR_FLAG;

        mResults->AddFrame(frame);
        mResults->CommitResults();

        ReportProgress(frame.mEndingSampleInclusive);
        CheckIfThreadShouldExit();

        if (mTwi->GetBitState() == BIT_LOW)
            mTwi->AdvanceToNextEdge();
    }
}

bool TwiAnalyzer::NeedsRerun()
{
    return false;
}

U32 TwiAnalyzer::GenerateSimulationData(U64 minSampleIndex, U32 devSampleRate, SimulationChannelDescriptor** simChannels)
{
    if (mSimulationInitialized == false)
    {
        mSimulationDataGenerator.Initialize(GetSimulationSampleRate(), mSettings.get());
        mSimulationInitialized = true;
    }

    return mSimulationDataGenerator.GenerateSimulationData(minSampleIndex, devSampleRate, simChannels);
}

U32 TwiAnalyzer::GetMinimumSampleRateHz()
{
    return 4000 * 4 ;
}

const char* TwiAnalyzer::GetAnalyzerName() const
{
    return "Three Wire";
}

const char* GetAnalyzerName()
{
    return "Three Wire";
}

Analyzer* CreateAnalyzer()
{
    return new TwiAnalyzer();
}

void DestroyAnalyzer(Analyzer* analyzer)
{
    delete analyzer;
}
