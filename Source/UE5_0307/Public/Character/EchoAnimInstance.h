// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Character/EchoCharacter.h"
#include "CharacterType.h"
#include "EchoAnimInstance.generated.h"

class AEchoCharacter;
class UCharacterMovementComponent;


UCLASS()
class UE5_0307_API UEchoAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	//获取动画所处的角色
	UPROPERTY(BlueprintReadOnly)
	AEchoCharacter* EchoChar;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	UCharacterMovementComponent* CharacterMovement;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool isFalling;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	ECharacterState CharacterState;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	TEnumAsByte<EDeathPose> DeathPose;

protected:

private:
	
	
};
