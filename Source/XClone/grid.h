// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "grid.generated.h"

UCLASS()
class XCLONE_API Agrid : public AActor
{
	GENERATED_BODY()

	//Debug Box for showing grid size
	UPROPERTY(EditAnywhere, Category = Debug)
	class UBoxComponent* box;

public:	
	// Sets default values for this actor's properties
	Agrid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	FVector2D size;
	UPROPERTY(EditAnywhere, Category = Settings)
	float nodeDiameter;
	UPROPERTY(VisibleAnywhere, Category = Settings)
	int numnodes[2];

};
