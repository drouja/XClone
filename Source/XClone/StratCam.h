// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "StratCam.generated.h"

UCLASS()
class XCLONE_API AStratCam : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AStratCam();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void MoveRight(float Value);
	void MoveForward(float Value);
	void Zoom(float Value);
	void RotateCam(float Value);
	void ChangeFocus();
	void MoveTo(FVector loc);
	void HighlightTile();

private:
	FTimerHandle findtile;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	class UCameraComponent* Cam;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	class USpringArmComponent* SpringArmComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	class USceneComponent* Arootcomponent;
	class ABattleManager* battlemanager;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	class UStaticMeshComponent* select;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	class USplineComponent* path;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	TArray<class USplineMeshComponent*> pathmesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	UStaticMesh* meshref;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float speed;
	float scrollspeed;
	FVector desiredloc;
	bool movetodesiredloc;
	TArray<FVector> patharray;
protected:
	class Atile* oldtile;

};
