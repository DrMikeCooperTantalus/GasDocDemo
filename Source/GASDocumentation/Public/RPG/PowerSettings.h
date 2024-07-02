// Copyright 2023 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PowerSettings.generated.h"

/**
 * 
 */
UCLASS()
class GASDOCUMENTATION_API UPowerSettings : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Damage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TSubclassOf<class UGameplayEffect>> Effects;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class  UAnimMontage* FireHipMontage;
};
