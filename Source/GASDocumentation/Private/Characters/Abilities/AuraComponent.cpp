// Copyright 2023 Dan Kestranek.


#include "Characters/Abilities/AuraComponent.h"

#include "CollisionDebugDrawingPublic.h"
#include "Characters/Abilities/GDAbilitySystemComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameplayCueNotify_Actor.h"

// debug logging utility
#define DBG(x) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Yellow, x);}

// Sets default values for this component's properties
UAuraComponent::UAuraComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAuraComponent::BeginPlay()
{
	Super::BeginPlay();

	// autosize any decals to match the functional radius of the aura
	UDecalComponent* decal = GetOwner()->GetComponentByClass<UDecalComponent>();
	if (decal)
	{
		decal->DecalSize.Y = Radius;
		decal->DecalSize.Z = Radius;
	}

	// ...
}


// Called every frame
void UAuraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (IsActive == false)
		return;

	// only do this on server
	if (GetNetMode() == ENetMode::NM_Client)
		return;
	
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
	//objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	
	TArray<AActor*> actorsToIgnore;
	if (instigator.IsValid())
		actorsToIgnore.Add(instigator.Get());

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
					if (instigator.IsValid())
						context.AddInstigator(instigator.Get(), instigator.Get());
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

void UAuraComponent::OnActivate()
{
	EffectHandles.Empty();
	IsActive = true;

	if (AGameplayCueNotify_Actor* owner = Cast<AGameplayCueNotify_Actor>(GetOwner()))
		instigator = owner->CueInstigator;
}

void UAuraComponent::OnDestroy()
{
	for(const auto& pair : EffectHandles)
		if (pair.Key.IsValid())
			pair.Key->RemoveActiveGameplayEffect(pair.Value);
	EffectHandles.Empty();
	IsActive = false;
}

