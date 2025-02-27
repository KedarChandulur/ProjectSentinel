// Fill out your copyright notice in the Description page of Project Settings.


#include "SentinelRebel.h"

// Sets default values
ASentinelRebel::ASentinelRebel()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASentinelRebel::BeginPlay()
{
	Super::BeginPlay();
	
	constexpr bool canTrust{ true };
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay() has called, What I just said is: %d"), canTrust);

	FString fstring{ TEXT("'This is the FString after UE_LOG'") };
	UE_LOG(LogTemp, Display, TEXT("%s"), *fstring);

	UE_LOG(LogTemp, Warning, TEXT("Name of instance: %s"), *GetName());
}

// Called every frame
void ASentinelRebel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASentinelRebel::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

