// Fill out your copyright notice in the Description page of Project Settings.


#include "XClonePlayerController.h"
#include "StratCam.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"

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
		controlledpawn->HighlightTile();
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
	case AimShot:
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
		controlledpawn->RequestMove();
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
	case AimShot:
		return;
	default:
		return;
	}
}

void AXClonePlayerController::ToAimShot()
{
	switch (Mode)
	{
	case Standard:
		Mode = AimShot;
		controlledpawn->Currenthud->RemoveFromViewport();
		SetViewTargetWithBlend(controlledpawn->focusedpawn,0.5f);
		controlledpawn->Currenthud = CreateWidget<UUserWidget>(GetWorld(),controlledpawn->AimHud);
		controlledpawn->Currenthud->AddToViewport();
		controlledpawn->clearsplinemesh();
		index = -1;
		controlledpawn->GetTargetsInRange();
		NextTarget(1,true);
		break;
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
		controlledpawn->Currenthud->RemoveFromViewport();
		SetViewTargetWithBlend(controlledpawn,0.5f);
		controlledpawn->Currenthud = CreateWidget<UUserWidget>(GetWorld(),controlledpawn->StandardHud);
		controlledpawn->Currenthud->AddToViewport();
		break;
	default:
		return;
	}
}

bool AXClonePlayerController::NextTarget(int direction, bool binstant)
{
	if(controlledpawn->TargetPawns.Num()<1) return false;
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
	TargetRot = UKismetMathLibrary::MakeRotFromXZ(
		controlledpawn->targetedpawn->GetActorLocation()-controlledpawn->focusedpawn->GetActorLocation(),
		controlledpawn->focusedpawn->GetActorUpVector());
	GetWorldTimerManager().ClearTimer(TurnHandle);
	if (!binstant)
		GetWorldTimerManager().SetTimer(TurnHandle, this, &AXClonePlayerController::Turn, 0.01, true, 0.0f);
	else controlledpawn->focusedpawn->SetActorRotation(TargetRot);
	return true;
}

void AXClonePlayerController::Turn()
{
	Axpawn* pawn = controlledpawn->focusedpawn;
	pawn->SetActorRotation(UKismetMathLibrary::RInterpTo(pawn->GetActorRotation(),
		TargetRot,GetWorld()->DeltaTimeSeconds,4.0f));
	if(pawn->GetActorRotation().Equals(TargetRot,0.01)) GetWorldTimerManager().ClearTimer(TurnHandle);
}

