// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/MHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "MGameModeBase.h"
#include "MShooterCharacter.h"
#include "CyberInvasion/CyberInvasion.h"

// Sets default values for this component's properties
UMHealthComponent::UMHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	defaultHealth = 100;
	health = defaultHealth;
	bIsDead = false;
	teamNum = 255;

	//这里本质上需要设置为true，但是调用这个函数时会自动断点，所以在蓝图中设置true
	//SetIsReplicated(true);
}

// Called when the game starts
void UMHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority) {
		AActor* owner = GetOwner();
		if (owner) {
			owner->OnTakeAnyDamage.AddDynamic(this, &UMHealthComponent::handleTakeAnyDamage);
		}
	}
}

// Called every frame
void UMHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

float UMHealthComponent::getHealth()
{
	return health;
}

float UMHealthComponent::getHealthPercentage()
{
	if (defaultHealth <= 0)
		return 0.0f;

	return health / defaultHealth;
}

//这个函数只会在server端调用
void UMHealthComponent::handleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	//UE_LOG(LogTemp, Warning, TEXT("maohaoran handleTakeAnyDamage 62: %f"), Damage);
	if (Damage < 0.0f || bIsDead)
		return;

	if (bnGameModeType == Enum_Multi_GameMode) {
		//多人模式下，只有AI才有友军判断，玩家角色不做判断
		AMShooterCharacter* player = Cast< AMShooterCharacter>(GetOwner());
		if (!player && DamagedActor != DamageCauser && isFriendly(DamagedActor, DamageCauser)) {
			//UE_LOG(LogTemp, Warning, TEXT("maohaoran handleTakeAnyDamage 70: %f"), Damage);
			return;
		}
	}
	else {
		if (DamagedActor != DamageCauser && isFriendly(DamagedActor, DamageCauser)) {
			//UE_LOG(LogTemp, Warning, TEXT("maohaoran handleTakeAnyDamage 76: %f"), Damage);
			return;
		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("maohaoran handleTakeAnyDamage 79: %f"), Damage);
	health = FMath::Clamp(health - Damage, 0.0f, defaultHealth);

	bIsDead = health <= 0 ? true : false;

	switch (bnGameModeType) {
		case Enum_Single_GameMode:
		{
			onHealthChanged.Broadcast(this, health, Damage, DamageType, InstigatedBy, DamageCauser);
			if (bIsDead) {
				AMGameModeBase* gm = Cast< AMGameModeBase>(GetWorld()->GetAuthGameMode());
				if (gm) {
					gm->onActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
				}
			}
		}
			break;
		case Enum_Multi_GameMode:
		{
			if (bIsDead) {
				AMGameModeBase* gm = Cast< AMGameModeBase>(GetWorld()->GetAuthGameMode());
				if (gm) {
					AMShooterCharacter* player = Cast< AMShooterCharacter>(GetOwner());
					APlayerController* playerCtrl = nullptr;
					if (player)
						playerCtrl = Cast< APlayerController>(player->GetController());

					//先取controller的值，不然AMShooterCharacter会被销毁
					onHealthChanged.Broadcast(this, health, Damage, DamageType, InstigatedBy, DamageCauser);

					gm->onActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
					if (playerCtrl) {	//若为多人模式，需要重生玩家角色
						gm->rebornPlayer(playerCtrl);
					}

				}
			}
			else
				onHealthChanged.Broadcast(this, health, Damage, DamageType, InstigatedBy, DamageCauser);
		}
			break;
		default:
			break;
	}
		
	return;
}

void UMHealthComponent::OnRep_Health(float oldHealth)
{
	float damage = 0;

	//if(GetOwnerRole()!= ROLE_Authority)
		//UE_LOG(LogTemp, Warning, TEXT("maohaoran 61: %d"), GetOwnerRole());

	//UE_LOG(LogTemp, Warning, TEXT("maohaoran UMHealthComponent::OnRep_Health() current health: %s ,role: %d"), *FString::SanitizeFloat(health), GetOwnerRole());

	onHealthChanged.Broadcast(this, health, damage, nullptr, nullptr, nullptr);
}

void UMHealthComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UMHealthComponent, health);
}

void UMHealthComponent::heal(float healAmount)
{
	//UE_LOG(LogTemp, Warning, TEXT("maohaoran heal() health: %f , healAmount:%f ,role: %d"), health, healAmount, GetOwnerRole());

	if (healAmount <= 0.0f || health <= 0.0f || health >= defaultHealth)
		return;

	health = FMath::Clamp(health + healAmount, 0.0f, defaultHealth);
	onHealthChanged.Broadcast(this, health, -healAmount, nullptr, nullptr, nullptr);
}

bool UMHealthComponent::isFriendly(AActor* actorA, AActor* actorB)
{
	if (!actorA || !actorB) {
		return false;
	}

	UMHealthComponent* healCompA = Cast< UMHealthComponent>(actorA->GetComponentByClass(UMHealthComponent::StaticClass()));
	UMHealthComponent* healCompB = Cast< UMHealthComponent>(actorB->GetComponentByClass(UMHealthComponent::StaticClass()));

	if (!healCompA || !healCompB) {
		return false;
	}


	return healCompA->teamNum == healCompB->teamNum;

}