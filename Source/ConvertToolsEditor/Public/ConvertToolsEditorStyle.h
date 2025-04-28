// Copyright (c) 2025 Moris0712. - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

/**  */
class CONVERTTOOLSEDITOR_API FConvertToolsEditorStyle
{
public:
	static void Initialize();
	static void Shutdown();

	/** reloads textures used by slate renderer */
	static void ReloadTextures();
	/** @return The Slate style set for the Shooter game */
	static const ISlateStyle& Get();
	static FName GetStyleSetName();

private:
	static TSharedRef< class FSlateStyleSet > Create();

private:
	static TSharedPtr< class FSlateStyleSet > StyleInstance;
};