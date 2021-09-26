// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleManager.generated.h"

UCLASS()
class XCLONE_API ABattleManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABattleManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
protected:
	TArray<class Axpawn*> friendlypawns;
	class AStratCam* cam;
	class Axpawn* focusedpawn;
	int focusindex;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	class Axpawn* CycleFocus();

};
