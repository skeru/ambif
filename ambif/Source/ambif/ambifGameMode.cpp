// Fill out your copyright notice in the Description page of Project Settings.

#include "ambif.h"
#include "ambifGameMode.h"


AambifGameMode::AambifGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnBPClass(TEXT("/Game/Blueprints/BrowserCharacter_BP"));
	if (PlayerPawnBPClass.Object != NULL)
	{
		DefaultPawnClass = (UClass*)PlayerPawnBPClass.Object->GeneratedClass;
	}

}


