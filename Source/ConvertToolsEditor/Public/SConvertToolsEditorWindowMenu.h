// Copyright (c) 2025 Moris0712. - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class FSlateTextLayout;
class SMultiLineEditableTextBox;
class SRichTextBlock;

namespace LogInternal
{
	enum class EConverterLog : uint8;
}

class USaveGame;



class CONVERTTOOLSEDITOR_API SConvertToolsEditorWindowMenu : public SCompoundWidget, public FGCObject
{
public:
	SLATE_BEGIN_ARGS(SConvertToolsEditorWindowMenu)
	{}
		//SLATE_EVENT(FOnRefreshListView, OnRefreshListView)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);


public:

protected:
	// Protected GC
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override
	{
		Collector.AddReferencedObject(SelectedSaveGame);
		Collector.AddReferencedObject(ChangedSaveGame);
	}
	virtual FString GetReferencerName() const override
	{
		return TEXT("SConvertToolsEditorWindowMenu");
	}

private:
	// TEST
	FReply CreateTestSav();
	// TEST


	// ----- Sav To Json -----
	TSharedRef<SWidget> SavToJsonWidget();

	FReply OpenLoadSavFileBrowser();
	FReply OpenSaveJsonFileBrowser();

	void LoadSaveFile(const FString& FilePath);
	void SaveToJsonFile(const FString& FilePath);
	TSharedPtr<FJsonObject> ConvertSavToJson(USaveGame* SaveGame);
	// ----- Sav To Json -----


	// ----- Json To Sav -----
	TSharedRef<SWidget> JsonToSavWidget();

	FReply OpenLoadJsonFileBrowser();
	FReply OpenSaveSavFileBrowser();

	void LoadJsonFile(const FString& FilePath);

	USaveGame* ConvertJsonToSav(const TSharedPtr<FJsonObject>& JsonObject);
	void JsonToSavFile(const FString& FilePath, USaveGame* SaveGame);
	// ----- Json To Sav -----


	// ConverterLog
	TSharedRef<SWidget> ConverterLogWidget();

	void AppendLog(const LogInternal::EConverterLog ConverterLogType, const TCHAR* Format, ...);

	void OnCheckBoxStateChanged(ECheckBoxState NewState);
	ECheckBoxState IsCheckBoxChecked() const;

	FReply ClearLog();
	TSharedRef<FSlateTextLayout> OnCreateTextLayout(SWidget* Widget, const FTextBlockStyle& TextStyle);

	void CompareAndLogJsonValues(const TSharedPtr<FJsonObject>& OriginJsonObject, const TSharedPtr<FJsonObject>& ChangedJsonObject);
	void CompareJsonRecursive(const FString& KeyPath, const TSharedPtr<FJsonValue>& OriginValue, const TSharedPtr<FJsonValue>& ChangedValue, const bool bAppendComma);
	FString DiffValueString(const FString& OriginValueString, const FString& ChangedValueString);
	void AppendFormattedLog(const FString& KeyPath, const FString& OriginValueString, const FString& ChangedValueString, const LogInternal::EConverterLog ConverterLogType, const bool bAppendComma);

	int32 GetIndentDepthFromKey(const FString& Key);
	FString GetPrettyKey(const FString& KeyPath);
	// ConverterLog

	void Notify(const TSharedPtr<STextBlock>& TextBlock, const LogInternal::EConverterLog ConverterLogType, const TCHAR* Format, ...);
	
public:
protected:
private:
	// ----- Sav To Json -----
	TSharedPtr<STextBlock> SaveFileTextBlock;
	TSharedPtr<STextBlock> SavNoticeTextBlock;
	TSharedPtr<SButton> ChangeJsonButton;
	// ----- Sav To Json -----

	// ----- Json To Sav -----
	TSharedPtr<STextBlock> JsonFileTextBlock;
	TSharedPtr<STextBlock> JsonNoticeTextBlock;
	TSharedPtr<SButton> ChangeSavButton;
	// ----- Json To Sav -----

	// ----- Log Panel -----
	TSharedPtr<SScrollBox> LogScrollBox;
	TSharedPtr<SMultiLineEditableTextBox> LogTextBox;
	TSharedPtr<FSlateTextLayout> LogTextLayout;
	// ----- Log Panel -----

 	TObjectPtr<USaveGame> SelectedSaveGame;
	TObjectPtr<USaveGame> ChangedSaveGame;

	bool bScrollEnded = true;
};

