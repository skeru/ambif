//* Copyright 2015 Stefano Cherubin
//* stefano1{dot}cherubin{at}mail{dot}polimi{dot}it
//* ISPG - Image and Sound Processing Group
//* Politecnico di Milano

#include "ambif.h"
#include "ambifPlayerController.h"


AambifPlayerController::AambifPlayerController(const FObjectInitializer & ObjectInitializer)
	:Super(ObjectInitializer)
{
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
	bEnableMouseOverEvents = true;
	bShowMouseCursor = true;
}


