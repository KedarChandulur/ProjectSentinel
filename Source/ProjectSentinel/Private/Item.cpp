// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "SentinelRebel.h"

// Sets default values
AItem::AItem()
	: _mItemName(FString("Default")),
	  _mItemCount(0),
	  _mItemRarity(EItemRarity::EIR_Common),
	  _mItemState(EItemState::EIS_PickUp)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_mItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(_mItemMesh);

	_mCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	_mCollisionBox->SetupAttachment(_mItemMesh);
	_mCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	_mCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	_mPickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	_mPickupWidget->SetupAttachment(GetRootComponent());

	_mAreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Area Sphere"));
	_mAreaSphere->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	// Hide Pickup Widget
	if (_mPickupWidget)
	{
		_mPickupWidget->SetVisibility(false);
	}

	// Sets ActiveStars array based on Item Rarity
	SetActiveStars();

	// Setup overlap for AreaSphere
	_mAreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	_mAreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	// Set Item properties based on ItemState
	SetItemProperties(_mItemState);
}

void AItem::OnSphereOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	if (otherActor)
	{
		ASentinelRebel* sentinelRebel = Cast<ASentinelRebel>(otherActor);
		
		if (sentinelRebel)
		{
			sentinelRebel->IncrementOverlappedItemCount(1);
		}
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex)
{
	if (otherActor)
	{
		ASentinelRebel* sentinelRebel = Cast<ASentinelRebel>(otherActor);

		if (sentinelRebel)
		{
			sentinelRebel->IncrementOverlappedItemCount(-1);
		}
	}
}

void AItem::SetActiveStars()
{
	for (uint8 i = 0; i < 5; ++i)
	{
		_mActiveStars.Add(false);
	}

	uint8 rarityIncrementer = 0;

	switch (_mItemRarity)
	{
	case EItemRarity::EIR_Damaged:
		_mActiveStars[rarityIncrementer++] = true;
		break;
	case EItemRarity::EIR_Common:
		_mActiveStars[rarityIncrementer++] = true;
		_mActiveStars[rarityIncrementer++] = true;
		break;
	case EItemRarity::EIR_Uncommon:
		_mActiveStars[rarityIncrementer++] = true;
		_mActiveStars[rarityIncrementer++] = true;
		_mActiveStars[rarityIncrementer++] = true;
		break;
	case EItemRarity::EIR_Rare:
		_mActiveStars[rarityIncrementer++] = true;
		_mActiveStars[rarityIncrementer++] = true;
		_mActiveStars[rarityIncrementer++] = true;
		_mActiveStars[rarityIncrementer++] = true;
		break;
	case EItemRarity::EIR_Legendary:
		_mActiveStars[rarityIncrementer++] = true;
		_mActiveStars[rarityIncrementer++] = true;
		_mActiveStars[rarityIncrementer++] = true;
		_mActiveStars[rarityIncrementer++] = true;
		_mActiveStars[rarityIncrementer++] = true;
		break;
	default:
		UE_LOG(LogTemp, Error, TEXT("Default has been hit for rarity... did you setup your enum correctly?"));
		break;
	}
}

void AItem::SetItemProperties(EItemState state)
{
	switch (state)
	{
	case EItemState::EIS_PickUp:
		// Set mesh properties
		_mItemMesh->SetSimulatePhysics(false);
		_mItemMesh->SetEnableGravity(false);
		_mItemMesh->SetVisibility(true);
		_mItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		_mItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Set area sphere properties
		_mAreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		_mAreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		// Set collision box properties
		_mCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		_mCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
		_mCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;

	case EItemState::EIS_Equipped:
		// Set mesh properties
		_mItemMesh->SetSimulatePhysics(false);
		_mItemMesh->SetEnableGravity(false);
		_mItemMesh->SetVisibility(true);
		_mItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		_mItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Set area sphere properties
		_mAreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		_mAreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Set collision box properties
		_mCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		_mCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EItemState::EIS_Falling:
		// Set mesh properties
		_mItemMesh->SetSimulatePhysics(true);
		_mItemMesh->SetEnableGravity(true);
		_mItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		_mItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		_mItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

		// Set area sphere properties
		_mAreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		_mAreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Set collision box properties
		_mCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		_mCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	default:
		UE_LOG(LogTemp, Error, TEXT("Default has been hit for item properties... did you setup your enum correctly?"));
		break;
	}
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItem::SetItemState(EItemState state)
{
	_mItemState = state;

	SetItemProperties(state);
}