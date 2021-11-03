#pragma once

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

class TwiAnalyzerSettings : public AnalyzerSettings
{
  public:
      TwiAnalyzerSettings();
      virtual ~TwiAnalyzerSettings();

      virtual bool SetSettingsFromInterfaces();
      void UpdateInterfacesFromSettings();
      virtual void LoadSettings(const char*);
      virtual const char* SaveSettings();

      Channel mInputChannel;

  protected:
      std::unique_ptr<AnalyzerSettingInterfaceChannel> mInputChannelInterface;
};
