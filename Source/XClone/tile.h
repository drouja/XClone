// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "tile.generated.h"

//EditInlineNew
UCLASS()
class XCLONE_API Atile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	Atile();
	UPROPERTY(VisibleAnywhere, Category = Debug)
	Atile* neighbours[8] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}; //Array of Neighboring nodes
	float gcost;
	float hcost;
	float fcost;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	class UStaticMeshComponent* tilemesh;
	UPROPERTY(VisibleAnywhere, Category = Debug)
	class UBoxComponent* box;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
