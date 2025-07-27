// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SentinelRebel.generated.h"

UCLASS()
class PROJECTSENTINEL_API ASentinelRebel : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASentinelRebel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Called for forwards/backwards input */
	void MoveForward(float value);

	/** Called for side to side input */
	void MoveRight(float value);

	/** Called via input to turn at a given rate.
	* @param rate This is a normalized rate, i.e. 1.0 means 100% desired turn rate
	*/
	void TurnAtRate(float rate);

	/** Called via input to look up/down at a given rate.
	* @param rate This is a normalized rate, i.e 1.0 means 100% desired turn rate
	*/
	void LookUpAtRate(float rate);

	/** Rotate controller based on mouse X movement
	* @param value The input value from mouse movement
	*/
	void Turn(float value);

	/** Rotate controller based on mouse Y movement
	* @param value The input value from mouse movement
	*/
	void LookUp(float value);

	/** Called when the Fire Button is pressed */
	void FireWeapon();

	bool GetBeamEndLocation(const FVector& muzzleSocketLocation, FVector& outBeamLocation);

	/** Set bAiming to true on button press */
	void AimingButtonPressed();

	/** Set bAiming to false on button release */
	void AimingButtonReleased();

	void CameraInterpZoom(float deltaTime);

	/** Set BaseTurnRate and BaseLookUp based on aiming */
	void SetLookRates();

	void CalculateCrosshairSpread(float deltaTime);

	void StartCrosshairBulletFire();

	UFUNCTION() // Since this is a call back function for a timer its needs to be a UFUNCTION
	void FinishCrosshairBulletFire();

	void FireButtonPressed();

	void FireButtonReleased();

	void StartFireTimer();

	UFUNCTION() // Since this is a call back function for a timer its needs to be a UFUNCTION
	void AutoFireReset();

	/** Line trace for items under the crosshairs */
	bool TraceUnderCrosshairs(FHitResult& outHitResult);

	/** Trace for items if OverlappedItemCount > 0 */
	void TraceForItems();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* _mCameraBoom;

	/** Follow camera attached to the end of Camera Boom */
	class UCameraComponent* _mFollowCamera;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float _mBaseTurnRate;

	/** Base Look up/down rate, in deg/sec. Other scaling may affect final turn rate */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float _mBaseLookUpRate;

	/** Turn rate while not aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float _mHipTurnRate;

	/** Loop up rate when not aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float _mHipLookUpRate;

	/** Turn rate when aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float _mAimingTurnRate;

	/** Loop up rate when aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float _mAimingLookUpRate;

	/** Scale factor for mouse look sensitivity. Turn rate when not aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float _mMouseHipTurnRate;

	/** Scale factor for mouse look sensitivity. Look Up rate when not aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float _mMouseHipLookUpRate;

	/** Scale factor for mouse look sensitivity. Turn rate when aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float _mMouseAimingTurnRate;

	/** Scale factor for mouse look sensitivity. Look Up rate when aiming */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float _mMouseAimingLookUpRate;

	/** Randomized gunshot sound cue */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* _mFireSound;

	/** Flash spawned at BarrelSocket */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* _mMuzzleFlash;

	/** Montage for firing the weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* _mHipFireMontage;

	/** Particles spawned on bullet impact */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* _mImpactParticles;

	/** Smoke trail for bullets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* _mBeamParticles;

	/** True when aiming */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool _mbAiming;

	/** Default camera field of view value */
	float _mCameraDefaultFOV;

	/** Field of view value when zoomed in*/
	float _mCameraZoomedFOV;

	/** Current field of view this frame */
	float _mCameraCurrentFOV;

	/** Interp speed for zooming when aiming */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float _mZoomInterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "50.0", UIMin = "0.0", UIMax = "50.0"))
	float _mCrosshairYOffset;

	/** Determines the spread of the crosshairs */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float _mCrosshairSpreadMultiplier;

	/** Velocity component for crosshairs spread */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float _mCrosshairVelocityFactor;

	/** In air component for crosshairs spread */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float _mCrosshairInAirFactor;

	/** Aim component for crosshairs spread */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float _mCrosshairAimFactor;

	/** Shooting component for crosshairs spread */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float _mCrosshairShootingFactor;

	float _mShootTimeDuration;

	bool _mbFiringBullet;

	FTimerHandle _mCrosshairShootTimer;

	/** Left mouse button or right trigger pressed */
	bool _mbFireButtonPressed;

	/** True when we can fire. False when waiting for the timer */
	bool _mbShouldFire;

	/** Rate of automatic gun fire */
	float _mAutomaticFirerate;

	/** Sets a timer between gunshots */
	FTimerHandle _mAutoFireTimer;

	/** True if we should trace every frame for items */
	bool _mbShouldTraceForItems;

	/** Number of overlapped AItems */
	int8 _mOverlappedItemCount;

public:
	/** Returns CameraBoom subobject */
	FORCEINLINE USpringArmComponent* GetCameraBoom() const
	{
		return _mCameraBoom;
	}

	/** Returns FollowCamera subobject */
	FORCEINLINE UCameraComponent* GetFollowCamera() const
	{
		return _mFollowCamera;
	}

	FORCEINLINE bool GetAiming() const
	{
		return _mbAiming;
	}

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;

	FORCEINLINE int8 GetOverlappedItemCount() const
	{
		return _mOverlappedItemCount;
	}

	/** Adds/substracts to/from OverlappedItemCount */
	void IncrementOverlappedItemCount(int8 amount);
};
