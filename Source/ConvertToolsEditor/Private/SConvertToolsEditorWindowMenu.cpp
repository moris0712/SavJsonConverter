// Copyright (c) 2025 Moris0712. - All Rights Reserved

#include "SConvertToolsEditorWindowMenu.h"

#include "ConvertToolsEditorStyle.h"
#include "ConvertToolsLog.h"
#include "DesktopPlatformModule.h"
#include "ConvertToolsPropertyRuleBPLibrary.h"
#include "SlateOptMacros.h"
#include "Framework/Text/SlateTextRun.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Text/SMultiLineEditableText.h"

#include "GameFramework/SaveGame.h"
#include "Test/TestSaveData.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"


#define PROCESS_SUCCESS(TextBlock, Format, ...) Notify(TextBlock, LogInternal::EConverterLog::Success, Format, ##__VA_ARGS__);
#define PROCESS_FAIL(TextBlock, Format, ...) Notify(TextBlock, LogInternal::EConverterLog::Error, Format, ##__VA_ARGS__);

#define APPEND_NORMAL(Format, ...) AppendLog(LogInternal::EConverterLog::Normal, Format, ##__VA_ARGS__)
#define APPEND_SUCCESS(Format, ...) AppendLog(LogInternal::EConverterLog::Success, Format, ##__VA_ARGS__)
#define APPEND_WARN(Format, ...) AppendLog(LogInternal::EConverterLog::Warning, Format, ##__VA_ARGS__)
#define APPEND_ERROR(Format, ...) AppendLog(LogInternal::EConverterLog::Error, Format, ##__VA_ARGS__)
#define APPEND_HIGHLIGHT(Format, ...) AppendLog(LogInternal::EConverterLog::Highlight, Format, ##__VA_ARGS__)

constexpr int32 SlotPadding = 10;
constexpr int32 HorizontalSlotPadding = 20;
constexpr int32 VerticalSlotPadding = 20;

constexpr float LeftBoxStretch = 0.4f;
constexpr float RightBoxStretch = 0.6f;

const FString SaveGameClassField = TEXT("SaveGameClass");
const int32 IndentLevelMultiply = 4;

const FString LoadSaveFileName = TEXT("Load Save File");
const FString SaveToJsonFileName = TEXT("Save To Json File");

const FString LoadJsonFileName = TEXT("Load Json File");
const FString SaveToSavFileName = TEXT("Save To Sav File");

const FString DefaultWarnLog = TEXT("If you've modified the `.sav or .json` file externally (e.g., via another tool), make sure to **reload** it before attempting conversion.");

// .AutoHeight - Align Size to Content
// Two Horizontal (Left / Right) + (AddSlots - VerticalBoxes)

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SConvertToolsEditorWindowMenu::Construct(const FArguments& InArgs)
{
    const TSharedRef<SWidget> SavToJsonPanel = SavToJsonWidget();
    const TSharedRef<SWidget> JsonToSavPanel = JsonToSavWidget();
    const TSharedRef<SWidget> LogPanel = ConverterLogWidget();

    ChildSlot
    .Padding(FMargin(SlotPadding))
	[
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .FillWidth(LeftBoxStretch)
        .Padding(SlotPadding)
        [
            SNew(SBox)
            [
		        SNew(SVerticalBox)
		        + SVerticalBox::Slot()
		        [
                    SavToJsonPanel
		        ]
		        + SVerticalBox::Slot()
		        [
                    JsonToSavPanel
		        ]
	            // TEST Data Widget
	            + SVerticalBox::Slot()
	            [
	                SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        SNew(SVerticalBox)
                        +SVerticalBox::Slot()
                        .AutoHeight()
                        [
	                        SNew(SButton)
                            .Text(FText::FromString("Create Test Sav Data"))
                            .OnClicked(FOnClicked::CreateSP(this, &SConvertToolsEditorWindowMenu::CreateTestSav))
						]
                    ]
	            ]
            ]
        ]
        + SHorizontalBox::Slot()
        .FillWidth(RightBoxStretch)
        .Padding(HorizontalSlotPadding, 0)
        [
            LogPanel
        ]
    ];
    ChangeSavButton->SetEnabled(false);
    ChangeJsonButton->SetEnabled(false);

    ClearLog();
}

