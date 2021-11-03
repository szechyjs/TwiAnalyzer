#pragma once

#include <AnalyzerResults.h>

#define FRAMING_ERROR_FLAG ( 1 << 0 )
#define PARITY_ERROR_FLAG ( 1 << 1 )

#define SIGNATURE_TYPE 1
#define DATA_TYPE 2

class TwiAnalyzer;
class TwiAnalyzerSettings;

class TwiAnalyzerResults : public AnalyzerResults
{
  public:
      TwiAnalyzerResults(TwiAnalyzer*, TwiAnalyzerSettings*);
      virtual ~TwiAnalyzerResults();

      virtual void GenerateBubbleText(U64, Channel&, DisplayBase);
      virtual void GenerateExportFile(const char*, DisplayBase, U32);

      virtual void GenerateFrameTabularText(U64, DisplayBase);
      virtual void GeneratePacketTabularText(U64, DisplayBase);
      virtual void GenerateTransactionTabularText(U64, DisplayBase);

  protected:
      TwiAnalyzerSettings* mSettings;
      TwiAnalyzer* mAnalyzer;
};
