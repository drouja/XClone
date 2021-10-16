// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleManager.h"
#include "xpawn.h"
#include "tile.h"
#include "StratCam.h"
#include "Containers/Array.h"
#include "Kismet/GameplayStatics.h"
#include "Algo/Reverse.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SplineComponent.h"

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

bool ABattleManager::Pathfind(Atile* end, TArray<FVector>& path)
{
	if (GetWorldTimerManager().IsTimerActive(movehandle)) return false; //If pawn is moving dont pathfind

	// if destination already contains tile, don't pathfind to it
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
			makepath(start,end,path);
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

void ABattleManager::startmovepawn(Atile* end, USplineComponent* spline)
{
	if (GetWorldTimerManager().IsTimerActive(movehandle) || focusedpawn->FindTile() == end) return;
	FTimerDelegate movehandledel;
	movedist = 0;
	movehandledel.BindUFunction(this, FName("movepawn"), end, spline);
	GetWorldTimerManager().SetTimer(movehandle, movehandledel, 0.01, true, 0.0f);
}

void ABattleManager::movepawn(Atile* end, USplineComponent* spline)
{
	movedist += 3;
	if (movedist >= spline->GetSplineLength())
	{
		GetWorldTimerManager().ClearTimer(movehandle);
		return;
	}
	FVector loc = spline->GetLocationAtDistanceAlongSpline(movedist, ESplineCoordinateSpace::World);
	loc.Z = focusedpawn->GetActorLocation().Z; //Temporary so that pawn doesnt go down into floor
	focusedpawn->SetActorLocationAndRotation(loc, spline->GetRotationAtDistanceAlongSpline(movedist, ESplineCoordinateSpace::World));
}