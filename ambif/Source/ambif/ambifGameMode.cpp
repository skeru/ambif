//* Copyright 2015 Stefano Cherubin
//* stefano1{dot}cherubin{at}mail{dot}polimi{dot}it
//* ISPG - Image and Sound Processing Group
//* Politecnico di Milano

#include "ambif.h"
#include "ambifPlayerController.h"
#include "ambifGameMode.h"


AambifGameMode::AambifGameMode(const FObjectInitializer & ObjectInitializer) 
	:Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnBPClass(TEXT("/Game/Blueprints/BrowserCharacter_BP"));
	if (PlayerPawnBPClass.Object != NULL)
	{
		DefaultPawnClass = (UClass*)PlayerPawnBPClass.Object->GeneratedClass;
	}

	/* HUD used for tooltip messages */
	static ConstructorHelpers::FObjectFinder<UBlueprint> Default_HUD_Obj(TEXT("Blueprint'/Game/HUD/InspectionHUD_BP.InspectionHUD_BP'"));
	if (Default_HUD_Obj.Object != NULL)
	{
		HUDClass = (UClass*)Default_HUD_Obj.Object->GeneratedClass;
	}

	PlayerControllerClass = AambifPlayerController::StaticClass();
}


