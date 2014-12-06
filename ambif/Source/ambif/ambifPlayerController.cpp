// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#include "ambif.h"
#include "ambifPlayerController.h"


AambifPlayerController::AambifPlayerController(const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer)
{
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
	bEnableMouseOverEvents = true;
}


