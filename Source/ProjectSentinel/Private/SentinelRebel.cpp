// Fill out your copyright notice in the Description page of Project Settings.


#include "SentinelRebel.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ASentinelRebel::ASentinelRebel()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a camera boom (pulls in towards the character if there is a collision)
	_mcameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	_mcameraBoom->SetupAttachment(RootComponent);
	_mcameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character
	_mcameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	_mfollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	_mfollowCamera->SetupAttachment(_mcameraBoom, USpringArmComponent::SocketName); // Attach camera to the end of boom
	_mfollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

// Called when the game starts or when spawned
void ASentinelRebel::BeginPlay()
{
	Super::BeginPlay();

}

void ASentinelRebel::MoveForward(float value)
{
	if ((Controller != nullptr) && value != 0.0f)
	{
		const FRotator controllerRotation{ Controller->GetControlRotation() };
		//const FRotator yawRotation{ 0, controllerRotation.Yaw, 0 };
		const FRotator yawRotation{ 0.0f, controllerRotation.Yaw, 0.0f };

		const FVector directionX{ FRotationMatrix{yawRotation}.GetUnitAxis(EAxis::X) };

		AddMovementInput(directionX, value);
	}
}

void ASentinelRebel::MoveRight(float value)
{
	if ((Controller != nullptr) && value != 0.0f)
	{
		const FRotator controllerRotation{ Controller->GetControlRotation() };
		//const FRotator yawRotation{ 0, controllerRotation.Yaw, 0 };
		const FRotator yawRotation{ 0.0f, controllerRotation.Yaw, 0.0f };

		const FVector directionY{ FRotationMatrix{yawRotation}.GetUnitAxis(EAxis::Y) };

		AddMovementInput(directionY, value);
	}
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
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASentinelRebel::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASentinelRebel::MoveRight);
}