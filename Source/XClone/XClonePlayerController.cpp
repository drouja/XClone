// Fill out your copyright notice in the Description page of Project Settings.


#include "XClonePlayerController.h"
#include "StratCam.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "BaseFloatText.h"

AXClonePlayerController::AXClonePlayerController()
{
	Mode = Standard;
}

void AXClonePlayerController::BeginPlay()
{
	Super::SetupInputComponent();
	controlledpawn = Cast<AStratCam>(GetPawn());
	SetupPlayerInputComponent();
}

void AXClonePlayerController::SetupPlayerInputComponent()
{
	InputComponent->BindAxis("MoveForward", this, &AXClonePlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AXClonePlayerController::MoveRight);
	InputComponent->BindAxis("Scroll", this, &AXClonePlayerController::Scroll);
	InputComponent->BindAxis("Shift", this, &AXClonePlayerController::Q_E);
	InputComponent->BindAxis("MouseMove", this, &AXClonePlayerController::MouseMovement);
	InputComponent->BindAction("Tab", IE_Pressed, this, &AXClonePlayerController::Tab);
	InputComponent->BindAction("LeftClick", IE_Pressed, this, &AXClonePlayerController::LeftClick);
	InputComponent->BindAction("Enter", IE_Pressed, this, &AXClonePlayerController::Enter);
}

void AXClonePlayerController::MoveRight(float Value)
{
	switch (Mode)
	{
	case Standard:
		controlledpawn->MoveRight(Value);
		return;
	case AimShot:
		return;
	default:
		return;
	}
	
}

void AXClonePlayerController::MoveForward(float Value)
{
	switch (Mode)
	{
	case Standard:
		controlledpawn->MoveForward(Value);
		return;
	case AimShot:
		return;
	default:
		return;
	}
}

void AXClonePlayerController::Scroll(float Value)
{
	switch (Mode)
	{
	case Standard:
		controlledpawn->Zoom(Value);
		return;
	case AimShot:
		return;
	default:
		return;
	}
}

void AXClonePlayerController::Q_E(float Value)
{
	switch (Mode)
	{
	case Standard:
		controlledpawn->RotateCam(Value);
		return;
	case AimShot:
		return;
	default:
		return;
	}
	
}

void AXClonePlayerController::MouseMovement(float Value)
{
	switch (Mode)
	{
	case Standard:
		if (!CanAct()) return;
		controlledpawn->HighlightTile();
		return;
	case AimShot:
		return;
	default:
		return;
	}
}

void AXClonePlayerController::Tab()
{
	switch (Mode)
	{
	case Standard:
		controlledpawn->ChangeFocus();
		return;
	case AimShot:
		NextTarget(1);
		return;
	default:
		return;
	}
}

void AXClonePlayerController::LeftClick()
{
	switch (Mode)
	{
	case Standard:
		if(!CanAct()) return;
		controlledpawn->RequestMove();
		return;
	case AimShot:
		return;
	default:
		return;
	}
}

void AXClonePlayerController::Enter()
{
	switch (Mode)
	{
	case Standard:
		controlledpawn->EndTurn();
		return;
	case AimShot:
		return;
	default:
		return;
	}
}

bool AXClonePlayerController::CanAct()
{
	return (controlledpawn->focusedpawn->ActionsLeft>0 && controlledpawn->ismyturn());
}

void AXClonePlayerController::ToAimShot()
{
	switch (Mode)
	{
	case Standard:
		if (!CanAct()) return;
		HitChance = 0.0f;
		OGLoc = controlledpawn->focusedpawn->GetActorLocation();
		Mode = AimShot;
		controlledpawn->Currenthud->RemoveFromViewport();
		SetViewTargetWithBlend(controlledpawn->focusedpawn,0.5f);
		controlledpawn->Currenthud = CreateWidget<UUserWidget>(GetWorld(),controlledpawn->AimHud);
		controlledpawn->Currenthud->AddToViewport();
		controlledpawn->clearsplinemesh();
		index = -1;
		controlledpawn->GetTargetsInRange();
		NextTarget(1,true);
		return;
	case AimShot:
		return;
	default:
		return;
	}
}

