// Copyright (c) 2025 Moris0712. - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "SaveGameData.generated.h"

struct FConvertToolsGameArchive : public FObjectAndNameAsStringProxyArchive
{
	FConvertToolsGameArchive(FArchive& InInnerArchive)
		: FObjectAndNameAsStringProxyArchive(InInnerArchive, true)
	{
		ArIsSaveGame = true;
		ArNoDelta = true;
		// Optional, useful when saving/loading variables without resetting the level.
		// Serialize variables even if weren't modified and mantain their default values.
	}

};

// Object Serialize Struct
USTRUCT()
struct FObjectRecord
{
	GENERATED_BODY()

	UPROPERTY()
	UClass* ObjectClass;

	UPROPERTY()
	FString ObjectName;

	UPROPERTY()
	TArray<uint8> ObjectData;
};