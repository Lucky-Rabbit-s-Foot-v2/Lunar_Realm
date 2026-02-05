// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Data/Gacha/LRGachaTypes.h"
#include "UI/BaseWidget.h"

#include "LRGachaRevealWidget.generated.h"

/**
 * 리빌 위젯
 * 결과를 받아서 BP_PlayRevealSequence로 연출 시작
 * 클릭으로 스킵(가속/완전스킵) 처리
 * UIManager가 커서를 끄는 문제를 "다음 틱에 입력모드/커서 강제"로 덮어쓰기
 */

UCLASS()
class LUNAR_REALM_API ULRGachaRevealWidget : public UBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	// 트랜잭션 포함 시작 (UI 연출용으로 결과 받기)
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Reveal")
	void StartRevealWithTransaction(FName InBannerID, FGuid InTxnId, const TArray<FLRGachaResult>& InResults);

	// 다음 틱에 UI 입력/커서를 강제하는 유틸 함수
	UFUNCTION(BlueprintCallable)
	void ForceUIInputNextTick();

	// 리빌 종료(결과 확인까지 끝났을 때 호출)
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Reveal")
	void FinishRevealAndClose();

protected:
	// 블루프린트에서 구현할 연출 진입점
	// 등급별 카드색/이팩트/사운드/실루엣->리빌 시퀸스를 여기서 처리
	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Gacha|Reveal")
	void BP_PlayRevealSequence(const TArray<FLRGachaResult>& InResults);

	// BP에 스킵 상태 변경 알려주기(애니메이션 속도 조절 등)
	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Gacha|Reveal")
	void BP_OnSkipStateChanged(int32 NewSkipState);

	// 스킵/가속 입력 처리
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	// 현재 결과 캐시(스킵/가속시 필요)
	UPROPERTY()
	TArray<FLRGachaResult> CachedResults;

	FGuid CachedTxnId;	//	저장
	FName CachedBannerId;

	// 0=정상, 1=가속, 2=완전 스킵(끝까지)
	int32 SkipState = 0;

	void CloseSelfPopup();	//	UIManager로 닫기

	// SkipState=2(스킵)으로 결과를 보여준 뒤, 다음 클릭에 닫기용
	bool bSkipResultsShown = false;
};
