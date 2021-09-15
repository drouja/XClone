// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class XCLONE_API Node
{
public:
	Node();
	~Node();
public:
	Node * neighbours[8]; //Array of Neighboring nodes
	FVector loc; //Node location
	enum struct type {walkable,block}; //Node type
};
