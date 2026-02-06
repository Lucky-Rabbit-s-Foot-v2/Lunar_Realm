// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Tags/LRGameplayTags.h"

namespace LRTags
{
	//어빌리티 태그
	UE_DEFINE_GAMEPLAY_TAG(Ability_Combat_BasicShoot, "Ability.Combat.BasicShoot");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Combat_BasicShoot_Cooldown, "Ability.Combat.BasicShoot.Cooldown");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Skill_Fireball, "Ability.Skill.Fireball");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Skill_Fireball_Cooldown, "Ability.Skill.Fireball.Cooldown");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Movement_Dash, "Ability.Movement.Dash");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Movement_Dash_Cooldown, "Ability.Movement.Dash.Cooldown");
	
	//상태 태그
	UE_DEFINE_GAMEPLAY_TAG(State_Debuff_Burning, "State.Debuff.Burning");
	UE_DEFINE_GAMEPLAY_TAG(State_Dead, "State.Dead");
	UE_DEFINE_GAMEPLAY_TAG(State_Dashing, "State.Dashing");
	
	//데이터 태그
	UE_DEFINE_GAMEPLAY_TAG(Data_Damage, "Data.Damage");
	
	//GC 태그
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Combat_Burn, "GameplayCue.Combat.Burn");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Combat_Fireball_Launch, "GameplayCue.Combat.Fireball.Launch");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Combat_Fireball_Impact, "GameplayCue.Combat.Fireball.Impact");
	
	//GE 태그
	UE_DEFINE_GAMEPLAY_TAG(Event_Montage_HitCheck, "Event.Montage.HitCheck");
	UE_DEFINE_GAMEPLAY_TAG(Event_Combat_Death, "Event.Combat.Death");
	UE_DEFINE_GAMEPLAY_TAG(Event_Combat_TakeDamage, "Event.Combat.TakeDamage");

	// 루트
	UE_DEFINE_GAMEPLAY_TAG(Team_Player, "Player");
	UE_DEFINE_GAMEPLAY_TAG(Team_Enemy, "Enemy");

	// 플레이어 하위
	UE_DEFINE_GAMEPLAY_TAG(Team_Player_Character_Member, "Player.Character.Member");
	UE_DEFINE_GAMEPLAY_TAG(Team_Player_Character_Player, "Player.Character.Player");
	UE_DEFINE_GAMEPLAY_TAG(Team_Player_Structure_Core, "Player.Structure.Core");
	UE_DEFINE_GAMEPLAY_TAG(Team_Player_Structure_Obstacle, "Player.Structure.Obstacle");

	// 적 하위
	UE_DEFINE_GAMEPLAY_TAG(Team_Enemy_Character_Normal, "Enemy.Character.Normal");
	UE_DEFINE_GAMEPLAY_TAG(Team_Enemy_Character_Boss, "Enemy.Character.Boss");
	UE_DEFINE_GAMEPLAY_TAG(Team_Enemy_Structure_Core, "Enemy.Structure.Core");
	UE_DEFINE_GAMEPLAY_TAG(Team_Enemy_Structure_Obstacle, "Enemy.Structure.Obstacle");

	// 입력 태그
	UE_DEFINE_GAMEPLAY_TAG(Input_Summon_1, "Input.Summon.1");
	UE_DEFINE_GAMEPLAY_TAG(Input_Summon_2, "Input.Summon.2");
	UE_DEFINE_GAMEPLAY_TAG(Input_Summon_3, "Input.Summon.3");
	UE_DEFINE_GAMEPLAY_TAG(Input_Summon_4, "Input.Summon.4");
	UE_DEFINE_GAMEPLAY_TAG(Input_Command_Toggle, "Input.Command.Toggle");

	
	
	
	
	
}
