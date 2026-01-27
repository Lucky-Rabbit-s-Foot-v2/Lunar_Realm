// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/LRCharacter.h"

ALRCharacter::ALRCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ALRCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALRCharacter::OnPoolActivate_Implementation()
{
	/* 풀 할당할 때 진행할 로직 작성
	* 시각적/물리적 상태 복구
	* 데이터를 할당 받아서 스텟 등 복구
	* 움직임 멈춤 해제
	* 빙의 등
	*/
}

void ALRCharacter::OnPoolDeactivate_Implementation()
{
	/* 풀 반환할 때 진행할 로직 작성
	* 모든 동작 정지
	* 타이머 정리
	* GAS 효과 제거 등
	*/
}

void ALRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

