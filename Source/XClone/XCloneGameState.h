// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "XCloneGameState.generated.h"

/**
 * 
 */
UCLASS()
class XCLONE_API AXCloneGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	AXCloneGameState();
	int Turn;
	UFUNCTION(Server, Reliable)
	void EndTurn();
private:
	UFUNCTION(NetMulticast, Reliable)
	void StartTurn(int NewTurn);
	
		
};
