// Fill out your copyright notice in the Description page of Project Settings.


#include "XCloneGameState.h"

#include "XClonePlayerController.h"
#include "Kismet/GameplayStatics.h"

void AXCloneGameState::EndTurn_Implementation()
{
	if (Turn == 0)
	{
		StartTurn(1);
	}
	else if (Turn == 1)
	{
		StartTurn(0);
	}
	
}

void AXCloneGameState::StartTurn_Implementation(int NewTurn)
{
	AXClonePlayerController* pc = Cast<AXClonePlayerController>(GetWorld()->GetFirstPlayerController());
	pc->StartTurn();
	Turn = NewTurn;
}

AXCloneGameState::AXCloneGameState()
{
	Turn = 0;
}
