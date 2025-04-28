// Copyright (c) 2025 Moris0712. - All Rights Reserved

#include "ConvertToolsEditorStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FConvertToolsEditorStyle::StyleInstance = nullptr;

void FConvertToolsEditorStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FConvertToolsEditorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FConvertToolsEditorStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("ConvertToolsEditorStyle"));
	return StyleSetName;
}

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FConvertToolsEditorStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("ConvertToolsEditorStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("SavJsonConverter")->GetBaseDir() / TEXT("Resources"));

	Style->Set("ConvertToolsEditor.OpenPluginWindow", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon20x20));

	Style->Set("ConvertToolsEditor.Log", FTextBlockStyle{}
		.SetFont(FAppStyle::Get().GetFontStyle("NormalFont"))
		.SetColorAndOpacity(FSlateColor(FLinearColor::White))
	);

	return Style;
}

void FConvertToolsEditorStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FConvertToolsEditorStyle::Get()
{
	return *StyleInstance;
}
