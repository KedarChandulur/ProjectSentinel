// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "SentinelRebel.h"

// Sets default values
AItem::AItem()
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
	_mPickupWidget->SetVisibility(false);

	// Setup overlap for AreaSphere
	_mAreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	_mAreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
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

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

