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
	UPROPERTY(EditAnywhere, Category = Camera)
	/** Quadratically increase view angle when getting closer. */
	bool bUseParabolicZoom;

public:
	UFUNCTION(BlueprintCallable, Category = "Camera Management")
	/** Reset Camera to default values */
	void ResetCamera();

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
	 * Does not update Widget Zoom 
	 * (could cause loop due to circular reference)
	 * (I know it's shitty by design but quickly just works) */
	void CameraZoomTo(float ZoomPercent);

	UFUNCTION(BlueprintCallable, Category = "Camera Management")
	/** Activate / Deactivate parabolic zoom. 
	 *
	 * Parabolic zoom quadratically increase view angle when getting closer. */
	void SetParabolicZoomEnable(bool Enable);

	UFUNCTION(BlueprintCallable, Category = "Camera Management")
	/** Returns true if and only if parabolic zoom is in use. */
	bool IsParabolicZoomEnable();

private:
	float cameraZoom_current;

	//-------------------PARABOLIC CAMERA ZOOM--------------------
private:
	UFUNCTION(BlueprintCallable, Category = "Parabolic Camera Management")
	/** Parabolic camera zoom step in */
	void ParabolicCameraZoomIn();

	UFUNCTION(BlueprintCallable, Category = "Parabolic Camera Management")
	/** Parabolic camera zoom step out */
	void ParabolicCameraZoomOut();

	UFUNCTION(BlueprintCallable, Category = "Parabolic Camera Management")
	/** Parabolic camera zoom set to value.
	 * Zoom 0 means close to object.
	 *
	 * Does not update Widget Zoom */
	void ParabolicCameraZoomTo(float ZoomPercent);

	//---------------------LINEAR CAMERA ZOOM----------------------
private:
	UFUNCTION(BlueprintCallable, Category = "Linear Camera Management")
	/** Linear camera zoom step in */
	void LinearCameraZoomIn();

	UFUNCTION(BlueprintCallable, Category = "Linear Camera Management")
	/** Linear camera zoom step out */
	void LinearCameraZoomOut();

	UFUNCTION(BlueprintCallable, Category = "Linear Camera Management")
	/** Linear camera zoom set to value.
	 * Zoom 0 means close to object.
	 *
	 * Does not update Widget Zoom */
	void LinearCameraZoomTo(float ZoomPercent);


	//------------------------- AXES CONTROL -------------------------
private:
	void Turn(float Val);

	void LookUp(float Val);

	//-------------------------CHARACTER MOVEMEMENT-------------------------
private:
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
