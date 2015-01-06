// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#pragma once

#include "GameFramework/Character.h"
#include "MapElements/MapElementActor.h"
#include "LogicController.h"
#include "BrowserCharacter.generated.h"

/** Character used to focus elements.
 *
 * Also contains camera controls.
 */
UCLASS()
class AMBIF_API ABrowserCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	//constructor
	ABrowserCharacter(const FObjectInitializer& ObjectInitializer);

	//-------------------------ACTOR REFERENCES-------------------------
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LogicController)
	/** Manager Actor */
	ALogicController* Manager;

	//-------------------------CAMERA COMPONENTS-------------------------
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	/** Camera boom positioning the camera behind the character */
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UCameraComponent* FollowCamera;

	//-------------------------CHARACTER SPEED-------------------------
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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	/** Speed multplier for rotation */
	float SpeedRotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
	/** Speed multplier for LookUp */
	float SpeedLookUp;

	//-------------------------CAMERA ZOOM-------------------------

private:
	UFUNCTION(BlueprintCallable, Category = "Camera Management")
	/** Reset Camera to default values */
	void ResetCamera();

public:
	UFUNCTION(BlueprintCallable, Category = "Camera Management")
	/** Camera zoom step in */
	void CameraZoomIn();

	UFUNCTION(BlueprintCallable, Category = "Camera Management")
	/** Camera zoom step out */
	void CameraZoomOut();

	UFUNCTION(BlueprintCallable, Category = "Camera Management")
	/** Camera zoom set to value.
	 * Zoom 0 means close to object.
	 *
	 * Does not update Widget Zoom */
	void CameraZoomTo(float ZoomPercent);

	float cameraZoom_current;

	//------------------------- AXES CONTROL -------------------------

	void Turn(float Val);

	void LookUp(float Val);

	//-------------------------CHARACTER MOVEMEMENT-------------------------

	void MoveForward(float Val);

	void MoveRight(float Val);

	void MoveUp(float Val);

	//-------------------------INPUT MAPPING-------------------------

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

	//-------------------------OVERLAP EVENTS-------------------------

protected:
	UFUNCTION()
	void OnBeginOverlap(AActor* Other);

	UFUNCTION()
	void OnEndOverlap(AActor* Other);

};
