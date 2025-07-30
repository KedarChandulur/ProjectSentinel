// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

AWeapon::AWeapon()
	:_mThrowWeaponTime(0.7f),
	 _mbFalling(false)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWeapon::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	// Keep the Weapon upright
	if (GetItemState() == EItemState::EIS_Falling && _mbFalling)
	{
		const FRotator meshRotation{ 0.0f, GetItemMesh()->GetComponentRotation().Yaw, 0.0f };
		GetItemMesh()->SetWorldRotation(meshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void AWeapon::StopFalling()
{
	_mbFalling = false;
	SetItemState(EItemState::EIS_PickUp);
}

void AWeapon::ThrowWeapon()
{
	FRotator meshRotation{ 0.0f, GetItemMesh()->GetComponentRotation().Yaw, 0.0f };
	GetItemMesh()->SetWorldRotation(meshRotation, false, nullptr, ETeleportType::TeleportPhysics);

	const FVector meshForward{ GetItemMesh()->GetForwardVector() };
	const FVector meshRight{ GetItemMesh()->GetRightVector() };

	// Direction in which we throw the weapon
	FVector impulseDirection = meshRight.RotateAngleAxis(-20.0f, meshForward);

	float randomRotation{ FMath::FRandRange(10.0f, 30.0f) };
	impulseDirection = impulseDirection.RotateAngleAxis(randomRotation, FVector(0.0f, 0.0f, 1.0f));
	impulseDirection *= 10'000.0f;

	GetItemMesh()->AddImpulse(impulseDirection);

	_mbFalling = true;
	GetWorldTimerManager().SetTimer(_mThrowWeaponTimer, this, &AWeapon::StopFalling, _mThrowWeaponTime);
}