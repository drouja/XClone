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
	inline static bool SortPredicate(class Atile* itemA, class Atile* itemB);
	inline static float h(class Atile* itemA, class Atile* itemB);
protected:
	TArray<class Axpawn*> friendlypawns;
	class AStratCam* cam;
	class Axpawn* focusedpawn;
	int focusindex;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	class Axpawn* CycleFocus();
	void Pathfind(Atile* end);
	void makepath(Atile* start, Atile* end);
	TArray<FVector> path;

};
