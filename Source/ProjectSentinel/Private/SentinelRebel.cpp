// Fill out your copyright notice in the Description page of Project Settings.


#include "SentinelRebel.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
//#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ASentinelRebel::ASentinelRebel()
	: // Base rates for turning/looking up
	  _mBaseTurnRate(45.0f), 
	  _mBaseLookUpRate(45.0f),
	  // Turn rates for aiming/not aiming
	  _mHipTurnRate(90.0f),
	  _mHipLookUpRate(90.0f),
	  _mAimingTurnRate(20.0f),
	  _mAimingLookUpRate(20.0f),
	  // Mouse look sensitivity scale factors
	  _mMouseHipTurnRate(1.0f),
	  _mMouseHipLookUpRate(1.0f),
	  _mMouseAimingTurnRate(0.5f),
	  _mMouseAimingLookUpRate(0.5f),
	  // true when aiming the weapon
	  _mbAiming(false), 
	  // Camera field of view values
	  _mCameraDefaultFOV(0.0f), // setting this in BeginPlay
	  _mCameraZoomedFOV(35.0f),
	  _mCameraCurrentFOV(0.0f),
	  _mZoomInterpSpeed(20.0f),
	  // Crosshair spread factors
	  _mCrosshairSpreadMultiplier(0.0f),
	  _mCrosshairVelocityFactor(0.0f),
	  _mCrosshairInAirFactor(0.0f),
	  _mCrosshairAimFactor(0.0f),
	  _mCrosshairShootingFactor(0.0f),
	  // Bullet fire timer variables
	  _mShootTimeDuration(0.05f),
	  _mbFiringBullet(false)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a camera boom (pulls in towards the character if there is a collision)
	_mCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	_mCameraBoom->SetupAttachment(RootComponent);
	_mCameraBoom->TargetArmLength = 180.0f; // The camera follows at this distance behind the character
	_mCameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	_mCameraBoom->SocketOffset = FVector(0.0f, 50.0f, 70.0f);

	_mFollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	_mFollowCamera->SetupAttachment(_mCameraBoom, USpringArmComponent::SocketName); // Attach camera to the end of boom
	_mFollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Don't rotate when the controller rotates. Let the controller only affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ... at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.2f;
}

// Called when the game starts or when spawned
void ASentinelRebel::BeginPlay()
{
	Super::BeginPlay();

	if (_mFollowCamera)
	{
		_mCameraDefaultFOV = GetFollowCamera()->FieldOfView;
		_mCameraCurrentFOV = _mCameraDefaultFOV;
	}
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

void ASentinelRebel::Turn(float value)
{
	float turnScaleFactor{};

	if (_mbAiming)
	{
		turnScaleFactor = _mMouseAimingTurnRate;
	}
	else
	{
		turnScaleFactor = _mMouseHipTurnRate;
	}

	AddControllerYawInput(value * turnScaleFactor);
}

void ASentinelRebel::LookUp(float value)
{
	float lookUpScaleFactor{};

	if (_mbAiming)
	{
		lookUpScaleFactor = _mMouseAimingLookUpRate;
	}
	else
	{
		lookUpScaleFactor = _mMouseHipLookUpRate;
	}

	AddControllerPitchInput(value * lookUpScaleFactor);
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

		FVector beamEnd;
		bool bBeamEnd = GetBeamEndLocation(socketTransform.GetLocation(), beamEnd);

		if (bBeamEnd)
		{
			// Spawn impact particles after updating BeamEndPoint
			if (_mImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), _mImpactParticles, beamEnd);
			}

			if (_mBeamParticles)
			{
				UParticleSystemComponent* beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), _mBeamParticles, socketTransform);

				if (beam)
				{
					beam->SetVectorParameter(FName("Target"), beamEnd);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Beam is invalid."));
				}
			}
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

	// Start bullet fire timer for crosshairs
	StartCrosshairBulletFire();
}

bool ASentinelRebel::GetBeamEndLocation(const FVector& muzzleSocketLocation, FVector& outBeamLocation)
{
	// Get current size of the viewport
	FVector2D viewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(viewportSize);
	}

	// Get screen space location of crosshairs
	FVector2D crosshairLocation(viewportSize.X / 2.0f, viewportSize.Y / 2.0f);
	crosshairLocation.Y -= 50.0f;

	FVector crosshairWorldPosition;
	FVector crosshairWorldDirection;

	// Get world position and direction of crosshairs
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld
	(
		UGameplayStatics::GetPlayerController(this, 0),
		crosshairLocation,
		crosshairWorldPosition,
		crosshairWorldDirection
	);

	if (bScreenToWorld) // was Deprojection successful?
	{
		FHitResult screenTraceHit;
		const FVector start{ crosshairWorldPosition };
		const FVector end{ crosshairWorldPosition + crosshairWorldDirection * 50'000.0f };

		// Set beam end point to line trace end point
		outBeamLocation = end;

		// Trace outward from crosshairs world location
		GetWorld()->LineTraceSingleByChannel(screenTraceHit, start, end, ECollisionChannel::ECC_Visibility);

		if (screenTraceHit.bBlockingHit) // was there a trace hit?
		{
			// Beam end point is now trace hit location
			outBeamLocation = screenTraceHit.Location;
		}

		// Perform a second trace, this time from gun barrel
		FHitResult weaponTraceHit;
		const FVector weaponTraceStart{ muzzleSocketLocation };
		const FVector weaponTraceEnd{ outBeamLocation };

		GetWorld()->LineTraceSingleByChannel(weaponTraceHit, weaponTraceStart, weaponTraceEnd, ECollisionChannel::ECC_Visibility);

		if (weaponTraceHit.bBlockingHit) // object between barrel and BeamEndPoint?
		{
			outBeamLocation = weaponTraceHit.Location;
		}

		return true;
	}

	return false;
}

