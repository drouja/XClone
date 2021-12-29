// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleManager.h"
#include "xpawn.h"
#include "tile.h"
#include "Containers/Array.h"
#include "Algo/Reverse.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "XClonePlayerController.h"

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

void ABattleManager::Delete_Multicast_Implementation(uint32 ID)
{
	for (auto Fpawn : FriendlyPawns)
	{
		if (Fpawn->ID == ID)
		{
			FriendlyPawns.Remove(Fpawn);
			break;
		}
	}
	uint32 index = 0;
	for (auto Tpawn : TargetPawns)
	{
		if (Tpawn->ID == ID)
		{
			TargetPawns.Remove(Tpawn);
			ShootFromLocs.RemoveAt(index);
			AimAtLocs.RemoveAt(index);
			ExposureScores.RemoveAt(index);
			Cast<AXClonePlayerController>(GetWorld()->GetFirstPlayerController())->NextTarget(1);
			return;
		}
		index++;
	}
	if (FriendlyPawns.Num()<=0)
	{
		Cast<AXClonePlayerController>(GetWorld()->GetFirstPlayerController())->StartEndGame();
	}
}

void ABattleManager::GetFriendlyPawns(TEnumAsByte<Team> playerteam)
{
	TArray<AActor* > foundpawns;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), Axpawn::StaticClass(), foundpawns);
	for (AActor* Actor : foundpawns)
	{
		Axpawn* foundpawn = Cast<Axpawn>(Actor);
		if (foundpawn != nullptr && foundpawn->team == playerteam)
		{
			FriendlyPawns.Add(foundpawn);
		}
			
	}
}

void ABattleManager::GetTargetsInRange(TEnumAsByte<Team> playerteam, Axpawn* focusedpawn)
{
	TargetPawns.Empty();
	AimAtLocs.Empty();
	ShootFromLocs.Empty();
	ExposureScores.Empty();
	
	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add(focusedpawn);
	FHitResult Outhit{};
	
	TArray<AActor* > foundpawns;
	TArray<Axpawn* > EnemyPawns;
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), Axpawn::StaticClass(), foundpawns);
	for (AActor* Actor : foundpawns)
	{
		Axpawn* foundpawn = Cast<Axpawn>(Actor);
		if (foundpawn != nullptr &&
			foundpawn->team != playerteam) EnemyPawns.Add(foundpawn);
	}
	
	FVector OGStartLoc {focusedpawn->GunLoc->GetComponentLocation()};
	Atile* CurrentTile = focusedpawn->FindTile();
	
	for (Axpawn* pawn : EnemyPawns)
	{
		FVector StartLoc = OGStartLoc;
		bool nextpawn = false;
		TArray<USceneComponent*> Targets {};
		Targets.Add(pawn->HeadLoc);
		Targets.Add(pawn->BodyLoc);
		Targets.Add(pawn->LegLoc);
		
		
		for (int i{-1};i<4;i++)
		{
			if (nextpawn) break;
			if(i!=-1)
			{
				FVector NeighborLoc{};
				if (CurrentTile->neighbours[i] !=nullptr)
					NeighborLoc = CurrentTile->neighbours[i]->GetActorLocation();
				StartLoc.X=NeighborLoc.X;
				StartLoc.Y=NeighborLoc.Y;
			}
			for (USceneComponent* Target : Targets)
			{
				if (UKismetSystemLibrary::LineTraceSingle(this,
				StartLoc,
				Target->GetComponentLocation(),
				UEngineTypes::ConvertToTraceType(ECC_Visibility),
				false, actorsToIgnore,
				EDrawDebugTrace::None, Outhit,
				true, FLinearColor::Red,
				FLinearColor::Green, 0.0f)
			)
				if(Outhit.Actor == pawn)
				{
					if (!nextpawn)
					{
						TargetPawns.Add(pawn);
						ShootFromLocs.Add(StartLoc);
						AimAtLocs.Add(Target->GetComponentLocation());
						ExposureScores.Add(1);
					}
					else ExposureScores[ExposureScores.Num()-1]++;
					nextpawn = true;
				}
				
			}
			
		}
	}
}

void ABattleManager::EndGame_Multicast_Implementation()
{
	Cast<AXClonePlayerController>(GetWorld()->GetFirstPlayerController())->DrawEndScreen(FriendlyPawns.Num()>0);
}

inline bool ABattleManager::SortPredicate(class Atile* itemA, class Atile* itemB)
{
	return (itemA->fcost < itemB->fcost);
}

inline float ABattleManager::h(Atile* itemA, Atile* itemB)
{
	return (itemA->GetActorLocation() - itemB->GetActorLocation()).Size();
}