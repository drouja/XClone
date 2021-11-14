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
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	TArray<class Axpawn*> TargetPawns;
	class Axpawn* targetedpawn;
protected:
	class Atile* oldtile;
	TArray<class Axpawn*> friendlypawns;
	int focusindex;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	TEnumAsByte<Team> playerteam;
public:
	void clearsplinemesh();
	void GetTargetsInRange();

// Multiplayer functions
protected:
	 UFUNCTION(Server, Reliable, WithValidation)
	void server_requestmove(FVector loc, FRotator rot, Axpawn* focusedpawn1);
	void startmovepawn();
	UFUNCTION()
	void movepawn();
	float movedist;
	FTimerHandle movehandle;
	UFUNCTION(Server, Reliable, WithValidation)
	void server_endturn();
	bool ismyturn();

	//Hud Stuff
public:
	UPROPERTY(EditAnywhere, Category=HUD)
	TSubclassOf<UUserWidget> StandardHud;
	UPROPERTY(EditAnywhere, Category=HUD)
	TSubclassOf<UUserWidget> AimHud;
	UUserWidget* Currenthud;
	
};


