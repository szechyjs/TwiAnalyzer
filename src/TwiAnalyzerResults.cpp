#include "TwiAnalyzerResults.h"
#include <AnalyzerHelpers.h>

TwiAnalyzerResults::TwiAnalyzerResults(TwiAnalyzer* analyzer, TwiAnalyzerSettings* settings)
: AnalyzerResults(),
  mSettings(settings),
  mAnalyzer(analyzer)
  {

  }

TwiAnalyzerResults::~TwiAnalyzerResults()
{

}

void TwiAnalyzerResults::GenerateBubbleText(U64 frameIndex, Channel& channel, DisplayBase displayBase)
{
    ClearResultStrings();
    Frame frame = GetFrame(frameIndex);

    bool framingError = false;
    bool parityError = false;

    switch(frame.mType)
    {
      case SIGNATURE_TYPE:
          AddResultString("Signature");
          break;
      case DATA_TYPE:
          if (frame.HasFlag(FRAMING_ERROR_FLAG) != 0)
              framingError = true;

          if (frame.HasFlag(PARITY_ERROR_FLAG) != 0)
              parityError = true;

          char numberStr[128];
          AnalyzerHelpers::GetNumberString(frame.mData1, displayBase, 8, numberStr, 128);

          char resultStr[128];
          if (parityError || framingError)
          {
              AddResultString("!");
              snprintf(resultStr, sizeof(resultStr), "%s (error)", numberStr);
              AddResultString(resultStr);

              if (parityError && !framingError)
              {
                  snprintf(resultStr, sizeof(resultStr), "%s (parity error)", numberStr);
              }
              else if (!parityError && framingError)
              {
                  snprintf(resultStr, sizeof(resultStr), "%s (framing error)", numberStr);
              }
              else
              {
                  snprintf(resultStr, sizeof(resultStr), "%s (framing error & parity error)", numberStr);
              }
              AddResultString(resultStr);
          }
          else
          {
              AddResultString(numberStr);
          }
          break;
      default:
          AddResultString("Unknown");
          break;
    }
}

void TwiAnalyzerResults::GenerateExportFile(const char* file, DisplayBase displayBase, U32 exportTypeUserId)
{

}

void TwiAnalyzerResults::GenerateFrameTabularText(U64 frameIndex, DisplayBase displayBase)
{
    ClearTabularText();
    Frame frame = GetFrame(frameIndex);

    bool framingError = false;
    bool parityError = false;

    switch(frame.mType)
    {
      case SIGNATURE_TYPE:
          AddTabularText("Signature");
          break;
      case DATA_TYPE:
          if (frame.HasFlag(FRAMING_ERROR_FLAG))
              framingError = true;

          if (frame.HasFlag(PARITY_ERROR_FLAG))
              parityError = true;

          char numberStr[128];
          AnalyzerHelpers::GetNumberString(frame.mData1, displayBase, 8, numberStr, 128);

          char resultStr[128];

          if (parityError || framingError)
          {
              if (parityError && !framingError)
              {
                  snprintf(resultStr, sizeof(resultStr), "%s (parity error)", numberStr);
              }
              else if (!parityError && framingError)
              {
                  snprintf(resultStr, sizeof(resultStr), "%s (framing error)", numberStr);
              }
              else
              {
                  snprintf(resultStr, sizeof(resultStr), "%s (framing error & parity error)", numberStr);
              }
              AddTabularText(resultStr);
          }
          else
          {
              AddTabularText(numberStr);
          }
          break;
      default:
          AddTabularText("Unkown");
          break;
    }
}

void TwiAnalyzerResults::GeneratePacketTabularText(U64 packetId, DisplayBase displayBase)
{
    ClearResultStrings();
    AddResultString("not supported");
}

void TwiAnalyzerResults::GenerateTransactionTabularText(U64 transactionId, DisplayBase displayBase)
{
    ClearResultStrings();
    AddResultString("not supported");
}
