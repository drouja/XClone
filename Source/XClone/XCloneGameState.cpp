// Fill out your copyright notice in the Description page of Project Settings.


#include "XCloneGameState.h"

void AXCloneGameState::EndTurn_Implementation()
{
	if (Turn == 0)
	{
		Turn = 1;
	}
	else if (Turn == 1)
	{
		Turn = 0;
	}
	StartTurn(Turn);
}

void AXCloneGameState::StartTurn_Implementation(int NewTurn)
{
	Turn = NewTurn;
}

AXCloneGameState::AXCloneGameState()
{
	Turn = 0;
}
