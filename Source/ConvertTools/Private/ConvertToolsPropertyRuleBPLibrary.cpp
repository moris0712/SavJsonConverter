// Copyright (c) 2025 Moris0712. - All Rights Reserved

#include "ConvertToolsPropertyRuleBPLibrary.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "JsonObjectConverter.h"
#include "SaveGameData.h"
#include "UObject/Package.h"

const FString ObjectClassKey = TEXT("ObjectClass");
const FString ObjectNameKey = TEXT("ObjectName");
const FString ObjectDataKey = TEXT("ObjectData");


FObjectRecord UConvertToolsPropertyRuleBPLibrary::SerializeObjectRecord(UObject* Element)
{
    FObjectRecord ObjectRecord;
    ObjectRecord.ObjectClass = Element->GetClass();
    ObjectRecord.ObjectName = Element->GetName();

    FMemoryWriter MemoryWriter{ ObjectRecord.ObjectData, true };

    FConvertToolsGameArchive Ar{ MemoryWriter };
	Element->Serialize(Ar);
    
    return ObjectRecord;
}

UObject* UConvertToolsPropertyRuleBPLibrary::DeSerializeObjectRecord(const FObjectRecord& ObjectRecord)
{
    TArray<uint8> ObjectData = ObjectRecord.ObjectData;

    FMemoryReader MemReader{ ObjectData };
    UObject* CreatedObject = NewObject<UObject>(GetTransientPackage(), ObjectRecord.ObjectClass, FName(*ObjectRecord.ObjectName));

    FConvertToolsGameArchive Ar{ MemReader };
    CreatedObject->Serialize(Ar);

    return CreatedObject;
}

#if WITH_EDITOR
TSharedPtr<FJsonValue> UConvertToolsPropertyRuleBPLibrary::GetJsonValue(FProperty* Property, void* ValuePtr)
{
    if (FStructProperty* StructProp = CastField<FStructProperty>(Property); StructProp != nullptr)
    {
        return GetStructJsonValue(StructProp, ValuePtr);
    }
    if (const FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property); ArrayProp != nullptr)
    {
        return GetJsonArrayValue(ArrayProp, ValuePtr);
    }
    return FJsonObjectConverter::UPropertyToJsonValue(Property, ValuePtr);
}

TSharedPtr<FJsonValue> UConvertToolsPropertyRuleBPLibrary::GetStructJsonValue(FStructProperty* StructProp, void* ValuePtr)
{
    const TSharedPtr<FJsonObject> StructJson = MakeShareable(new FJsonObject());

	if (StructProp->Struct != FObjectRecord::StaticStruct())
	{
        return FJsonObjectConverter::UPropertyToJsonValue(StructProp, ValuePtr);
	}

    // ObjectRecord Located in ValuePtr
    const FObjectRecord* ObjectRecord = reinterpret_cast<FObjectRecord*>(ValuePtr);
    if (ObjectRecord != nullptr)
    {
        if (ObjectRecord->ObjectClass == nullptr) return MakeShared<FJsonValueObject>(StructJson);

        // Add Json ObjectRecord's Property
        StructJson->SetStringField(ObjectClassKey, ObjectRecord->ObjectClass->GetName());
        StructJson->SetStringField(ObjectNameKey, ObjectRecord->ObjectName);

        // Create Instance From ObjectRecord
        UObject* CreatedObject = DeSerializeObjectRecord(*ObjectRecord);
        TSharedPtr<FJsonObject> ObjectJson = MakeShareable(new FJsonObject());
        if (FJsonObjectConverter::UStructToJsonObject(CreatedObject->GetClass(), CreatedObject, ObjectJson.ToSharedRef(), 0, 0))
        {
            StructJson->SetObjectField(ObjectDataKey, ObjectJson);
        }
        else
        {
            return MakeShared<FJsonValueObject>(StructJson);
        }
    }
    return MakeShared<FJsonValueObject>(StructJson);
}

TSharedPtr<FJsonValueArray> UConvertToolsPropertyRuleBPLibrary::GetJsonArrayValue(const FArrayProperty* ArrayProp, const void* ValuePtr)
{
    TArray<TSharedPtr<FJsonValue>> JsonArray;

    FScriptArrayHelper ArrayHelper(ArrayProp, ValuePtr);

    const int32 NumElements = ArrayHelper.Num(); 
    for (int32 Index = 0; Index < NumElements; ++Index)
    {
        void* ElementPtr = ArrayHelper.GetRawPtr(Index);

        TSharedPtr<FJsonValue> JsonValue = GetJsonValue(ArrayProp->Inner, ElementPtr);
        if (JsonValue.IsValid()) { JsonArray.Push(JsonValue); }
    }
    return MakeShared<FJsonValueArray>(JsonArray);
}


// --------------------------------------------------
bool UConvertToolsPropertyRuleBPLibrary::AddSaveField(const TSharedPtr<FJsonValue>& JsonValue, FProperty* Property, void* ValuePtr, FText* OutFailReason)
{
	if (FStructProperty* StructProp = CastField<FStructProperty>(Property); StructProp != nullptr)
    {
        return AddSaveStructField(JsonValue, StructProp, ValuePtr, OutFailReason);
    }
    else if (FArrayProperty* ArrayProp = CastField<FArrayProperty>(Property); ArrayProp != nullptr)
    {
        return AddSaveArrayField(JsonValue, ArrayProp, ValuePtr, OutFailReason);
    }
    return FJsonObjectConverter::JsonValueToUProperty(JsonValue, Property, ValuePtr, 0, 0, false, OutFailReason);
}

bool UConvertToolsPropertyRuleBPLibrary::AddSaveStructField(const TSharedPtr<FJsonValue>& JsonValue, FStructProperty* StructProp, void* ValuePtr, FText* OutFailReason)
{
    if(StructProp->Struct != FObjectRecord::StaticStruct())
    {
        return FJsonObjectConverter::JsonValueToUProperty(JsonValue, StructProp, ValuePtr, 0, 0, false, OutFailReason);
    }

    FObjectRecord* ObjectRecord = reinterpret_cast<FObjectRecord*>(ValuePtr);
    if (ObjectRecord == nullptr)
    {
        *OutFailReason = FText::FromString(TEXT("AddSaveStructField: FObjectRecord is null"));
        return false;
    }

    const TSharedPtr<FJsonObject> ObjectJson = JsonValue->AsObject();
    FString ObjectClassName;
    FString ObjectName;
    if (ObjectJson->TryGetStringField(ObjectClassKey, ObjectClassName))
    {
        ObjectRecord->ObjectClass = FindObject<UClass>(ANY_PACKAGE, *ObjectClassName);
    }
    if (ObjectJson->TryGetStringField(ObjectNameKey, ObjectName))
    {
        ObjectRecord->ObjectName = ObjectName;
    }

    if (ObjectJson->HasField(ObjectDataKey))
    {
        const TSharedPtr<FJsonObject> ObjectDataJson = ObjectJson->GetObjectField(ObjectDataKey);

        // Create Instance From ObjectRecord
        UObject* CreatedObject = DeSerializeObjectRecord(*ObjectRecord);
        if (CreatedObject != nullptr && 
            FJsonObjectConverter::JsonObjectToUStruct(ObjectDataJson.ToSharedRef(), CreatedObject->GetClass(), CreatedObject, 0, 0))
        {
            const FObjectRecord SerializedObjectRecord = SerializeObjectRecord(CreatedObject);
            *ObjectRecord = SerializedObjectRecord;
        }
        else
        {
            *OutFailReason = FText::FromString(TEXT("Failed to deserialize ObjectData for FObjectRecord"));
            return false;
        }
    }
    return true;
}

bool UConvertToolsPropertyRuleBPLibrary::AddSaveArrayField(const TSharedPtr<FJsonValue>& JsonValue, const FArrayProperty* ArrayProp, void* ValuePtr, FText* OutFailReason)
{
    const TArray<TSharedPtr<FJsonValue>>& JsonArray = JsonValue->AsArray();
    const int32 Length = JsonArray.Num();

    FScriptArrayHelper ArrayHelper(ArrayProp, ValuePtr);
    ArrayHelper.Resize(Length);

    for (int32 Index = 0; Index < Length; ++Index)
    {
        const TSharedPtr<FJsonValue>& JsonElement = JsonArray[Index];
        if (JsonElement.IsValid() && !JsonElement->IsNull())
        {
            void* ElementPtr = ArrayHelper.GetRawPtr(Index);
            FText ArrayElementFailReason;
            if (!AddSaveField(JsonElement, ArrayProp->Inner, ElementPtr, &ArrayElementFailReason))
            {
                *OutFailReason = FText::FromString(OutFailReason->ToString() + "\n" + FString::Printf(TEXT("[%d] Failed to Add Array Element -  %s"), Index, *ArrayElementFailReason.ToString()));
            }
        }
    }
    return true;
}

bool UConvertToolsPropertyRuleBPLibrary::IsJsonValueEmpty(const TSharedPtr<FJsonValue>& JsonValue)
{
    if (!JsonValue.IsValid() || JsonValue->Type == EJson::Null)
        return true;

    switch (JsonValue->Type)
    {
	    case EJson::String:
	    {
	        return JsonValue->AsString().IsEmpty();
	    }
	    case EJson::Array: 
	    {
	        return JsonValue->AsArray().Num() == 0;
	    }
	    case EJson::Object:
	    {
	        const TSharedPtr<FJsonObject> Obj = JsonValue->AsObject();
	        return !Obj.IsValid() || Obj->Values.Num() == 0;
	    }
	    default:
	        return false; // Num, Bool Can't be Empty
    }
}

FString UConvertToolsPropertyRuleBPLibrary::JsonValueToString(const TSharedPtr<FJsonValue>& Value)
{
    if (!Value.IsValid())
        return TEXT("null");

    switch (Value->Type)
    {
    case EJson::String:
        return FString::Printf(TEXT("\"%s\""), *Value->AsString());
    case EJson::Number:
        return FString::SanitizeFloat(Value->AsNumber());
    case EJson::Boolean:
        return Value->AsBool() ? TEXT("true") : TEXT("false");
    case EJson::Array:
    case EJson::Object:
    {
        FString OutStr;
        TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutStr);
        FJsonSerializer::Serialize(Value, TEXT(""), Writer);
        return OutStr;
    }
    case EJson::Null:
    default:
        return TEXT("null");
    }
}
#endif