// TEST
FReply SConvertToolsEditorWindowMenu::CreateTestSav()
{
    UTestSerializableSaveData* CreatedMoonTestSaveGameData = Cast<UTestSerializableSaveData>(UGameplayStatics::CreateSaveGameObject(UTestSerializableSaveData::StaticClass()));
    CreatedMoonTestSaveGameData->FillTestData();

    TArray<uint8> ObjectBytes;
    if (UGameplayStatics::SaveGameToMemory(CreatedMoonTestSaveGameData, ObjectBytes))
    {
        const FString DefaultPath = FPaths::ProjectSavedDir() + "SaveGames/";
        const FString FileTypes = TEXT("Save Files (*.sav)|*.sav");
        const FString DefaultFileName = TEXT("TestSerializableSaveData");
        const uint32 FileFlag = EFileDialogFlags::None;

        IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
        if (DesktopPlatform)
        {
            TArray<FString> OutFileNames;

            TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow(AsShared());
            void* ParentWindowHandle = ParentWindow.IsValid()
                ? ParentWindow->GetNativeWindow()->GetOSWindowHandle()
                : nullptr;

            const bool bOpenFile = DesktopPlatform->SaveFileDialog(
                ParentWindowHandle,
                TEXT("Save Sav File"),
                DefaultPath,
                DefaultFileName + TEXT(".sav"),
                FileTypes,
                FileFlag,
                OutFileNames
            );

            if (bOpenFile && !OutFileNames.IsEmpty())
            {
                JsonToSavFile(OutFileNames[0], CreatedMoonTestSaveGameData);
            }
        }
    }
    return FReply::Handled();
}
// TEST

// ----------------------- Sav to JSON --------------------------------
TSharedRef<SWidget> SConvertToolsEditorWindowMenu::SavToJsonWidget()
{
    return SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        .VAlign(VAlign_Center)
        .Padding(SlotPadding, SlotPadding)
        [
            SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()

                [
                    SNew(STextBlock)
                        .Text(FText::FromString("Selected Save File"))
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(SUniformGridPanel)
                        .SlotPadding(FMargin{ SlotPadding * 5, 0.0f })
                        .MinDesiredSlotWidth(FAppStyle::GetFloat("StandardDialog.MinDesiredSlotWidth"))
                        .MinDesiredSlotHeight(FAppStyle::GetFloat("StandardDialog.MinDesiredSlotHeight"))
                        + SUniformGridPanel::Slot(0, 0)
                        [
                            SAssignNew(SaveFileTextBlock, STextBlock)
                                .Text(FText::FromString(TEXT("...")))
                                .AutoWrapText(true)
                        ]
                ]
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign(HAlign_Left)
        .VAlign(VAlign_Center)
        .Padding(SlotPadding, SlotPadding)
        [

            SNew(SSplitter)
                .Orientation(Orient_Horizontal)
                + SSplitter::Slot()
                [
                    SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(SButton)
                                .Text(FText::FromString(LoadSaveFileName))
                                .OnClicked(FOnClicked::CreateSP(this, &SConvertToolsEditorWindowMenu::OpenLoadSavFileBrowser))
                        ]
                ]
                + SSplitter::Slot()
                [
                    SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SAssignNew(ChangeJsonButton, SButton)
                                .Text(FText::FromString(SaveToJsonFileName))
                                .OnClicked(FOnClicked::CreateSP(this, &SConvertToolsEditorWindowMenu::OpenSaveJsonFileBrowser))
                        ]
                ]
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SAssignNew(SavNoticeTextBlock, STextBlock)
                        .Text(FText::FromString(TEXT("")))
                ]
        ];
}

FReply SConvertToolsEditorWindowMenu::OpenLoadSavFileBrowser()
{
    const FString DefaultPath = FPaths::ProjectSavedDir() + "SaveGames/";
    const FString FileTypes = TEXT("Save Files (*.sav)|*.sav");
    const uint32 FileFlag = EFileDialogFlags::None;

    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
    if (DesktopPlatform)
    {
        TArray<FString> OutFileNames;

        TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow(AsShared());
        void* ParentWindowHandle = ParentWindow.IsValid()
            ? ParentWindow->GetNativeWindow()->GetOSWindowHandle()
            : nullptr;

        const bool bOpenFile = DesktopPlatform->OpenFileDialog(
            ParentWindowHandle,
            TEXT("Select Sav File"),
            DefaultPath,
            TEXT(""),
            FileTypes,
            FileFlag,
            OutFileNames
        );

        if (bOpenFile && !OutFileNames.IsEmpty())
        {
            LoadSaveFile(OutFileNames[0]);
        }
    }

    return FReply::Handled();
}

