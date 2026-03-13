// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "LRPureButton.generated.h"

/**
 * 배경이 투명하고 클릭 영역만 존재하는 투명 버튼 클래스
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class LUNAR_REALM_API ULRPureButton : public UButton
{
	GENERATED_BODY()
	
public:
	ULRPureButton();

protected:
	// 에디터에서 수정 후 실행 시 스타일이 초기화되는 것을 방지하기 위함.
	virtual void SynchronizeProperties() override;

private:
	void ApplyTransparentStyle();
};
