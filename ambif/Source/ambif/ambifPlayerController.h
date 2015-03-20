// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

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
