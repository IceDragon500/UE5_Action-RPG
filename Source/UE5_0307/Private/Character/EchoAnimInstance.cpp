// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/EchoAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/EchoCharacter.h"


void UEchoAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	EchoChar = Cast<AEchoCharacter>(TryGetPawnOwner());
	if (EchoChar)
	{
		CharacterMovement = EchoChar->GetCharacterMovement();
	}
}

void UEchoAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (CharacterMovement)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(CharacterMovement->Velocity);
		isFalling = CharacterMovement->IsFalling();
		CharacterState = EchoChar->GetCharacterState();
		DeathPose = EchoChar->GetDeathPose();
	}
}
