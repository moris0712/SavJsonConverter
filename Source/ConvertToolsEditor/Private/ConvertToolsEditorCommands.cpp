// Copyright (c) 2025 Moris0712. - All Rights Reserved

#include "ConvertToolsEditorCommands.h"

#define LOCTEXT_NAMESPACE "FConvertToolsEditorModule"

void FConvertToolsEditorCommands::RegisterCommands()
{
	UI_COMMAND(OpenConvertToolsEditorWindowMenu, "Sav Json Converter Tool", "Open Sav Json Converter Window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE