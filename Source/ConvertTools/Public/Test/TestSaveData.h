// Copyright (c) 2025 Moris0712. - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "TestSaveData.generated.h"

struct FObjectRecord;

USTRUCT()
struct FTestStructChild
{
	GENERATED_BODY()

	UPROPERTY()
	int32 num;

	friend FArchive& operator<<(FArchive& Ar, FTestStructChild& TheStruct)
	{
		Ar << TheStruct.num;
		return Ar;
	}
};

USTRUCT()
struct FTestStructParent
{
	GENERATED_BODY()

	UPROPERTY()
	FTestStructChild TestStructChild;

	UPROPERTY()
	int32 Index;

	friend FArchive& operator<<(FArchive& Ar, FTestStructParent& TheStruct)
	{
		Ar << TheStruct.Index;
		Ar << TheStruct.TestStructChild;
		return Ar;
	}
};

UCLASS()
class CONVERTTOOLS_API UTestSerializableObject: public UObject
{
	GENERATED_BODY()

public:
	virtual void Serialize(FArchive& Ar) override
	{
		Super::Serialize(Ar);
		Ar << A;
		Ar << B;
		Ar << C;
		Ar << D;
		Ar << E;
	}

	UPROPERTY()
	int32 A;
	UPROPERTY()
	FString B;
	UPROPERTY()
	int32 C;
	UPROPERTY()
	FTestStructParent D;
	UPROPERTY()
	float E;

	void FillTestData()
	{
		A = FMath::RandRange(1, 9);
		TArray<FString> randArray = { TEXT("abc"), TEXT("abcdefu"), TEXT("abcdefghijklmnopqrstuvwxyz") };
		B = randArray[FMath::RandRange(0, randArray.Num() - 1)];
		C = FMath::RandRange(1, 9);
		TArray<float> doubleArr = { 1.1f, -2.0f, 0.625f };
		D = FTestStructParent{ FTestStructChild {5}, 6 };
		E = doubleArr[FMath::RandRange(0, doubleArr.Num() - 1)];
	}
};


UCLASS()
class CONVERTTOOLS_API UTestSerializableSaveData : public USaveGame
{
	GENERATED_BODY()

public:
	void FillTestData();

	UPROPERTY()
	bool bValue;

	UPROPERTY()
	int32 INumber;

	UPROPERTY()
	float fNumber;

	UPROPERTY()
	int64 I64Number;

	UPROPERTY()
	FString String;

	UPROPERTY()
	TArray<bool> bArray;

	UPROPERTY()
	TArray<int32> IArray;

	UPROPERTY()
	TArray<float> fArray;

	UPROPERTY()
	TArray<int64> I64Array;

	UPROPERTY()
	TArray<FString> StringArray;

	UPROPERTY()
	TMap<FString, int> ItemLeftMap;

	UPROPERTY()
	FDateTime DateTimeStruct;

	UPROPERTY()
	TArray<FDateTime> DateTimeArray;

	UPROPERTY()
	int32 FixedArray[5];

	UPROPERTY()
	TArray<FTestStructParent> TestStructParentArray;

	UPROPERTY()
	TMap<FString, FTestStructParent> TestStructParentMap;

	UPROPERTY()
	TArray<FObjectRecord> SavedObjects;

	UPROPERTY()
	TArray<FObjectRecord> EmptySavedObjects;
};