// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingDamage/LRFloatingDamage.h"

ALRFloatingDamage::ALRFloatingDamage()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ALRFloatingDamage::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALRFloatingDamage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALRFloatingDamage::OnPoolActivate_Implementation()
{
	// TODO: 인터페이스 참고
}

void ALRFloatingDamage::OnPoolDeactivate_Implementation()
{
	// TODO: 인터페이스 참고
}