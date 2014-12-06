// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#pragma once

#include "GameFramework/Character.h"
#include "MapElements/MapElementActor.h"
#include "LogicController.h"
#include "BrowserCharacter.generated.h"

/**
 * 
 */
UCLASS()
class AMBIF_API ABrowserCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	//constructor
	ABrowserCharacter(const FObjectInitializer& ObjectInitializer);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LogicController)
	/** Manager Actor */
	ALogicController* Manager;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	/** Camera boom positioning the camera behind the character */
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UCameraComponent* FollowCamera;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	/** Speed multplier on X axis */
	float SpeedX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	/** Speed multplier on Y axis */
	float SpeedY;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	/** Speed multplier on Z axis */
	float SpeedZ;

protected:
	void CameraZoomIn();

	void CameraZoomOut();

	float cameraZoom_current;

	void MoveForward(float Val);

	void MoveRight(float Val);

	void MoveUp(float Val);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

protected:
	UFUNCTION()
	void OnBeginOverlap(AActor* Other);

	UFUNCTION()
	void OnEndOverlap(AActor* Other);

};
