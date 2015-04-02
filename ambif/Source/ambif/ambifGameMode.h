//* Copyright 2015 Stefano Cherubin
//* stefano1{dot}cherubin{at}mail{dot}polimi{dot}it
//* ISPG - Image and Sound Processing Group
//* Politecnico di Milano

#pragma once

#include "GameFramework/GameMode.h"
#include "ambifGameMode.generated.h"

/**
 * 
 */
UCLASS()
class AMBIF_API AambifGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	//constructor
	AambifGameMode(const FObjectInitializer & ObjectInitializer);
	
};
