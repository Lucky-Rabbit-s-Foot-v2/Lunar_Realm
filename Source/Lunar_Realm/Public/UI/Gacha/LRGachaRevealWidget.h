// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/BaseWidget.h"
#include "Data/Gacha/LRGachaTypes.h"
#include "LRGachaRevealWidget.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRGachaRevealWidget : public UBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	// 트랜잭션 포함 시작
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Reveal")
	void StartRevealWithTransaction(FName InBannerID, FGuid InTxnId, const TArray<FLRGachaResult>& InResults);

	// BP 버튼에서 호출할 커밋/취소
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Reveal")
	void ConfirmCommit();	// 지급 확정

	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Reveal")
	void CancelAndRefund();	// 취소/환불(테스트용 혹은 뒤로가기)

protected:
	// 블루프린트에서 구현할 연출 진입점
	// 등급별 카드색/이팩트/사운드/실루엣->리빌 시퀸스를 여기서 처리
	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Gacha|Reveal")
	void BP_PlayRevealSequence(const TArray<FLRGachaResult>& InResults);

	// 블루프린트에 "가속/스킵 상태 변경" 알려주기(애니메이션 속도 조절용)
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
};
