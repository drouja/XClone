// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StratCam.h"
#include "GameFramework/PlayerController.h"
#include "XClonePlayerController.generated.h"

UENUM()
enum EControlMode
{
	Standard UMETA(DisplayName = "Standard"),
	AimShot	UMETA(DisplayName = "AimShot"),
};

/**
 * 
 */
UCLASS()
class XCLONE_API AXClonePlayerController : public APlayerController
{
	GENERATED_BODY()
	AXClonePlayerController();
	virtual void BeginPlay() override;
	AStratCam * controlledpawn;

private:
	void MoveRight(float Value);
	void MoveForward(float Value);
	void Scroll(float Value);
	void Q_E(float Value);
	void MouseMovement(float Value);
	void Tab();
	void LeftClick();
	void Enter();

protected:
	UFUNCTION(BlueprintCallable)
	virtual void ToAimShot();
	UFUNCTION(BlueprintCallable)
	virtual void ToStandardMode();
	UFUNCTION(BlueprintCallable)
	virtual bool NextTarget(int direction, bool binstant = false);
	UFUNCTION(BlueprintCallable)
	virtual void Shoot();
	UFUNCTION(Server, Reliable, WithValidation)
	void server_shoot(Axpawn* fp,float hc, FVector loc, Axpawn* tp);
	int index;
	void Turn();
	
	FRotator TargetRot;
	FVector TargetLoc;
	FVector OGLoc;
protected:
	FTimerHandle TurnHandle;
public:
	UPROPERTY(BlueprintReadOnly)
	float HitChance;

public:
	virtual void SetupPlayerInputComponent();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mode)
	TEnumAsByte<EControlMode> Mode;
	
	
};
