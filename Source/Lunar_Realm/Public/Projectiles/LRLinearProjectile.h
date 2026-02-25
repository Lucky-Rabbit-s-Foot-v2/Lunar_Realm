// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectiles/LRProjectile.h"
#include "LRLinearProjectile.generated.h"

//============================================================================
/**
 * 직선형 투사체 클래스
 * - 발사 방향으로 일직선 비행
 * - 베이스 로직만으로 동작
 * - 파라미터 : Speed, Lifetime
 */
//============================================================================
// (260224) KHS 제작. 제반 사항 구현.
//============================================================================
UCLASS()
class LUNAR_REALM_API ALRLinearProjectile : public ALRProjectile
{
	GENERATED_BODY()
	
	//베이스 로직만으로 동작하므로 추가 구현 X
};
