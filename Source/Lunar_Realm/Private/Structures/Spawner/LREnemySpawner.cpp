// Fill out your copyright notice in the Description page of Project Settings.


#include "Structures/Spawner/LREnemySpawner.h"
#include "Units/Enemy/LREnemyCharacter.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Engine/GameInstance.h"


// Sets default values
ALREnemySpawner::ALREnemySpawner()
{
 //	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	//// GameDataSubsystem에서 TArray<int32> 타입으로 가져온 EnemyIDs를 스테이지에 맞게 파싱해서 그 값을 저장해둔다.
	//// 저장한 값을 토대로 타이머를 통해서 시간마다 소환 Or.. (소환 타이밍은 추후 고려)
	//// 소환 과정: 파싱해서 저장된 값을 Enemy한테 전달
	////			 전달 받은 아이디를 토대로 Enemy 클래스에서 이후 알아서 동작

	//// 에너미 아이디 목록 가져와서 저장
	//UGameInstance* GI = GetGameInstance();
	//UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>();
	//CachedEnemyIDs = DataSys->GetAllEnemyIDs();

	//// 특정 아이디로 확정하는 로직
	//

	//// => 결과값은 int32
	


}

// Called when the game starts or when spawned
void ALREnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALREnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALREnemySpawner::SpawnEnemy()
{
	

	//// 1. 풀링 서브시스템 가져오기
	//UPoolingSubsystem* PoolSys = GetWorld()->GetSubsystem<UPoolingSubsystem>();
	//if (!PoolSys || !TargetEnemeyclass) return;

	//// 2. 스폰 위치 설정
	//FTransform SpawnTransform = GetActorTransform();

	//// 3. 풀에서 꺼내기 (Spawn<T> 템플릿 사용 권장)
	//// 자동으로 캐스팅되어 나옵니다.
	//ALREnemyCharacter* NewEnemy = PoolSys->Spawn<ALREnemyCharacter>(TargetEnemeyclass, SpawnTransform);

	////// (필요 시 추가 로직)
	////if (NewBullet)
	////{
	////	// 예: 발사한 주인 설정
	////	NewBullet->SetOwner(this);
	////}

}

