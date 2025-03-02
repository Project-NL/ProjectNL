// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameInstance.h"

UBaseGameInstance::UBaseGameInstance()
{
	LoadItemData();
}

const TArray<FItemInfoData>& UBaseGameInstance::GetItemInfoList()
{
	 return ItemInfoList; 
}


void UBaseGameInstance::Init()
{
	Super::Init();

	//LoadItemData();
}


void UBaseGameInstance::LoadItemData()
{
	// DataTable 만들면 주소 변경
	const static ConstructorHelpers::FObjectFinder<UDataTable>

		ItemInfoDataTable(TEXT("/Script/Engine.DataTable'/Game/Blueprints/Item/DT_Item.DT_Item'"));

	if (ItemInfoDataTable.Succeeded())
	{
		TArray<FItemInfoData*> TempItemInfoList;
		ItemInfoDataTable.Object->GetAllRows<FItemInfoData>(TEXT(""), TempItemInfoList);
		
		for (const FItemInfoData* InfoItem : TempItemInfoList)
		{
			ItemInfoList.Add(*InfoItem);
		}
	}
}
