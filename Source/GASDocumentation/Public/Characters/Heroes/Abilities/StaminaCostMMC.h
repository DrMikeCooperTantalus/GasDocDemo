// Copyright 2023 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "StaminaCostMMC.generated.h"

/**
 * 
 */
UCLASS()
class GASDOCUMENTATION_API UStaminaCostMMC : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
public:
	
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
