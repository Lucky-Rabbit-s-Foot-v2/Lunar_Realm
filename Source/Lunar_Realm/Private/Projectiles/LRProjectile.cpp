// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles/LRProjectile.h"

ALRProjectile::ALRProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ALRProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALRProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ALRProjectile::OnPoolActivate_Implementation()
{
	// TODO: 인터페이스 참고
}

void ALRProjectile::OnPoolDeactivate_Implementation()
{
	// TODO: 인터페이스 참고
}