void SConvertToolsEditorWindowMenu::LoadSaveFile(const FString& FilePath)
{
    TArray<uint8> BinaryData;
    if (!FFileHelper::LoadFileToArray(BinaryData, *FilePath))
    {
        PROCESS_FAIL(SavNoticeTextBlock, TEXT("Load Sav Failed"));
        ChangeJsonButton->SetEnabled(false);
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("File Size: %d"), BinaryData.Num())
    if (BinaryData.Num() == 0)
    {
        // Empty buffer, return instead of causing a bad serialize that could crash
        PROCESS_FAIL(SavNoticeTextBlock, TEXT("Empty buffer"));
        ChangeJsonButton->SetEnabled(false);
        return;
    }

    // Change Binary To SaveGame Instance
    SelectedSaveGame = UGameplayStatics::LoadGameFromMemory(BinaryData);
    if (SelectedSaveGame == nullptr)
    {
        PROCESS_FAIL(SavNoticeTextBlock, TEXT("Not UE SavFile Format Or Can't Find SaveGameClass In This Project"));
    	ChangeJsonButton->SetEnabled(false);
        return;
    }

    const FString& SelectedFileName = FilePath;
    const FString& AbsoluteFilePath = FPaths::ConvertRelativePathToFull(SelectedFileName);
    SaveFileTextBlock->SetText(FText::FromString(AbsoluteFilePath));
    ChangeJsonButton->SetEnabled(true);

    PROCESS_SUCCESS(SavNoticeTextBlock,TEXT("Load Sav File Successfully"));

    const TSharedPtr<FJsonObject>& JsonObject = ConvertSavToJson(SelectedSaveGame);

    APPEND_HIGHLIGHT(TEXT("This sav file will be converted with the following data"));
    CompareAndLogJsonValues(JsonObject, JsonObject);
    APPEND_SUCCESS(TEXT("If you want to save as JSON, please click the '%s' button."), *SaveToJsonFileName);
}

FReply SConvertToolsEditorWindowMenu::OpenSaveJsonFileBrowser()
{
    const FString DefaultPath = FPaths::ProjectSavedDir() + "SaveGames/";
    const FString FileTypes = TEXT("Json Files (*.json)|*.json");
    const FString DefaultFileName = FPaths::GetBaseFilename(SaveFileTextBlock->GetText().ToString());
    const uint32 FileFlag = EFileDialogFlags::None;

    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

    if (DesktopPlatform)
    {
        TArray<FString> OutFileNames;

        TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow(AsShared());
        void* ParentWindowHandle = ParentWindow.IsValid()
            ? ParentWindow->GetNativeWindow()->GetOSWindowHandle()
            : nullptr;

        const bool bFileChosen = DesktopPlatform->SaveFileDialog(
            ParentWindowHandle,
            TEXT("Save To Json File"),
            DefaultPath,
            DefaultFileName + TEXT(".json"),
            FileTypes,
            FileFlag,
            OutFileNames
        );

        if (bFileChosen && !OutFileNames.IsEmpty())
        {
            SaveToJsonFile(OutFileNames[0]);
        }
    }
    return FReply::Handled();
}

void SConvertToolsEditorWindowMenu::SaveToJsonFile(const FString& FilePath)
{
    const TSharedPtr<FJsonObject>& JsonObject = ConvertSavToJson(SelectedSaveGame);

    FString OutputJson;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputJson);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    if (FFileHelper::SaveStringToFile(OutputJson, *FilePath))
    {
	    PROCESS_SUCCESS(SavNoticeTextBlock, TEXT("Change To JSON Successfully"));
    }
    else
    {
	    PROCESS_FAIL(SavNoticeTextBlock, TEXT("Change To JSON Failed"));
    }
}

TSharedPtr<FJsonObject> SConvertToolsEditorWindowMenu::ConvertSavToJson(USaveGame* SaveGame)
{
    const TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
    JsonObject->SetStringField(SaveGameClassField, SaveGame->GetClass()->GetName());

    const UClass* SaveGameClass = SaveGame->GetClass();
    for (TFieldIterator<FProperty> It(SaveGameClass); It; ++It)
    {
        FProperty* Property = *It;
        void* ValuePtr = Property->ContainerPtrToValuePtr<void>(SaveGame);

        TSharedPtr<FJsonValue> JsonValue = UConvertToolsPropertyRuleBPLibrary::GetJsonValue(Property, ValuePtr);
        JsonObject->SetField(Property->GetName(), JsonValue);
    }
    return JsonObject;
}
// ----------------------- Sav to JSON --------------------------------

