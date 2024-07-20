// Copyright 2023 Dan Kestranek.


#include "Characters/Abilities/AuraActor.h"

#include "GameplayEffectTypes.h"
#include "Characters/Abilities/GDAbilitySystemComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

// debug logging utility
#define DBG(x) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Yellow, x);}

// Sets default values
AAuraActor::AAuraActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAuraActor::BeginPlay()
{
	Super::BeginPlay();

	// autosize any decals to match the functional radius of the aura
	UDecalComponent* decal = GetComponentByClass<UDecalComponent>();
	if (decal)
	{
		decal->DecalSize.Y = Radius;
		decal->DecalSize.Z = Radius;
	}
}

// Called every frame
void AAuraActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// only do this on server
	if (GetNetMode() == ENetMode::NM_Client)
		return;
	
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
	//objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	
	TArray<AActor*> actorsToIgnore;
	if (Instigator.IsValid())
		actorsToIgnore.Add(Instigator.Get());

	TArray<AActor*> overlappedActors;
	
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(),
			 GetOwner()->GetActorLocation(),
			 Radius,
			 objectTypes,
			 nullptr,
			 actorsToIgnore,
			 overlappedActors);

	// get an array of actors from last check who currently have buffs on them
	TArray<TWeakObjectPtr<UGDAbilitySystemComponent>> oldASCs;
	for(const auto& pair : EffectHandles)
		if (pair.Key.IsValid())
			oldASCs.Add(pair.Key);
	
	// make sure all actors in the sphere this time get buffs assigned
	for (const AActor* actor : overlappedActors)
	{
		UGDAbilitySystemComponent* Asc = actor->FindComponentByClass<UGDAbilitySystemComponent>();
		if (Asc != nullptr)
		{
			if (EffectHandles.Contains(Asc) == false)
			{
				bool isAlly = false; // TODO based on your game
				if ((isAlly && AffectsAllies) || (!isAlly && AffectsEnemies))
				{
					UGameplayEffect* effect = Effect->GetDefaultObject<UGameplayEffect>();
					FGameplayEffectContextHandle context;
					if (Instigator.IsValid())
						context.AddInstigator(Instigator.Get(), Instigator.Get());
					FActiveGameplayEffectHandle handle = Asc->ApplyGameplayEffectToSelf(effect, 1.0f, context);
					EffectHandles.Add(Asc, handle);
					DBG(L"Enter: " + actor->GetName());
				}
			}
			oldASCs.Remove(Asc);
		}
	}

	// remove old ones
	for (auto old: oldASCs)
	{
		if (old.IsValid() && EffectHandles.Contains(old))
		{
			old->RemoveActiveGameplayEffect(EffectHandles[old]);
			EffectHandles.Remove(old);
			DBG(L"Exit: " + old->GetOwner()->GetName());
		}
	}
}

void AAuraActor::LifeSpanExpired()
{
	RemoveAllEffects();
	Super::LifeSpanExpired();
}

void AAuraActor::RemoveAllEffects()
{
	// only do this on server
	if (GetNetMode() == ENetMode::NM_Client)
		return;
	
	// cancel all outstanding effects
	for(const auto& pair : EffectHandles)
		if (pair.Key.IsValid())
			pair.Key->RemoveActiveGameplayEffect(pair.Value);
	EffectHandles.Empty();
}

void AAuraActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAuraActor, Radius);
}

