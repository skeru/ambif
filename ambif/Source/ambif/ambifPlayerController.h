//* Copyright 2015 Stefano Cherubin
//* stefano1{dot}cherubin{at}mail{dot}polimi{dot}it
//* ISPG - Image and Sound Processing Group
//* Politecnico di Milano

#pragma once

#include "GameFramework/PlayerController.h"
#include "ambifPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class AMBIF_API AambifPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AambifPlayerController(const FObjectInitializer & ObjectInitializer);
	
};
