// Copyright (c) 2025 Moris0712. - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class SConvertToolsEditorWindowMenu;
class FToolBarBuilder;
class FMenuBuilder;

class CONVERTTOOLSEDITOR_API FConvertToolsEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command (by default it will bring up plugin window) */
	//void PluginButtonClicked();
	
private:
	void CreateMenuBar();
	void AddMenuBar(FMenuBarBuilder& MenuBarBuilder);
	void FillMenu(FMenuBuilder& Builder);

	//void RegisterMenus();

	void OpenSaveEditTool();

	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

private:
	TSharedPtr<class FUICommandList> PluginCommands;

	TSharedPtr <SConvertToolsEditorWindowMenu> ConvertToolsEditorWindowMenu;
};