// ----------------------- Json to Sav --------------------------------
TSharedRef<SWidget> SConvertToolsEditorWindowMenu::JsonToSavWidget()
{
    return SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        .VAlign(VAlign_Center)
        .Padding(SlotPadding, SlotPadding)
        [
            SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(STextBlock)
                        .Text(FText::FromString("Selected JSON File"))
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(SUniformGridPanel)
                        .SlotPadding(FMargin{ SlotPadding * 5, 0.0f })
                        .MinDesiredSlotWidth(FAppStyle::GetFloat("StandardDialog.MinDesiredSlotWidth"))
                        .MinDesiredSlotHeight(FAppStyle::GetFloat("StandardDialog.MinDesiredSlotHeight"))
                        + SUniformGridPanel::Slot(0, 0)
                        [
                            SAssignNew(JsonFileTextBlock, STextBlock)
                                .Text(FText::FromString(TEXT("...")))
                                .AutoWrapText(true)
                        ]
                ]
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign(HAlign_Left)
        .VAlign(VAlign_Center)
        .Padding(SlotPadding, SlotPadding)
        [
            SNew(SSplitter)
                .Orientation(Orient_Horizontal)

                + SSplitter::Slot()
                [
                    SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(SButton)
                                .Text(FText::FromString(LoadJsonFileName))
                                .OnClicked(FOnClicked::CreateSP(this, &SConvertToolsEditorWindowMenu::OpenLoadJsonFileBrowser))
                        ]
                ]
                + SSplitter::Slot()
                [
                    SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SAssignNew(ChangeSavButton, SButton)
                                .Text(FText::FromString(SaveToSavFileName))
                                .OnClicked(FOnClicked::CreateSP(this, &SConvertToolsEditorWindowMenu::OpenSaveSavFileBrowser))
                        ]
                ]
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SAssignNew(JsonNoticeTextBlock, STextBlock)
                        .Text(FText::FromString(TEXT("")))
                ]
        ];
}

FReply SConvertToolsEditorWindowMenu::OpenLoadJsonFileBrowser()
{
    const FString DefaultPath = FPaths::ProjectSavedDir() + "SaveGames/";
    const FString FileTypes = TEXT("Json Files (*.json)|*.json");
    const uint32 FileFlag = EFileDialogFlags::None;

    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

    if (DesktopPlatform)
    {
        TArray<FString> OutFileNames;

        TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow(AsShared());
        void* ParentWindowHandle = ParentWindow.IsValid()
            ? ParentWindow->GetNativeWindow()->GetOSWindowHandle()
            : nullptr;

        const bool bOpenFile = DesktopPlatform->OpenFileDialog(
            ParentWindowHandle,
            TEXT("Load Json File"),
            DefaultPath,
            TEXT(""),
            FileTypes,
            FileFlag,
            OutFileNames
        );

        if (bOpenFile && !OutFileNames.IsEmpty())
        {
            LoadJsonFile(OutFileNames[0]);
        }
    }
    return FReply::Handled();
}

void SConvertToolsEditorWindowMenu::LoadJsonFile(const FString& FilePath)
{
    FString JsonString;
    if (!FFileHelper::LoadFileToString(JsonString, *FilePath))
    {
        PROCESS_FAIL(JsonNoticeTextBlock, TEXT("Load JSON File Failed"));
        ChangeSavButton->SetEnabled(false);
        return;
    }

    TSharedPtr<FJsonObject> SelectedJsonObject = MakeShared<FJsonObject>();
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
    if (!FJsonSerializer::Deserialize(Reader, SelectedJsonObject) || !SelectedJsonObject.IsValid())
    {
        PROCESS_FAIL(JsonNoticeTextBlock, TEXT("JSON Parsing Failed"));
        SelectedJsonObject = nullptr;
        ChangeSavButton->SetEnabled(false);
        return;
    }

    const FString& SelectedFileName = FilePath;
    const FString& AbsoluteFilePath = FPaths::ConvertRelativePathToFull(SelectedFileName);
    JsonFileTextBlock->SetText(FText::FromString(AbsoluteFilePath));
    ChangeSavButton->SetEnabled(true);

    // Preview Changed SaveGame to Json
    ChangedSaveGame = ConvertJsonToSav(SelectedJsonObject);
    if (ChangedSaveGame == nullptr)
    {
        return;
    }

    PROCESS_SUCCESS(JsonNoticeTextBlock, TEXT("JSON Load SuccessFully"));

	const TSharedPtr<FJsonObject>& ChangedJsonContent = ConvertSavToJson(ChangedSaveGame);
    APPEND_HIGHLIGHT(TEXT("This sav file will be saved with the following data"));
    CompareAndLogJsonValues(SelectedJsonObject, ChangedJsonContent);
    APPEND_SUCCESS(TEXT("If you want to save as Sav, please click the '%s' button."), *SaveToSavFileName);
}

