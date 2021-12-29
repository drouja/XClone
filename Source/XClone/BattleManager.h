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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	bool Pathfind(Atile* end, TArray<FVector>& path, class Axpawn* focusedpawn);
	void makepath(Atile* start, Atile* end, TArray<FVector>& path, class Axpawn* focusedpawn);
	bool ismoving; //because unreal is really really fun to use!!!!!!!!!!!!!!!!
	TArray<class Axpawn*> FriendlyPawns;
	void GetFriendlyPawns(TEnumAsByte<enum Team> playerteam);

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	TArray<class Axpawn*> TargetPawns;
	TArray<FVector> ShootFromLocs;
	TArray<FVector> AimAtLocs;
	TArray<int> ExposureScores;
	void GetTargetsInRange(TEnumAsByte<enum Team> playerteam, Axpawn* focusedpawn);
	UFUNCTION(NetMulticast,Reliable)
	void Delete_Multicast(uint32 ID);
	UFUNCTION(NetMulticast,Reliable)
	void EndGame_Multicast();

};
