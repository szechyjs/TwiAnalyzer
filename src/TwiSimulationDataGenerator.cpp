#include "TwiSimulationDataGenerator.h"
#include "TwiAnalyzerSettings.h"

TwiSimulationDataGenerator::TwiSimulationDataGenerator()
: mValue(0)
{

}

TwiSimulationDataGenerator::~TwiSimulationDataGenerator()
{

}

void TwiSimulationDataGenerator::Initialize(U32 simSampleRate, TwiAnalyzerSettings* settings)
{
    mSimulationSampleRateHz = simSampleRate;
    mSettings = settings;

    mClockGenerator.Init(4000, simSampleRate);
    mTwiSimulationData.SetChannel(mSettings->mInputChannel);
    mTwiSimulationData.SetSampleRate(simSampleRate);

    mTwiSimulationData.SetInitialBitState(BIT_HIGH);
    mTwiSimulationData.Advance(mClockGenerator.AdvanceByTimeS(0.5));

    CreateSignature();
}

U32 TwiSimulationDataGenerator::GenerateSimulationData(U64 largestSampleReq, U32 sampleRate, SimulationChannelDescriptor** simChannel)
{
    U64 adjustedLargestSampleReq = AnalyzerHelpers::AdjustSimulationTargetSample(largestSampleReq, sampleRate, mSimulationSampleRateHz);

    while (mTwiSimulationData.GetCurrentSampleNumber() < adjustedLargestSampleReq)
    {
        CreateByte(mValue++);
    }

    *simChannel = &mTwiSimulationData;

    return 1;
}

void TwiSimulationDataGenerator::CreateSignature()
{
    mTwiSimulationData.TransitionIfNeeded(BIT_LOW);
    mTwiSimulationData.Advance(mClockGenerator.AdvanceByTimeS(0.1));
    mTwiSimulationData.TransitionIfNeeded(BIT_HIGH);
    mTwiSimulationData.Advance(mClockGenerator.AdvanceByTimeS(0.005));
}

void TwiSimulationDataGenerator::CreateByte(U8 value)
{
    mTwiSimulationData.TransitionIfNeeded(BIT_LOW);
    mTwiSimulationData.Advance(mClockGenerator.AdvanceByHalfPeriod()); // start bit

    BitExtractor bitExtractor(value, AnalyzerEnums::MsbFirst, 8);

    for (U32 i = 0; i < 8; i++)
    {
        mTwiSimulationData.TransitionIfNeeded(bitExtractor.GetNextBit());
        mTwiSimulationData.Advance(mClockGenerator.AdvanceByHalfPeriod());
    }

    // parity
    if (AnalyzerHelpers::IsEven(AnalyzerHelpers::GetOnesCount(value)) == true)
        mTwiSimulationData.TransitionIfNeeded(BIT_LOW);
    else
        mTwiSimulationData.TransitionIfNeeded(BIT_HIGH);
    mTwiSimulationData.Advance(mClockGenerator.AdvanceByHalfPeriod());

    mTwiSimulationData.TransitionIfNeeded(BIT_LOW);

    mTwiSimulationData.Advance(mClockGenerator.AdvanceByTimeS(0.001));  // 1ms

    mTwiSimulationData.TransitionIfNeeded(BIT_HIGH); // idle

    mTwiSimulationData.Advance(mClockGenerator.AdvanceByHalfPeriod(2));
}