void SConvertToolsEditorWindowMenu::CompareAndLogJsonValues(const TSharedPtr<FJsonObject>& OriginJsonObject, const TSharedPtr<FJsonObject>& ChangedJsonObject)
{
    APPEND_HIGHLIGHT(TEXT("--------------------------------------------------------------------"));
    APPEND_NORMAL(TEXT("{"));
    if (!OriginJsonObject.IsValid() || !ChangedJsonObject.IsValid())
        return;

    ChangedJsonObject->Values.Remove(SaveGameClassField);
    int32 Count = 0;
    for (const TTuple<FString, TSharedPtr<FJsonValue>>& Elem : ChangedJsonObject->Values)
    {
        const TSharedPtr<FJsonValue>* OriginValue = OriginJsonObject->Values.Find(Elem.Key);
        const TSharedPtr<FJsonValue>* ChangedValue = ChangedJsonObject->Values.Find(Elem.Key);
        const bool bIsLast = (++Count == ChangedJsonObject->Values.Num());

        CompareJsonRecursive(Elem.Key, OriginValue != nullptr ? *OriginValue : nullptr, ChangedValue != nullptr ? *ChangedValue : nullptr, !bIsLast);
    }
    APPEND_NORMAL(TEXT("}"));
    APPEND_HIGHLIGHT(TEXT("--------------------------------------------------------------------"));
}

FReply SConvertToolsEditorWindowMenu::OpenSaveSavFileBrowser()
{
    if (ChangedSaveGame == nullptr)
    {
        return FReply::Handled();
    }

    const FString DefaultPath = FPaths::ProjectSavedDir() + "SaveGames/";
    const FString FileTypes = TEXT("Save Files (*.sav)|*.sav");
    const FString DefaultFileName = FPaths::GetBaseFilename(JsonFileTextBlock->GetText().ToString());
    const uint32 FileFlag = EFileDialogFlags::None;

    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
    if (DesktopPlatform)
    {
        TArray<FString> OutFileNames;

        TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow(AsShared());
        void* ParentWindowHandle = ParentWindow.IsValid()
            ? ParentWindow->GetNativeWindow()->GetOSWindowHandle()
            : nullptr;

        const bool bOpenFile = DesktopPlatform->SaveFileDialog(
            ParentWindowHandle,
            TEXT("Save Sav File"),
            DefaultPath,
            DefaultFileName + TEXT(".sav"),
            FileTypes,
            FileFlag,
            OutFileNames
        );

        if (bOpenFile && !OutFileNames.IsEmpty())
        {
            JsonToSavFile(OutFileNames[0], ChangedSaveGame);
        }
    }
    return FReply::Handled();
}

void SConvertToolsEditorWindowMenu::JsonToSavFile(const FString& FilePath, USaveGame* SaveGame)
{
    TArray<uint8> SaveData;
    if (!UGameplayStatics::SaveGameToMemory(SaveGame, SaveData))
    {
		PROCESS_FAIL(JsonNoticeTextBlock, TEXT("Change To Sav Failed"));
        return;
    }

	if (!FFileHelper::SaveArrayToFile(SaveData, *FilePath))
	{
        PROCESS_FAIL(JsonNoticeTextBlock, TEXT("Save Sav Failed"));
        return;
	}
	PROCESS_SUCCESS(JsonNoticeTextBlock, TEXT("Save Sav SuccessFully"));
}

USaveGame* SConvertToolsEditorWindowMenu::ConvertJsonToSav(const TSharedPtr<FJsonObject>& JsonObject)
{
    // Get SaveGameClass Name
    FString SaveGameClassName;
    if (!JsonObject->TryGetStringField(SaveGameClassField, SaveGameClassName))
    {
        PROCESS_FAIL(JsonNoticeTextBlock, TEXT("Not Exist SaveGameClass in JSON"));
        return nullptr;
    }
    // Validate SaveGameClass 
    const UClass* SaveGameClass = FindObject<UClass>(ANY_PACKAGE, *SaveGameClassName);
    const FString SaveGameUClassName = SaveGameClass != nullptr ? SaveGameClass->GetName() : FString(TEXT(""));;
    if (!SaveGameClass || !SaveGameClass->IsChildOf(USaveGame::StaticClass()))
    {
        PROCESS_FAIL(JsonNoticeTextBlock, TEXT("Not Valid SaveGameClass: %s"), 
            SaveGameUClassName.IsEmpty() ? *SaveGameClassName : *SaveGameUClassName);
    	return nullptr;
    }

    // Create SaveGame Instance
    USaveGame* SaveGame = NewObject<USaveGame>(GetTransientPackage(), SaveGameClass);
	if (!SaveGame)
    {
        PROCESS_FAIL(JsonNoticeTextBlock, TEXT("Created SaveGame Instance Failed"));
        return nullptr;
    }

    JsonObject->Values.Remove(SaveGameClassField);
    for (const TTuple<FString, TSharedPtr<FJsonValue>>& Entry : JsonObject->Values)
    {
        if (UConvertToolsPropertyRuleBPLibrary::IsJsonValueEmpty(Entry.Value))
        {
            APPEND_WARN(TEXT("%s - Json Value Is Empty"), *Entry.Key);
        }

        FProperty* Property = SaveGame->GetClass()->FindPropertyByName(FName(*Entry.Key));
        if (!Property)
        {
            APPEND_WARN(TEXT("%s - Not Exist Property In SaveGameClass"), *Entry.Key);
            continue;
        }

        void* ValuePtr = Property->ContainerPtrToValuePtr<void>(SaveGame);
        if (!ValuePtr)
        {
            APPEND_WARN(TEXT("%s - Can't Get Value"), *Entry.Key);
            continue;
        }

        FText FailReason;
        UConvertToolsPropertyRuleBPLibrary::AddSaveField(Entry.Value, Property, ValuePtr, &FailReason);

        if (!FailReason.IsEmpty()) APPEND_WARN(TEXT("%s %s"), *Entry.Key, *FailReason.ToString());
    }
    APPEND_SUCCESS(TEXT("%s Create Success"), *SaveGameUClassName);
    return SaveGame;
}
// ----------------------- Json to Sav --------------------------------


