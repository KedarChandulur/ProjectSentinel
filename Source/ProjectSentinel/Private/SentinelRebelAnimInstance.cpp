// Fill out your copyright notice in the Description page of Project Settings.


#include "SentinelRebelAnimInstance.h"
#include "SentinelRebel.h"
#include "GameFramework/CharacterMovementComponent.h"

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
	}
}

void USentinelRebelAnimInstance::NativeInitializeAnimation()
{
	sentinelRebel = Cast<ASentinelRebel>(TryGetPawnOwner());
}