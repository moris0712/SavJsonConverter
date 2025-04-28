// Copyright (c) 2025 Moris0712. - All Rights Reserved

#include "Test/TestSaveData.h"
#include "SaveGameData.h"
#include "ConvertToolsPropertyRuleBPLibrary.h"


void UTestSerializableSaveData::FillTestData()
{
	bValue = true;
	INumber = 150;
	fNumber = 106.123;
	I64Number = 21000000000;
	String = TEXT("TEST String");

	bArray = { 1,0,1,1,0 };
	IArray = { 2, 3, 5, 7, 11, 13, 17, 19 };
	fArray = { 5.0, 3.123, 6.32423123132132 };
	I64Array = { 21000000000, 21000000000 - 1, 21000000000 - 2, 21000000000 - 3 };
	StringArray = { TEXT("A"), TEXT("B") , TEXT("C") , TEXT("D"), TEXT("E") };

	ItemLeftMap = {
		{TEXT("Item1"), 1 },
		{TEXT("Item2"), 2 },
		{TEXT("Item3"), 3 },
		{TEXT("Item4"), 4 }
	};

	DateTimeStruct = FDateTime::Now();
	DateTimeArray = {
		FDateTime::Now(), FDateTime::Now(), FDateTime::Now(), FDateTime::Now(), FDateTime::Now()
	};

	FixedArray[0] = 10;
	FixedArray[1] = 20;
	FixedArray[2] = 30;
	FixedArray[3] = 40;
	FixedArray[4] = 50;

	TestStructParentArray = {
		FTestStructParent{ FTestStructChild{1}, 2},
		FTestStructParent{ FTestStructChild{3}, 4},
		FTestStructParent{ FTestStructChild{5}, 6}
	};

	TestStructParentMap = {
		{TEXT("abc1"), FTestStructParent{ FTestStructChild{1}, 2}},
		{TEXT("abc2"), FTestStructParent{ FTestStructChild{3}, 4}},
		{TEXT("abc3"), FTestStructParent{ FTestStructChild{5}, 6}}
	};


	TArray<UTestSerializableObject*> TestArray;
	for (int i = 0; i < 5; ++i)
	{
		UTestSerializableObject* TestObject = NewObject<UTestSerializableObject>(this, UTestSerializableObject::StaticClass());
		TestObject->FillTestData();
		TestArray.Add(TestObject);
	}

	SavedObjects.Empty();
	for (UTestSerializableObject* ItemElement : TestArray)
	{
		SavedObjects.Add(UConvertToolsPropertyRuleBPLibrary::SerializeObjectRecord(ItemElement));
	}
}
