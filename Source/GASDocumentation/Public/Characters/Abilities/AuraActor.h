// Copyright 2023 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActiveGameplayEffectHandle.h"
#include "AuraActor.generated.h"

UCLASS()
class GASDOCUMENTATION_API AAuraActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAuraActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TMap<TWeakObjectPtr<class UGDAbilitySystemComponent>, FActiveGameplayEffectHandle> EffectHandles;
	TWeakObjectPtr<AActor> Instigator;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, meta = (ExposeOnSpawn = true))
	float Radius;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true))
	TSubclassOf<class UGameplayEffect> Effect;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true))
	bool AffectsEnemies;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true))
	bool AffectsAllies;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true))
	bool AffectsSelf;

	virtual void LifeSpanExpired() override;
	void RemoveAllEffects();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
};
