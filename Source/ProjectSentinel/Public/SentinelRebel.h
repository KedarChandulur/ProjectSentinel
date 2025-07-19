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

	/** Called when the Fire Button is pressed */
	void FireWeapon();

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate */
	float _mBaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	/** Base Look up/down rate, in deg/sec. Other scaling may affect final turn rate */
	float _mBaseLookUpRate;

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

public:
	/** Returns CameraBoom subobject */
	FORCEINLINE USpringArmComponent* GetCameraBoom() const
	{
		return _mCameraBoom;
	}
};
