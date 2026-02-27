// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Equipment/LREquipmentBase.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/GameInstance.h"
#include "Subsystems/GameDataSubsystem.h"

// Sets default values
ALREquipmentBase::ALREquipmentBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
}

bool ALREquipmentBase::InitEquipment(FName InEquipmentID)
{
	//기존 메시정보가 있다면 초기화
	MeshComp->SetStaticMesh(nullptr);
	CachedEquipmentID = InEquipmentID;

	UGameInstance* GI = GetGameInstance();
	check(GI);

	UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>();
	if (!ensureMsgf(DataSys, TEXT("데이터시스템이 없으면 망한겁니다")))
	{
		return false;
	}

	const FEquipmentStaticData& EquipData = DataSys->GetEquipmentStaticData(CachedEquipmentID);

	// 새 메시 세팅
	if (!EquipData.EquipmentMesh.IsNull())
	{
		UStaticMesh* Mesh = EquipData.EquipmentMesh.LoadSynchronous();
		MeshComp->SetStaticMesh(Mesh);
		return true;
	}
	
	return false;
}

// Called when the game starts or when spawned
void ALREquipmentBase::BeginPlay()
{
	Super::BeginPlay();
	
}
