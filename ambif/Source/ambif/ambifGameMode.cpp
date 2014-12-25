// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#include "ambif.h"
#include "ambifPlayerController.h"
#include "ambifGameMode.h"


AambifGameMode::AambifGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnBPClass(TEXT("/Game/Blueprints/BrowserCharacter_BP"));
	if (PlayerPawnBPClass.Object != NULL)
	{
		DefaultPawnClass = (UClass*)PlayerPawnBPClass.Object->GeneratedClass;
	}

	/* HUD DISABLED
	static ConstructorHelpers::FObjectFinder<UBlueprint> TheHUDOb(TEXT("/Game/Blueprints/SimplePlayerHUD_BP"));
	if (TheHUDOb.Object != NULL)
	{
		HUDClass = (UClass*)TheHUDOb.Object->GeneratedClass;
	}
	*/

	PlayerControllerClass = AambifPlayerController::StaticClass();
}


