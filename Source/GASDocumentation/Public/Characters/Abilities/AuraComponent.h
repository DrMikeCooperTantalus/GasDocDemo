// Copyright 2023 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActiveGameplayEffectHandle.h"
#include "AuraComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GASDOCUMENTATION_API UAuraComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAuraComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	TMap<TWeakObjectPtr<class UGDAbilitySystemComponent>, FActiveGameplayEffectHandle> EffectHandles;
	bool IsActive = false;

	TWeakObjectPtr<AActor> instigator;
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Radius;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<class UGameplayEffect> Effect;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool AffectsEnemies;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool AffectsAllies;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool AffectsSelf;

	
	UFUNCTION(BlueprintCallable)
	void OnActivate();
	UFUNCTION(BlueprintCallable)
	void OnDestroy();
};
