// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "xpawn.generated.h"

UENUM()
enum Team
{
	Red		UMETA(DisplayName = "Red"),
	Blue	UMETA(DisplayName = "Blue"),
};

UCLASS()
class XCLONE_API Axpawn : public APawn
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	class USpringArmComponent* SpringArmComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	class UCameraComponent* Cam;

	
public:
	// Sets default values for this pawn's properties
	Axpawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	class UStaticMeshComponent* pawnmesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	class UStaticMeshComponent* gunmesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	class UCapsuleComponent* collider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	class USceneComponent* Arootcomponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	UMaterialInstanceDynamic* team_mat;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	class Atile* FindTile();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	int movedist;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	TEnumAsByte<Team> team;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	FLinearColor team_colour;
	
	

};
