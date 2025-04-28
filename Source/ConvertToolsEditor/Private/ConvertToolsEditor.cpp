// Copyright (c) 2025 Moris0712. - All Rights Reserved

#include "ConvertToolsEditor.h"
#include "ConvertToolsEditorStyle.h"
#include "ConvertToolsEditorCommands.h"
#include "SConvertToolsEditorWindowMenu.h"

#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

static const FName ConverterTabName("SavJsonConverter");

#define LOCTEXT_NAMESPACE "FConvertToolsEditorModule"

void FConvertToolsEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FConvertToolsEditorStyle::Initialize();
	FConvertToolsEditorStyle::ReloadTextures();

	FConvertToolsEditorCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);
	PluginCommands->MapAction(
		FConvertToolsEditorCommands::Get().OpenConvertToolsEditorWindowMenu,
		FExecuteAction::CreateRaw(this, &FConvertToolsEditorModule::OpenSaveEditTool),
		FCanExecuteAction());

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ConverterTabName,
		FOnSpawnTab::CreateRaw(this, &FConvertToolsEditorModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FConvertToolsEditorTabTitle", "Sav Json Convert Editor Tool"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	CreateMenuBar();

}

void FConvertToolsEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FConvertToolsEditorStyle::Shutdown();

	FConvertToolsEditorCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ConverterTabName);
}


void FConvertToolsEditorModule::CreateMenuBar()
{
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	const TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());

	MenuExtender->AddMenuBarExtension(
		TEXT("Help"),
		EExtensionHook::After,
		PluginCommands,
		FMenuBarExtensionDelegate::CreateRaw(this, &FConvertToolsEditorModule::AddMenuBar)
	);

	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
}

void FConvertToolsEditorModule::AddMenuBar(FMenuBarBuilder& MenuBarBuilder)
{
	MenuBarBuilder.AddPullDownMenu(
		FText::FromString("Converter"),
		FText::FromString("Open Sav Json Converter Editor Tool"),
		FNewMenuDelegate::CreateRaw(this, &FConvertToolsEditorModule::FillMenu));
}

void FConvertToolsEditorModule::FillMenu(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.BeginSection("Edit", LOCTEXT("Edit", "Edit"));
	MenuBuilder.AddMenuEntry(FConvertToolsEditorCommands::Get().OpenConvertToolsEditorWindowMenu);
	MenuBuilder.EndSection();
}

TSharedRef<SDockTab> FConvertToolsEditorModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[

			SNew(SConvertToolsEditorWindowMenu)
				// Put your tab content here!
				//.OnRefreshListView(FOnRefreshListView::CreateRaw(this, &FMyPluginModule::OnRefreshListView))
					// Put your tab content here!
				//SNew(SBox)
				//.HAlign(HAlign_Center)
				//.VAlign(VAlign_Center)
				//[
				//	SNew(STextBlock)
				//	.Text(WidgetText)
				//]
		];
}

void FConvertToolsEditorModule::OpenSaveEditTool()
{
	FGlobalTabmanager::Get()->TryInvokeTab(ConverterTabName);
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FConvertToolsEditorModule, ConvertToolsEditor)