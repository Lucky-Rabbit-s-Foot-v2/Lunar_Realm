// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/LROptionDataStructs.h"

//============================================================================
// Sound Option Data
//============================================================================

void FSoundOptionData::ApplyDefaults()
{
	*this = FSoundOptionData();
}

//============================================================================
// Graphic Option Data
//============================================================================

void FGraphicOptionData::ApplyDefaults()
{
	// 헤더에 적힌 기본 값을 가진 새 객체로 초기화
	*this = FGraphicOptionData();
}