void SConvertToolsEditorWindowMenu::Notify(const TSharedPtr<STextBlock>& TextBlock, const LogInternal::EConverterLog ConverterLogType, const TCHAR* Format, ...)
{
    TCHAR Buffer[1024];

    va_list Args;
    va_start(Args, Format);
    FCString::GetVarArgs(Buffer, UE_ARRAY_COUNT(Buffer), Format, Args);
    const FString Message{ Buffer };
    va_end(Args);

    TextBlock->SetText(FText::FromString(Message));
    const FSlateColor TextColor = GetColor(ConverterLogType);
    TextBlock->SetColorAndOpacity(TextColor);

    AppendLog(ConverterLogType, *Message);
    ConverterLog(ConverterLogType, Message);
}


// Converter Log
TSharedRef<SWidget> SConvertToolsEditorWindowMenu::ConverterLogWidget()
{
    return
        SNew(SBox)
        .MinDesiredWidth(600.0f)
        .MaxDesiredWidth(1000.0f)
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0.0f, VerticalSlotPadding)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(STextBlock)
                    .Text(FText::FromString("Log"))
                    .TextStyle(FAppStyle::Get(), "LargeText")
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(FMargin(HorizontalSlotPadding, 0.0f))
                [
                    SNew(SButton)
                    .Text(FText::FromString("Clear Log"))
                    .OnClicked(this, &SConvertToolsEditorWindowMenu::ClearLog)
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                .Padding(FMargin(HorizontalSlotPadding, 0.0f))
                [
                    SNew(SBox)
                    [
	                    SNew(SHorizontalBox)
	                    + SHorizontalBox::Slot()
	                    .AutoWidth()
	                    [
	                        SNew(STextBlock)
	                        .Text(FText::FromString("ScrollToEnd"))
	                    ]
	                    + SHorizontalBox::Slot()
	                    .Padding(FMargin(SlotPadding, 0.0f))
	                    [
	                        SNew(SCheckBox)
	                        .OnCheckStateChanged(this, &SConvertToolsEditorWindowMenu::OnCheckBoxStateChanged)
	                        .IsChecked(this, &SConvertToolsEditorWindowMenu::IsCheckBoxChecked)
						]
                    ]
                ]
            ]
            + SVerticalBox::Slot()
            [
                SAssignNew(LogScrollBox, SScrollBox)
                + SScrollBox::Slot()
                [
                    SNew(SBorder)
                    .BorderImage(FAppStyle::Get().GetBrush("BlackBrush"))
                    [
                        SAssignNew(LogTextBox, SMultiLineEditableTextBox)
                            .Padding(10.0f)
                            .Text(FText::GetEmpty())
                            .ForegroundColor(FLinearColor::Black)
                            .BackgroundColor(FLinearColor(0.25f, 0.25f, 0.25f))
                            .IsReadOnly(true)
                            .CreateSlateTextLayout(this, &SConvertToolsEditorWindowMenu::OnCreateTextLayout)
					]
				]
            ]
        ];
}

void SConvertToolsEditorWindowMenu::AppendLog(const LogInternal::EConverterLog ConverterLogType, const TCHAR* Format, ...)
{
    TCHAR Buffer[1024];

    va_list Args;
    va_start(Args, Format);
    FCString::GetVarArgs(Buffer, UE_ARRAY_COUNT(Buffer), Format, Args);
    const FString Message{ Buffer };
    va_end(Args);

    const FSlateColor TextColor = GetColor(ConverterLogType);

    FTextBlockStyle TextStyle = FConvertToolsEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("ConvertToolsEditor.Log");
    TextStyle.SetColorAndOpacity(TextColor);

    TSharedRef<FString> LineText = MakeShared<FString>(Message);
    const TSharedRef<FSlateTextRun> TextRun = FSlateTextRun::Create(
        FRunInfo { TEXT("ColoredRun") } ,
        LineText,
        TextStyle,
        FTextRange{ 0, LineText->Len() }
    );

    TArray<TSharedRef<IRun>> Runs;
    Runs.Add(TextRun);
    const FTextLayout::FNewLineData NewLineData{ MoveTemp(LineText), MoveTemp(Runs) };
    LogTextLayout->AddLine(NewLineData);
	if (bScrollEnded) LogScrollBox->ScrollToEnd();

    ConverterLog(ConverterLogType, Message);
}

