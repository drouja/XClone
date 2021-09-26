// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleManager.h"
#include "xpawn.h"
#include "StratCam.h"
#include "Containers/Array.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABattleManager::ABattleManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	focusindex = 0;
}

// Called when the game starts or when spawned
void ABattleManager::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor* > foundpawns;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), Axpawn::StaticClass(), foundpawns);
	for (AActor* Actor : foundpawns)
	{
		friendlypawns.Add(Cast<Axpawn>(Actor));
	}
	focusedpawn = friendlypawns[0];
	TArray<AActor* >foundcamera;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStratCam::StaticClass(), foundcamera);
	for (AActor* Actor : foundcamera)
	{
		cam = Cast<AStratCam>(Actor);
	}

}

// Called every frame
void ABattleManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

Axpawn* ABattleManager::CycleFocus()
{
	focusindex++;
	if (focusindex >= friendlypawns.Num()) focusindex = 0;
	focusedpawn = friendlypawns[focusindex];
	return focusedpawn;
}

