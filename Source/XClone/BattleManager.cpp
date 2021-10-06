// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleManager.h"
#include "xpawn.h"
#include "tile.h"
#include "StratCam.h"
#include "Containers/Array.h"
#include "Kismet/GameplayStatics.h"
#include "Algo/Reverse.h"
#include "DrawDebugHelpers.h"

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

void ABattleManager::Pathfind(Atile* end, TArray<FVector>& path)
{
	Atile* start = focusedpawn->FindTile();
	TArray<Atile*> open;
	open.Add(start);
	TArray<Atile*> closed;
	//float timeout{ 0 };
	while (true)
	{
		if (open.Num()>1)
			open.HeapSort([&]( const Atile & itemA, const Atile & itemB)
				{
					return (itemA.fcost < itemB.fcost);
				});
		Atile* current = open[0];
		open.RemoveAt(0);
		closed.Add(current);
		if (current == end)
		{
			makepath(start,end,path);
			return;
		}
		for (int i{ 0 }; i < 8;i++) {
			Atile* neighbour = current->neighbours[i];
			if (neighbour == nullptr || closed.Contains(neighbour)) continue;
			bool notcontains = !open.Contains(neighbour);
			if (notcontains || ((current->gcost + ABattleManager::h(neighbour, current)) < neighbour->gcost))
			{
				neighbour->gcost = current->gcost + ABattleManager::h(neighbour, current);
				neighbour->hcost = ABattleManager::h(neighbour, end);
				neighbour->fcost = neighbour->hcost + neighbour->gcost;
				neighbour->parent = current;
				if (notcontains) open.Add(neighbour);
			}
		}
		//timeout += GetWorld()->DeltaTimeSeconds;
	}
}

void ABattleManager::makepath(Atile* begin, Atile* end, TArray<FVector>& path)
{
	path.Empty();
	Atile* current = end;
	while (current != begin)
	{
		path.Add(current->GetActorLocation());
		current = current->parent;
	}
	Algo::Reverse(path);

	/*
	for (int i{1};i<path.Num();i++)
	{
		DrawDebugLine(GetWorld(), path[i - 1], path[i], FColor(52, 220, 239), true);
	}
	*/
}


inline bool ABattleManager::SortPredicate(class Atile* itemA, class Atile* itemB)
{
	return (itemA->fcost < itemB->fcost);
}

inline float ABattleManager::h(Atile* itemA, Atile* itemB)
{
	return (itemA->GetActorLocation() - itemB->GetActorLocation()).Size();
}