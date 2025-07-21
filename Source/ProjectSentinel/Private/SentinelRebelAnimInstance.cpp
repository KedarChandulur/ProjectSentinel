// Fill out your copyright notice in the Description page of Project Settings.


#include "SentinelRebelAnimInstance.h"
#include "SentinelRebel.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void USentinelRebelAnimInstance::UpdateAnimationProperties(float deltaTime)
{
	if (sentinelRebel == nullptr)
	{
		sentinelRebel = Cast<ASentinelRebel>(TryGetPawnOwner());
	}

	if (sentinelRebel)
	{
		FVector velocity{ sentinelRebel->GetVelocity() };
		velocity.Z = 0;
		speed = velocity.Size();

		isInAir = sentinelRebel->GetCharacterMovement()->IsFalling();

		if (sentinelRebel->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f)
		{
			isAccelerating = true;
		}
		else
		{
			isAccelerating = false;
		}

		FRotator aimRotation = sentinelRebel->GetBaseAimRotation();
		FRotator movementRotation = UKismetMathLibrary::MakeRotFromX(sentinelRebel->GetVelocity());
		movementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(movementRotation, aimRotation).Yaw;

		//FString rotationMessage = FString::Printf(TEXT("Base Aim Rotation: %f"), aimRotation.Yaw);
		//FString movementRotationMessage = FString::Printf(TEXT("Movement Rotation: %f"), movementRotation.Yaw);
		FString offsetMessage = FString::Printf(TEXT("Movement Offset Yaw: %f"), movementOffsetYaw);

		if (GEngine)
		{
			//GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::White, rotationMessage);
			//GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::White, movementRotationMessage);
			GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::White, offsetMessage);
		}
	}
}

void USentinelRebelAnimInstance::NativeInitializeAnimation()
{
	sentinelRebel = Cast<ASentinelRebel>(TryGetPawnOwner());
}