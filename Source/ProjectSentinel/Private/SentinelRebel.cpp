// Fill out your copyright notice in the Description page of Project Settings.


#include "SentinelRebel.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ASentinelRebel::ASentinelRebel()
	: _mBaseTurnRate(45.0f), _mBaseLookUpRate(45.0f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a camera boom (pulls in towards the character if there is a collision)
	_mCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	_mCameraBoom->SetupAttachment(RootComponent);
	_mCameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character
	_mCameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	_mFollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	_mFollowCamera->SetupAttachment(_mCameraBoom, USpringArmComponent::SocketName); // Attach camera to the end of boom
	_mFollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
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

void ASentinelRebel::TurnAtRate(float rate)
{
	// Calculate delta for this frame from the rate information
	AddControllerYawInput(rate * _mBaseTurnRate * GetWorld()->GetDeltaSeconds()); // deg/sec * sec/frame
}

void ASentinelRebel::LookUpAtRate(float rate)
{
	// Calculate delta for this frame from the rate information
	AddControllerPitchInput(rate * _mBaseLookUpRate * GetWorld()->GetDeltaSeconds()); // deg/sec * sec/frame
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
	PlayerInputComponent->BindAxis("TurnRate", this, &ASentinelRebel::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASentinelRebel::LookUpAtRate);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}