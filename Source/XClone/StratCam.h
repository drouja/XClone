// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "xpawn.h"
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
	void MoveTo(FVector loc);
public:
	void MoveRight(float Value);
	void MoveForward(float Value);
	void Zoom(float Value);
	void RotateCam(float Value);
	void ChangeFocus();
	void RequestMove();
	void EndTurn();
	void HighlightTile();
	

private:
	FTimerHandle findtile;

protected:
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	class UCameraComponent* Cam;
	class AXCloneGameState* GameState;
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
	class Axpawn* focusedpawn;
	// Not sure what this does
	class Axpawn* targetedpawn;
protected:
	class Atile* oldtile;
	int focusindex;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	TEnumAsByte<Team> playerteam;
public:
	void clearsplinemesh();

// Multiplayer functions
protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void server_requestmove(FVector loc, FRotator rot, Axpawn* focusedpawn1);
	void startmovepawn();
	UFUNCTION()
	void movepawn();
	float movedist;
	FTimerHandle movehandle;
public:
	bool ismyturn();
	void StartTurn();

	//Hud Stuff
public:
	UPROPERTY(EditAnywhere, Category=HUD)
	TSubclassOf<UUserWidget> StandardHud;
	UPROPERTY(EditAnywhere, Category=HUD)
	TSubclassOf<UUserWidget> AimHud;
	UPROPERTY(EditAnywhere, Category=HUD)
	TSubclassOf<UUserWidget> EnemyTurnHud;
	UPROPERTY(EditAnywhere, Category=HUD)
	TSubclassOf<UUserWidget> EndGameHud;
	UUserWidget* Currenthud;
private:
	UFUNCTION(Server,Reliable)
	void CallEndTurn();
	
};


