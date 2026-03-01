// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/LRDataStructs.h"
#include "GameFramework/Actor.h"
#include "Interfaces/LRPoolableInterface.h"
#include "LRProjectile.generated.h"

//============================================================================
/**
 * 투사체(Projectile) 베이스 클래스
 * - 오브젝트 풀링 처리
 * - GA로부터 InitData받아 초기화
 * - 충돌시 GE 적용후 풀 복귀
 */
//============================================================================
// (260128) PJB 제작. 제반 사항 구현.
// (260224) KHS 수정. GA/GE처리
// (260226) KHS 수정. VFX/SFX 처리
//============================================================================

UCLASS()
class LUNAR_REALM_API ALRProjectile : public AActor, public ILRPoolableInterface
{
	GENERATED_BODY()
	
public:	
	ALRProjectile();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) final;
	virtual void OnLifeTimeExpired() final;
	
	//오버라이드 사용 함수
	virtual void OnPoolActivate_Implementation();
	virtual void OnPoolDeactivate_Implementation();
	
	//자식 타입별 초기화 처리
	virtual void OnSkillObjectInitialized() { }
	//자식 타입별 생명주기 만료 처리
	virtual void OnSkillObjectExpired() { }
	//자식 스킬 타입별 충돌 처리 
	//@return true면 베이스가 풀 복귀, false면 자식이 직접 풀복귀
	virtual bool OnSkillObjectHit(AActor* OtherActor, const FHitResult& Hit) { return true; }
	
	
	
	//GE적용 헬퍼
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> Effect, float DamageValue);
	
public:	
	virtual void Tick(float DeltaTime) override;

	/**
	* 오브젝트 풀링 헬퍼 함수
	* - 자손 클래스의 베이스 클래스 타입 반환
	*/
	UFUNCTION(BlueprintPure)
	TSubclassOf<ALRProjectile> GetBaseClass() const { return TSubclassOf<ALRProjectile>(GetClass()); }

	//오버라이드 금지(final) 
	//GA가 호출 -> InitData받아서 초기화 -> OnSkillObjectInitialized 호출
	virtual void InitSkillObject(const FSkillObjectInitData& Initdata) final;
	
private:
	//FX 재생 헬퍼
	void PlaySpawnEffects();
	void PlayImpactEffects();
	
	FTimerHandle LifeTimeTimerHandle;
	bool bIsDeactivated = false;
	
protected:
	UPROPERTY(VisibleAnywhere, Category = "LR|Projectile")
	TObjectPtr<class USphereComponent> SphereComp;
	UPROPERTY(VisibleAnywhere, Category = "LR|Projectile")
	TObjectPtr<class UProjectileMovementComponent> ProjectileComp;
	UPROPERTY(VisibleAnywhere, Category = "LR|VFX") 
	TObjectPtr<class UNiagaraComponent> TrailVFXComponent;
	
	//InitData(GA로부터 받는 데이터)
	UPROPERTY()
	FSkillObjectInitData InitData; 
};
