#include "TwiAnalyzerSettings.h"

#include <AnalyzerHelpers.h>

TwiAnalyzerSettings::TwiAnalyzerSettings()
: mInputChannel(UNDEFINED_CHANNEL)
{
    mInputChannelInterface.reset(new AnalyzerSettingInterfaceChannel());
    mInputChannelInterface->SetTitleAndTooltip("K/F Line", "K/F");
    mInputChannelInterface->SetChannel(mInputChannel);

    AddInterface(mInputChannelInterface.get());

    AddExportOption(0, "Export as text/csv file");
    AddExportExtension(0, "text", "txt");
    AddExportExtension(0, "csv", "csv");

    ClearChannels();
    AddChannel(mInputChannel, "TWI", false);
}

TwiAnalyzerSettings::~TwiAnalyzerSettings()
{

}

bool TwiAnalyzerSettings::SetSettingsFromInterfaces()
{
    mInputChannel = mInputChannelInterface->GetChannel();

    ClearChannels();
    AddChannel(mInputChannel, "TWI", true);

    return true;
}

void TwiAnalyzerSettings::UpdateInterfacesFromSettings()
{
    mInputChannelInterface->SetChannel(mInputChannel);
}

void TwiAnalyzerSettings::LoadSettings(const char* settings)
{
    SimpleArchive textArchive;
    textArchive.SetString(settings);

    const char* nameString;
    textArchive >> &nameString;
    if (strcmp(nameString, "TwiAnalyzer") != 0)
        AnalyzerHelpers::Assert("TwiAnalyzer: Provided with a settings string that doesn't belong to us;");

    textArchive >> mInputChannel;

    ClearChannels();
    AddChannel(mInputChannel, "TWI", true);

    UpdateInterfacesFromSettings();
}

const char* TwiAnalyzerSettings::SaveSettings()
{
    SimpleArchive textArchive;

    textArchive << "TwiAnalyzer";
    textArchive << mInputChannel;

    return SetReturnString(textArchive.GetString());
}