TSharedRef<FSlateTextLayout> SConvertToolsEditorWindowMenu::OnCreateTextLayout(SWidget* Widget, const FTextBlockStyle& TextStyle)
{
    TSharedRef<FSlateTextLayout> CreatedLogTextLayout = FSlateTextLayout::Create(Widget, TextStyle);
	LogTextLayout = CreatedLogTextLayout.ToSharedPtr();
	return CreatedLogTextLayout;
}

FReply SConvertToolsEditorWindowMenu::ClearLog()
{
    LogTextLayout->ClearLines();
    AppendLog(LogInternal::EConverterLog::Warning, *DefaultWarnLog);
    return FReply::Handled();
}

void SConvertToolsEditorWindowMenu::OnCheckBoxStateChanged(ECheckBoxState NewState)
{
    bScrollEnded = NewState == ECheckBoxState::Checked ? true : false;
    if (bScrollEnded) LogScrollBox->ScrollToEnd();
}

ECheckBoxState SConvertToolsEditorWindowMenu::IsCheckBoxChecked() const
{
    return bScrollEnded ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void SConvertToolsEditorWindowMenu::CompareJsonRecursive(const FString& KeyPath, const TSharedPtr<FJsonValue>& OriginValue, const TSharedPtr<FJsonValue>& ChangedValue, const bool bAppendComma)
{
    const FString OriginValueString = UConvertToolsPropertyRuleBPLibrary::JsonValueToString(OriginValue);
    const FString ChangedValueString = UConvertToolsPropertyRuleBPLibrary::JsonValueToString(ChangedValue);

    if (!OriginValue.IsValid() || !ChangedValue.IsValid())
    {
        const bool bEqual = OriginValue.IsValid() == ChangedValue.IsValid();
        AppendFormattedLog(KeyPath, OriginValueString, ChangedValueString,
            bEqual ? LogInternal::EConverterLog::Normal : LogInternal::EConverterLog::Warning,
            bAppendComma);
        return;
    }

    if (OriginValue->Type == EJson::Object)
    {
        const int32 IndentLevel = GetIndentDepthFromKey(KeyPath);
        const FString Indent = FString::ChrN(IndentLevel * IndentLevelMultiply, ' ');

        const FString PrettyKey = GetPrettyKey(KeyPath);
        if (!PrettyKey.IsEmpty())
            APPEND_NORMAL(*FString::Printf(TEXT("%s\"%s\": {"), *Indent, *PrettyKey));
        else
            APPEND_NORMAL(*FString::Printf(TEXT("%s{"), *Indent));

        const TSharedPtr<FJsonObject> OriginObject = OriginValue->AsObject();
        const TSharedPtr<FJsonObject> ChangedObject = ChangedValue->AsObject();

        int32 Count = 0;
        for (const TTuple<FString, TSharedPtr<FJsonValue>>& Elem : OriginObject->Values)
        {
            const FString& SubKey = Elem.Key;
            const TSharedPtr<FJsonValue>* SubValA = OriginObject->Values.Find(SubKey);
            const TSharedPtr<FJsonValue>* SubValB = ChangedObject->Values.Find(SubKey);
            const bool bIsLast = (++Count == OriginObject->Values.Num());

            CompareJsonRecursive(
                KeyPath.IsEmpty() ? SubKey : KeyPath + TEXT(".") + SubKey,
                SubValA ? *SubValA : nullptr,
                SubValB ? *SubValB : nullptr,
                !bIsLast
            );
        }

        const FString Closing = FString::Printf(TEXT("%s}%s"), *Indent, bAppendComma ? TEXT(",") : TEXT(""));
        APPEND_NORMAL(*Closing);
    }
    else if (OriginValue->Type == EJson::Array)
    {
        const int32 IndentLevel = GetIndentDepthFromKey(KeyPath);
        const FString Indent = FString::ChrN(IndentLevel * IndentLevelMultiply, ' ');
        const FString PrettyKey = GetPrettyKey(KeyPath);

        APPEND_NORMAL(*FString::Printf(TEXT("%s\"%s\": ["), *Indent, *PrettyKey));

        const TArray<TSharedPtr<FJsonValue>>& ArrA = OriginValue->AsArray();
        const TArray<TSharedPtr<FJsonValue>>& ArrB = ChangedValue->AsArray();

        const int32 MaxLen = FMath::Max(ArrA.Num(), ArrB.Num());
        for (int32 i = 0; i < MaxLen; ++i)
        {
            const TSharedPtr<FJsonValue> ElemA = i < ArrA.Num() ? ArrA[i] : nullptr;
            const TSharedPtr<FJsonValue> ElemB = i < ArrB.Num() ? ArrB[i] : nullptr;
            const bool bIsLast = (i == MaxLen - 1);

            const FString SubKeyPath = FString::Printf(TEXT("%s[%d]"), *KeyPath, i);
            if (ElemA.IsValid() && ElemA->Type == EJson::Object)
            {
                CompareJsonRecursive(SubKeyPath, ElemA, ElemB, !bIsLast);
            }
            else
            {
                const FString ElementAValueString = UConvertToolsPropertyRuleBPLibrary::JsonValueToString(ElemA);
                const FString ElementBValueString = UConvertToolsPropertyRuleBPLibrary::JsonValueToString(ElemB);

                const bool bEqual = ElementAValueString.Equals(ElementBValueString);
                const FString ValueStr = DiffValueString(ElementAValueString, ElementBValueString);
 
                const FString ElemLine = FString::Printf(TEXT("%s  %s%s"), *Indent, *ValueStr, bIsLast ? TEXT("") : TEXT(","));
                AppendLog(bEqual ? LogInternal::EConverterLog::Normal : LogInternal::EConverterLog::Warning, *ElemLine);
            }
        }

        const FString Closing = FString::Printf(TEXT("%s]%s"), *Indent, bAppendComma ? TEXT(",") : TEXT(""));
        APPEND_NORMAL(*Closing);
    }
    else
    {
        const bool bEqual = OriginValueString.Equals(ChangedValueString);
        AppendFormattedLog(KeyPath, OriginValueString, ChangedValueString,
            bEqual ? LogInternal::EConverterLog::Normal : LogInternal::EConverterLog::Warning,
            bAppendComma);
    }
}

FString SConvertToolsEditorWindowMenu::DiffValueString(const FString& OriginValueString, const FString& ChangedValueString)
{
    FString ValueStr;
    if (!OriginValueString.IsEmpty() && ChangedValueString.IsEmpty())
    {
        ValueStr = FString::Printf(TEXT("(null) → %s"), *ChangedValueString);
    }
    else if (OriginValueString.IsEmpty() && !ChangedValueString.IsEmpty())
    {
        ValueStr = FString::Printf(TEXT("%s → (null)"), *OriginValueString);
    }
    else if (!OriginValueString.Equals(ChangedValueString))
    {
        ValueStr = FString::Printf(TEXT("%s → %s"), *OriginValueString, *ChangedValueString);
    }
    else
    {
        ValueStr = ChangedValueString;
    }
    return ValueStr;
}

void SConvertToolsEditorWindowMenu::AppendFormattedLog(const FString& KeyPath, const FString& OriginValueString, const FString& ChangedValueString, const LogInternal::EConverterLog ConverterLogType, const bool bAppendComma)
{
    const int32 IndentLevel = GetIndentDepthFromKey(KeyPath);
    const FString Indent = FString::ChrN(IndentLevel * IndentLevelMultiply, ' ');

    const FString ValueStr = DiffValueString(OriginValueString, ChangedValueString);

    const FString PrettyKey = GetPrettyKey(KeyPath);
    const FString Comma = bAppendComma ? TEXT(",") : TEXT("");
    const FString Line = FString::Printf(TEXT("%s\"%s\": %s%s"), *Indent, *PrettyKey, *ValueStr, *Comma);
    AppendLog(ConverterLogType, *Line);
}

int32 SConvertToolsEditorWindowMenu::GetIndentDepthFromKey(const FString& Key)
{
    int32 Depth = 1;
    for (int32 i = 0; i < Key.Len(); ++i)
    {
        if (Key[i] == TEXT('.')) { Depth++; }
        else if (Key[i] == TEXT('[')) { Depth++; }
    }
    return Depth;
}

FString SConvertToolsEditorWindowMenu::GetPrettyKey(const FString& KeyPath)
{
    FString Key = KeyPath;

	// Remove Array Index
    int32 LastBracketIdx;
    if (KeyPath.FindLastChar(']', LastBracketIdx))
    {
        int32 OpenBracketIdx;
        if (KeyPath.FindLastChar('[', OpenBracketIdx) && OpenBracketIdx < LastBracketIdx)
        {
            Key = KeyPath.Mid(LastBracketIdx + 1);
        }
    }
    int32 LastDotIdx;
    if (Key.FindLastChar('.', LastDotIdx))
    {
        Key = Key.Mid(LastDotIdx + 1);
    }
    return Key.TrimStartAndEnd();
}
// Converter Log
END_SLATE_FUNCTION_BUILD_OPTIMIZATION