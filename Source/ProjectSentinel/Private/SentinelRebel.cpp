// Fill out your copyright notice in the Description page of Project Settings.


#include "SentinelRebel.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DrawDebugHelpers.h"

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

	// Don't rotate when the controller rotates. Let the controller only affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ... at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.2f;
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

void ASentinelRebel::FireWeapon()
{
	if (_mFireSound)
	{
		UGameplayStatics::PlaySound2D(this, _mFireSound);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Fire sound is null."));
	}

	const USkeletalMeshSocket* barrelSocket = GetMesh()->GetSocketByName("BarrelSocket");

	if (barrelSocket)
	{
		const FTransform socketTransform = barrelSocket->GetSocketTransform(GetMesh());

		if (_mMuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), _mMuzzleFlash, socketTransform);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Muzzle flash is null."));
		}

		FHitResult fireHit;
		const FVector start{ socketTransform.GetLocation() };
		const FQuat rotation{ socketTransform.GetRotation() };
		const FVector rotationAxis{ rotation.GetAxisX() };
		const FVector end{ start + rotationAxis * 50'000.0f };

		GetWorld()->LineTraceSingleByChannel(fireHit, start, end, ECollisionChannel::ECC_Visibility);

		if (fireHit.bBlockingHit)
		{
			DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 2.0f);
			DrawDebugPoint(GetWorld(), fireHit.Location, 5.0f, FColor::Red, false, 2.0f);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Barrel socket is null."));
	}

	if (_mHipFireMontage)
	{
		UAnimInstance* animInstance = GetMesh()->GetAnimInstance();

		if (animInstance)
		{
			animInstance->Montage_Play(_mHipFireMontage);
			animInstance->Montage_JumpToSection(FName("StartFire"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Anim instance is null."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Hip fire montage is null."));
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
	PlayerInputComponent->BindAxis("TurnRate", this, &ASentinelRebel::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASentinelRebel::LookUpAtRate);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &ASentinelRebel::FireWeapon);
}