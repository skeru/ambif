// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#include "ambif.h"
#include "ambifPlayerController.h"


AambifPlayerController::AambifPlayerController(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
	bEnableMouseOverEvents = true;
}


