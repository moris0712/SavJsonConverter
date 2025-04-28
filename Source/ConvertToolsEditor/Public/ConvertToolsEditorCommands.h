// Copyright (c) 2025 Moris0712. - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "ConvertToolsEditorCommands.h"

class CONVERTTOOLSEDITOR_API FConvertToolsEditorCommands : public TCommands<FConvertToolsEditorCommands>
{
public:

	FConvertToolsEditorCommands()
		: TCommands<FConvertToolsEditorCommands>(TEXT("ConvertToolsEditor"), NSLOCTEXT("Contexts", "ConvertToolsEditor", "ConvertToolsEditor Plugin"), NAME_None, FConvertToolsEditorCommands::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenConvertToolsEditorWindowMenu;
};