void ASentinelRebel::AimingButtonPressed()
{
	_mbAiming = true;
}

void ASentinelRebel::AimingButtonReleased()
{
	_mbAiming = false;
}

void ASentinelRebel::CameraInterpZoom(float deltaTime)
{
	// Set current camera field of view
	if (_mbAiming)
	{
		// Interpolate to zoomed FOV
		_mCameraCurrentFOV = FMath::FInterpTo(_mCameraCurrentFOV, _mCameraZoomedFOV, deltaTime, _mZoomInterpSpeed);
	}
	else
	{
		// Interpolate to default FOV
		_mCameraCurrentFOV = FMath::FInterpTo(_mCameraCurrentFOV, _mCameraDefaultFOV, deltaTime, _mZoomInterpSpeed);
	}

	GetFollowCamera()->SetFieldOfView(_mCameraCurrentFOV);
}

void ASentinelRebel::SetLookRates()
{
	if (_mbAiming)
	{
		_mBaseTurnRate = _mAimingTurnRate;
		_mBaseLookUpRate = _mAimingLookUpRate;
	}
	else
	{
		_mBaseTurnRate = _mHipTurnRate;
		_mBaseLookUpRate = _mHipLookUpRate;
	}
}

void ASentinelRebel::CalculateCrosshairSpread(float deltaTime)
{
	FVector2D walkSpeedRange{ 0.0f, 600.0f };
	FVector2D velocityMultiplierRange{ 0.0f, 1.0f };

	FVector velocity{ GetVelocity() };
	velocity.Z = 0.0f;

	// Calculate crosshair velocity factor
	_mCrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(walkSpeedRange, velocityMultiplierRange, velocity.Size());

	// Calculate crosshair in air factor
	if (GetCharacterMovement()->IsFalling()) // is in air?
	{
		// Spread the crosshairs slowly while in air
		_mCrosshairInAirFactor = FMath::FInterpTo(_mCrosshairInAirFactor, 2.25f, deltaTime, 2.25f);
	}
	else // Character is on the ground
	{
		// Shrink the crosshairs rapidly while on the ground
		_mCrosshairInAirFactor = FMath::FInterpTo(_mCrosshairInAirFactor, 0.0f, deltaTime, 30.0f);
	}

	// Calculate crosshair aim factor
	if (_mbAiming) // are we aiming?
	{
		// Shrink crosshairs a small amount very quickly
		_mCrosshairAimFactor = FMath::FInterpTo(_mCrosshairAimFactor, 0.4f, deltaTime, 30.0f);
	}
	else // Not aiming
	{
		// Spread crosshairs back to normal very quickly
		_mCrosshairAimFactor = FMath::FInterpTo(_mCrosshairAimFactor, 0.0f, deltaTime, 30.0f);
	}

	// True 0.05 seconds after firing
	if (_mbFiringBullet)
	{
		_mCrosshairShootingFactor = FMath::FInterpTo(_mCrosshairShootingFactor, 0.3f, deltaTime, 60.0f);
	}
	else
	{
		_mCrosshairShootingFactor = FMath::FInterpTo(_mCrosshairShootingFactor, 0.0f, deltaTime, 60.0f);
	}

	_mCrosshairSpreadMultiplier = 0.5f + _mCrosshairVelocityFactor + _mCrosshairInAirFactor - _mCrosshairAimFactor + _mCrosshairShootingFactor;
}

void ASentinelRebel::StartCrosshairBulletFire()
{
	_mbFiringBullet = true;

	GetWorldTimerManager().SetTimer(_mCrosshairShootTimer,this, &ASentinelRebel::FinishCrosshairBulletFire, _mShootTimeDuration);
}

void ASentinelRebel::FinishCrosshairBulletFire()
{
	_mbFiringBullet = false;
}

// Called every frame
void ASentinelRebel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handle interpolation for zoom when aiming
	CameraInterpZoom(DeltaTime);

	//Change look sensitivity based on aiming
	SetLookRates();

	// Calculate crosshair spread multiplier
	CalculateCrosshairSpread(DeltaTime);
}

// Called to bind functionality to input
void ASentinelRebel::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	// Binding Axis(s)

	PlayerInputComponent->BindAxis("MoveForward", this, &ASentinelRebel::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASentinelRebel::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASentinelRebel::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASentinelRebel::LookUpAtRate);

	PlayerInputComponent->BindAxis("Turn", this, &ASentinelRebel::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ASentinelRebel::LookUp);

	// Binding Action(s)

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &ASentinelRebel::FireWeapon);

	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, &ASentinelRebel::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this, &ASentinelRebel::AimingButtonReleased);
}

float ASentinelRebel::GetCrosshairSpreadMultiplier() const
{
	return _mCrosshairSpreadMultiplier;
}