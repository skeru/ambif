// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

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

	static ConstructorHelpers::FObjectFinder<UBlueprint> TheHUDOb(TEXT("/Game/Blueprints/SimplePlayerHUD_BP"));
	if (TheHUDOb.Object != NULL)
	{
		HUDClass = (UClass*)TheHUDOb.Object->GeneratedClass;
	}

}


