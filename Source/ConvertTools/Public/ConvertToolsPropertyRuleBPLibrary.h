// Copyright (c) 2025 Moris0712. - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ConvertToolsPropertyRuleBPLibrary.generated.h"

struct FObjectRecord;
class USaveGame;

UCLASS()
class CONVERTTOOLS_API UConvertToolsPropertyRuleBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static FObjectRecord SerializeObjectRecord(UObject* Element);
	static UObject* DeSerializeObjectRecord(const FObjectRecord& ObjectData);

#if WITH_EDITOR
	static TSharedPtr<FJsonValue> GetJsonValue(FProperty* Property, void* ValuePtr);
	static TSharedPtr<FJsonValue> GetStructJsonValue(FStructProperty* StructProp, void* ValuePtr);
	static TSharedPtr<FJsonValueArray> GetJsonArrayValue(const FArrayProperty* ArrayProp, const void* ValuePtr);

	static bool AddSaveField(const TSharedPtr<FJsonValue>& JsonValue, FProperty* Property, void* ValuePtr, FText* OutFailReason);
	static bool AddSaveStructField(const TSharedPtr<FJsonValue>& JsonValue, FStructProperty* StructProp, void* ValuePtr, FText* OutFailReason);
	static bool AddSaveArrayField(const TSharedPtr<FJsonValue>& JsonValue, const FArrayProperty* ArrayProp, void* ValuePtr, FText* OutFailReason);

	static bool IsJsonValueEmpty(const TSharedPtr<FJsonValue>& JsonValue);
	static FString JsonValueToString(const TSharedPtr<FJsonValue>& Value);
#endif
};