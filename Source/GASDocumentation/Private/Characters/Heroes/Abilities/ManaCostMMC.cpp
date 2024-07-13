// Copyright 2023 Dan Kestranek.


#include "Characters/Heroes/Abilities/ManaCostMMC.h"

#include "Characters/Abilities/GDGameplayAbility.h"

float UManaCostMMC::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const UGDGameplayAbility* Ability = Cast<UGDGameplayAbility>(Spec.GetContext().GetAbilityInstance_NotReplicated());

	if (!Ability)
	{
		return 0.0f;
	}

	return -Ability->ManaCost;
}
