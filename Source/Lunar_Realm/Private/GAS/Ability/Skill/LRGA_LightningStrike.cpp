// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/Skill/LRGA_LightningStrike.h"

#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/GameInstance.h"
#include "GAS/Tags/LRGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Units/LRCharacter.h"

ULRGA_LightningStrike::ULRGA_LightningStrike()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = LRTags::Ability_Skill_Lightning;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);
	
	//DT 참조키
	SkillID = "SKILL_LIGHTNING";
	SkillEffectID = "EFFECT_LIGHTNING";
}

void ULRGA_LightningStrike::OnAbilityActivated(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	LR_INFO(TEXT("[ULRGA_LightningStrike] OnAbilityActivated 진입!"));
	
	// 유효성 검사
	if (!CachedInstigator)
	{
		LR_WARN(TEXT("필수 데이터 누락"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	// DT에서 스킬 데이터 읽기
	UGameInstance* GI = GetWorld()->GetGameInstance();
	check(GI);
	UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>();
	if (!DataSys)
	{
		LR_WARN(TEXT("말도 안돼 데이터시스템이 없다니!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const FSkillEffectData& EffectData = DataSys->GetSkillEffectData(SkillEffectID);
	const FSkillHitAreaData& HitAreaData = DataSys->GetSkillHitAreaData(SkillEffectID);
	const FSkillStaticData& SkillData = DataSys->GetSkillStaticData(SkillID);
	const FSkillResourceData& SkillResourceData = DataSys->GetSkillResourceData(SkillData.ResourceID);
	
	//적대 팀 태그 결정
	FGameplayTag HostileTag = GetHostileTeamTag();
	if (!HostileTag.IsValid())
	{
		LR_WARN(TEXT("HostileTag 없음"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	//가장 가까운 적 탐색(범위는 임시로 HitRadius 3배)
	float LockRange = HitAreaData.HitRadius * 3.f;
	AActor* NearestHostile = FindNearestHostile(HostileTag, LockRange);
	if (!NearestHostile)
	{
		LR_INFO(TEXT("범위 내 적 없음"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	FVector StrikeLoc = NearestHostile->GetActorLocation();
	
	//FX
	if (UNiagaraSystem* StrikeVFX = SkillResourceData.SpawnVFX.LoadSynchronous())
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), StrikeVFX, StrikeLoc);
	}

	if (USoundBase* StrikeSFX = SkillResourceData.SpawnSFX.LoadSynchronous())
	{
		UGameplayStatics::PlaySoundAtLocation(this, StrikeSFX, StrikeLoc);
	}
	
	//데미지/상태이상 적용
	ApplyLightningDamage(StrikeLoc, HostileTag, EffectData.Amount);
	
	// ImpactVFX/SFX 재생
	if (UNiagaraSystem* ImpactVFX = SkillResourceData.ImpactVFX.LoadSynchronous())
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactVFX, StrikeLoc);
	}
	if (USoundBase* ImpactSFX = SkillResourceData.ImpactSFX.LoadSynchronous())
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSFX, StrikeLoc);
	}
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}


void ULRGA_LightningStrike::ApplyLightningDamage(FVector StrikeLocation, FGameplayTag HostileTag, float Damage)
{
	// DT에서 스킬 데이터 읽기
	UGameInstance* GI = GetWorld()->GetGameInstance();
	check(GI);
	UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>();
	if (!DataSys)
	{
		LR_WARN(TEXT("말도 안돼 데이터시스템이 없다니!"));
		return;
	}
	
	const FSkillHitAreaData& HitAreaData = DataSys->GetSkillHitAreaData(SkillEffectID);
	
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    TArray<AActor*> IgnoreActors;
    IgnoreActors.Add(const_cast<ALRCharacter*>(CachedInstigator.Get()));

	// 감지 범위 디버그 구체 그리기 (에디터 빌드에서만)
#if WITH_EDITOR
	DrawDebugSphere(
		GetWorld(), StrikeLocation, HitAreaData.HitRadius,  
		16, FColor::Red, false, 2.0f );
#endif
	
    TArray<AActor*> OutActors;
    UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(), StrikeLocation, HitAreaData.HitRadius,
        ObjectTypes, AActor::StaticClass(),
        IgnoreActors,OutActors);

    // MaxTargetCount 체크 (0 = 제한 없음)
    int32 HitCount = 0;

    for (AActor* Target : OutActors)
    {
    	//MaxTargetCount가 존재할때만 카운트 진행
        if (HitAreaData.MaxTargetCount > 0 && HitCount >= HitAreaData.MaxTargetCount)
        {
            break;
        }

    	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);

    	//GAS 오브젝트가 아니거나 적대 태그가 아니면 패스
    	if (!TargetASC)
    	{
    		LR_WARN(TEXT("유효하지 않은 ASC"));
    		continue;
    	}
    	if (!TargetASC->HasMatchingGameplayTag(HostileTag))
    	{
    		continue;
    	}

        UAbilitySystemComponent* SourceASC = GetOwnerASC();
        FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
        FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.f, Context);

        if (SpecHandle.IsValid())
        {
            SpecHandle.Data->SetByCallerTagMagnitudes.Add(LRTags::Data_Damage, -Damage);
            SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
        }

        // 상태이상 적용(GE_SHOCK)
        if (StatusEffectClass)
        {
            FGameplayEffectSpecHandle StatusSpec = SourceASC->MakeOutgoingSpec(StatusEffectClass, 1.f, Context);
            if (StatusSpec.IsValid())
            {
	            SourceASC->ApplyGameplayEffectSpecToTarget(	*StatusSpec.Data.Get(), TargetASC);
            }
        }

        HitCount++;
    }
}
