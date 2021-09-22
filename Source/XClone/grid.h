// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "tile.h"
#include "grid.generated.h"

//Struct for getting around 2D TARRAY limitation
USTRUCT(BlueprintType)
struct FArray2d
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<Atile *> ary;
	FArray2d()
	{
	}

};

UCLASS()
class XCLONE_API Agrid : public AActor
{
	GENERATED_BODY()

	//Debug Box for showing grid size
	UPROPERTY(VisibleAnywhere, Category = Debug)
	class UBoxComponent* box;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:	
	// Sets default values for this actor's properties
	Agrid();
	void UpdateGrid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, Category = Settings)
	float nodeDiameter;
	UPROPERTY(EditAnywhere, Category = Settings)
	int numnodes[2];
	UPROPERTY(EditAnywhere, Category = Settings)
	TArray<FArray2d> grid;

};
