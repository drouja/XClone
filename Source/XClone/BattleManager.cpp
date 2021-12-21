// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleManager.h"
#include "xpawn.h"
#include "tile.h"
#include "Containers/Array.h"
#include "Algo/Reverse.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABattleManager::ABattleManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	ismoving = false;
}

// Called when the game starts or when spawned
void ABattleManager::BeginPlay()
{
}

// Called every frame
void ABattleManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ABattleManager::Pathfind(Atile* end, TArray<FVector>& path, Axpawn* focusedpawn)
{
	// if destination already contains pawn, don't pathfind to it
	TArray<AActor* > result1;
	end->GetOverlappingActors(result1, Axpawn::StaticClass());
	if (result1.Num() > 0) return false;

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
			makepath(start,end,path, focusedpawn);
			return true;
		}
		for (int i{ 0 }; i < 8;i++) {
			Atile* neighbour = current->neighbours[i];
			
			if (neighbour == nullptr || closed.Contains(neighbour)) continue;

			// Test if currently checked tile is already occupied
			TArray<AActor* > result;
			neighbour->GetOverlappingActors(result,Axpawn::StaticClass());
			if (result.Num() > 0) continue;

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
	}
}

void ABattleManager::makepath(Atile* begin, Atile* end, TArray<FVector>& path, Axpawn* focusedpawn)
{
	path.Empty();
	Atile* current = end;
	while (current != begin)
	{
		path.Add(current->GetActorLocation());
		current = current->parent;
	}
	if (path.Num() > focusedpawn->movedist)
	{
		path.RemoveAt(0, path.Num() - focusedpawn->movedist, true);
	}
	path.Add(begin->GetActorLocation());
	Algo::Reverse(path);
}


inline bool ABattleManager::SortPredicate(class Atile* itemA, class Atile* itemB)
{
	return (itemA->fcost < itemB->fcost);
}

inline float ABattleManager::h(Atile* itemA, Atile* itemB)
{
	return (itemA->GetActorLocation() - itemB->GetActorLocation()).Size();
}