void AXClonePlayerController::ToStandardMode()
{
	switch (Mode)
	{
	case Standard:
		return;
	case AimShot:
		Mode = Standard;
		GetWorldTimerManager().ClearTimer(TurnHandle);
		controlledpawn->focusedpawn->SetActorLocation(OGLoc);
		controlledpawn->Currenthud->RemoveFromViewport();
		SetViewTargetWithBlend(controlledpawn,0.5f);
		controlledpawn->Currenthud = CreateWidget<UUserWidget>(GetWorld(),controlledpawn->StandardHud);
		controlledpawn->Currenthud->AddToViewport();
		return;
	default:
		return;
	}
}

bool AXClonePlayerController::NextTarget(int direction, bool binstant)
{
	if(controlledpawn->TargetPawns.Num()<1)
	{
		controlledpawn->targetedpawn = nullptr;
		HitChance = -1.0;
		return false;
	}
	if(direction>=0)
	{
		index++;
		if (index>=controlledpawn->TargetPawns.Num())
		{
			index=0;
		}
	}
	else
	{
		index--;
		if(index<0)
		{
			index = controlledpawn->TargetPawns.Num()-1;
		}
	}
	controlledpawn->targetedpawn = controlledpawn->TargetPawns[index];
	TargetRot = controlledpawn->focusedpawn->GetActorRotation();
	TargetLoc = controlledpawn->ShootFromLocs[index];
	TargetRot.Yaw = UKismetMathLibrary::MakeRotFromXZ(
		controlledpawn->targetedpawn->GetActorLocation()-TargetLoc,
		controlledpawn->focusedpawn->GetActorUpVector()).Yaw;
	GetWorldTimerManager().ClearTimer(TurnHandle);
	if (!binstant)
		GetWorldTimerManager().SetTimer(TurnHandle, this, &AXClonePlayerController::Turn, 0.01, true, 0.0f);
	else
	{
		controlledpawn->focusedpawn->SetActorRotation(TargetRot);
		controlledpawn->focusedpawn->SetActorLocation(TargetLoc);
	}
	HitChance = (controlledpawn->ExposureScores[index]/3.0)-
		UKismetMathLibrary::Lerp(0,0.3,(controlledpawn->targetedpawn->GetActorLocation()-controlledpawn->focusedpawn->GetActorLocation()).Size()/1000.0);
	return true;
}

void AXClonePlayerController::Turn()
{
	Axpawn* pawn = controlledpawn->focusedpawn;
	pawn->SetActorRotation(UKismetMathLibrary::RInterpTo(pawn->GetActorRotation(),
		TargetRot,GetWorld()->DeltaTimeSeconds,4.0f));
	pawn->SetActorLocation(UKismetMathLibrary::VInterpTo(pawn->GetActorLocation(),
		TargetLoc,GetWorld()->DeltaTimeSeconds,4.0f));
	if(pawn->GetActorRotation().Equals(TargetRot,0.01)) GetWorldTimerManager().ClearTimer(TurnHandle);
}

void AXClonePlayerController::Shoot()
{
	if(controlledpawn->focusedpawn->ActionsLeft<=0) return;
	if (HitChance>0.0)
	{
		server_shoot(controlledpawn->focusedpawn,HitChance,controlledpawn->targetedpawn->GetActorLocation(),controlledpawn->targetedpawn);
	}
}

void AXClonePlayerController::server_shoot_Implementation(Axpawn* fp, float hc, FVector loc, Axpawn* tp)
{
	fp->Attack(hc,loc,tp);
}

bool AXClonePlayerController::server_shoot_Validate(Axpawn* fp,float hc, FVector loc, Axpawn* tp)
{
	return true;
}
