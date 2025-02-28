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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* _mcameraBoom;

	class UCameraComponent* _mfollowCamera;

public:
	/** Returns CameraBoom subobject */
	FORCEINLINE USpringArmComponent* GetCameraBoom() const
	{
		return _mcameraBoom;
	}
};
