// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#include "ambif.h"
#include "CustomUtils/DebugUtils.h"
#include "CustomUtils/CameraMacros.h"
#include "BrowserCharacter.h"

#define CHARACTER_MOVEMENT_SPEED_BASE 0.02f
#define CHARACTER_MOVEMENT_SPEED(cameraZoom_current) (CAMERA_ZOOM_PERCENT((cameraZoom_current))*CAMERA_ZOOM_PERCENT((cameraZoom_current)) + CHARACTER_MOVEMENT_SPEED_BASE)

//#define ENABLE_CAMERA_DEBUG_MESSAGES
//#define ENABLE_OVERLAP_SELECTION
//#define ENABLE_OVERLAP_DEBUG_MESSAGES

// comment/uncomment this define to update or not zoom widget 
//#define SKIP_ZOOM_WIDGET

//constructor for the implemented interface. Needed in 4.7 to keep UHT quiet
UBrowserController_i::UBrowserController_i(const FObjectInitializer & ObjectInitializer) : Super(ObjectInitializer) {};

ABrowserCharacter::ABrowserCharacter() {
	GetCapsuleComponent()->SetEnableGravity(false);
	GetCapsuleComponent()->SetCapsuleSize(150.0f, 0.0f);

	//actual speed may be limited by the character movement max speed/acceleration
	SpeedX = 0.5f;
	SpeedY = 0.5f;
	SpeedZ = 0.5f;

	//affects camera and character orientation
	SpeedRotation = 1.1f;

	//just affect camera
	SpeedLookUp = 1.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(FName("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	cameraZoom_current = CAMERA_ARM_LENGTH;
	
	// Camera arm rotation, based on its length 
	float angle = CAMERA_ANGLE(cameraZoom_current);
	bUseParabolicZoom = true;
	bEnable3D = true;
	
	//do not consider collisions with world
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->SetWorldRotation(FRotator(angle, 0, 0));
	
	// The camera follows at this distance behind the character
	CameraBoom->TargetArmLength = cameraZoom_current; 

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(FName("FollowCamera"));
	
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); 

	// Don't rotate when the controller rotates. Let that just affect the camera.
	FollowCamera->bUsePawnControlRotation = false;

	OnActorBeginOverlap.AddDynamic(this, &ABrowserCharacter::OnBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &ABrowserCharacter::OnEndOverlap);
}

void ABrowserCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	InputComponent->BindAxis("MoveForward", this, &ABrowserCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ABrowserCharacter::MoveRight);
	InputComponent->BindAxis("MoveUp", this, &ABrowserCharacter::MoveUp);

	InputComponent->BindAction("ZoomIn", IE_Pressed, this, &ABrowserCharacter::CameraZoomIn);
	InputComponent->BindAction("ZoomOut", IE_Pressed, this, &ABrowserCharacter::CameraZoomOut);

	InputComponent->BindAxis("Turn", this, &ABrowserCharacter::Turn);
	InputComponent->BindAxis("LookUp", this, &ABrowserCharacter::LookUp);
}

//------------------------------ CAMERA ZOOM ------------------------------

void ABrowserCharacter::ResetCamera()
{
	SetActorRotation(FRotator::ZeroRotator);
	cameraZoom_current = CAMERA_ARM_LENGTH;
	CameraBoom->TargetArmLength = cameraZoom_current;
	if (bEnable3D) {
		const float angle = CAMERA_ANGLE(cameraZoom_current);
		CameraBoom->SetWorldRotation(FRotator(angle, 0, 0));
	}
	else {
		CameraBoom->SetWorldRotation(FRotator(-90.0f, 0.0f, 0.0f));
	}
	Manager->ForceUpdateZoomWidget(CAMERA_ZOOM_PERCENT(cameraZoom_current));
#ifdef ENABLE_CAMERA_DEBUG_MESSAGES
	DEBUG("Camera values reset.")
#endif
}

void ABrowserCharacter::CameraZoomIn()
{	
	bUseParabolicZoom ? ParabolicCameraZoomIn() : LinearCameraZoomIn();
}

void ABrowserCharacter::CameraZoomOut()
{
	bUseParabolicZoom ? ParabolicCameraZoomOut() : LinearCameraZoomOut();
}

void ABrowserCharacter::CameraZoomTo(float ZoomPercent)
{
	bUseParabolicZoom ? ParabolicCameraZoomTo(ZoomPercent) : LinearCameraZoomTo(ZoomPercent);
}

void ABrowserCharacter::SetParabolicZoomEnable(bool Enable)
{
	bUseParabolicZoom = Enable;
}

bool ABrowserCharacter::IsParabolicZoomEnable()
{
	return bUseParabolicZoom;
}

//--------------------------- PARABOLIC CAMERA ZOOM ---------------------------

void ABrowserCharacter::ParabolicCameraZoomIn(){
	const float angle = (CAMERA_ANGLE(cameraZoom_current));
	cameraZoom_current = FMath::Clamp(cameraZoom_current - CAMERA_ZOOM_STEP, CAMERA_ZOOM_MIN, CAMERA_ZOOM_MAX);
	CameraBoom->TargetArmLength = cameraZoom_current;
	const float delta_angle = (CAMERA_ANGLE(cameraZoom_current)) - angle;
	//const FRotator Rotation = Controller->GetControlRotation();
	CameraBoom->AddRelativeRotation(FRotator(delta_angle, 0.0f, 0.0f));
#ifndef SKIP_ZOOM_WIDGET
	Manager->ForceUpdateZoomWidget(CAMERA_ZOOM_PERCENT(cameraZoom_current));
#endif
#ifdef ENABLE_CAMERA_DEBUG_MESSAGES
	DEBUG("angle should be " + FString::SanitizeFloat(angle));
#endif
}

void ABrowserCharacter::ParabolicCameraZoomOut(){
	const float angle = (CAMERA_ANGLE(cameraZoom_current));
	cameraZoom_current = FMath::Clamp(cameraZoom_current + CAMERA_ZOOM_STEP, CAMERA_ZOOM_MIN, CAMERA_ZOOM_MAX);
	CameraBoom->TargetArmLength = cameraZoom_current;
	const float delta_angle = (CAMERA_ANGLE(cameraZoom_current)) - angle;
	//const FRotator Rotation = Controller->GetControlRotation();
	//CameraBoom->SetWorldRotation(FRotator(Rotation.Pitch - delta_angle, Rotation.Yaw, Rotation.Roll));
	CameraBoom->AddRelativeRotation(FRotator(delta_angle, 0.0f, 0.0f));
#ifndef SKIP_ZOOM_WIDGET
	Manager->ForceUpdateZoomWidget(CAMERA_ZOOM_PERCENT(cameraZoom_current));
#endif
#ifdef ENABLE_CAMERA_DEBUG_MESSAGES
	DEBUG("angle should be " + FString::SanitizeFloat(angle));
#endif
}

void ABrowserCharacter::ParabolicCameraZoomTo(float ZoomPercent)
{
	const float angle = (CAMERA_ANGLE(cameraZoom_current));
	cameraZoom_current = (CAMERA_ZOOM_VALUE(FMath::Clamp(ZoomPercent, 0.0f, 1.0f)));
	const float delta_angle = (CAMERA_ANGLE(cameraZoom_current)) - angle;
	CameraBoom->TargetArmLength = cameraZoom_current;
	//const FRotator Rotation = Controller->GetControlRotation();
	//CameraBoom->SetWorldRotation(FRotator(Rotation.Pitch - delta_angle, Rotation.Yaw, Rotation.Roll));
	CameraBoom->AddRelativeRotation(FRotator(delta_angle, 0.0f, 0.0f));
#ifdef ENABLE_CAMERA_DEBUG_MESSAGES
	DEBUG("angle should be " + FString::SanitizeFloat(angle));
#endif
}


//----------------------------- LINEAR CAMERA ZOOM -----------------------------

void ABrowserCharacter::LinearCameraZoomIn()
{
	cameraZoom_current = FMath::Clamp(cameraZoom_current - CAMERA_ZOOM_STEP, CAMERA_ZOOM_MIN, CAMERA_ZOOM_MAX);
	CameraBoom->TargetArmLength = cameraZoom_current;
#ifndef SKIP_ZOOM_WIDGET
	Manager->ForceUpdateZoomWidget(CAMERA_ZOOM_PERCENT(cameraZoom_current));
#endif
}

void ABrowserCharacter::LinearCameraZoomOut()
{
	cameraZoom_current = FMath::Clamp(cameraZoom_current + CAMERA_ZOOM_STEP, CAMERA_ZOOM_MIN, CAMERA_ZOOM_MAX);
	CameraBoom->TargetArmLength = cameraZoom_current;
#ifndef SKIP_ZOOM_WIDGET
	Manager->ForceUpdateZoomWidget(CAMERA_ZOOM_PERCENT(cameraZoom_current));
#endif
}

void ABrowserCharacter::LinearCameraZoomTo(float ZoomPercent)
{
	cameraZoom_current = (CAMERA_ZOOM_VALUE(FMath::Clamp(ZoomPercent, 0.0f, 1.0f)));
	CameraBoom->TargetArmLength = cameraZoom_current;
}

//------------------------- 2D 3D SWITCH -------------------------

bool ABrowserCharacter::Is3DEnabled()
{
	return bEnable3D;
}

void ABrowserCharacter::Set3DEnabled(bool Enabled)
{
	bEnable3D = Enabled;
	if (!Enabled) {
		SetParabolicZoomEnable(false);
	}
	ResetCamera();
}

//------------------------- AXES CONTROL -------------------------

void ABrowserCharacter::Turn(float Val)
{
	const FRotator R = Controller->GetControlRotation();
	Controller->SetControlRotation(FRotator(R.Pitch, R.Yaw + (Val * SpeedRotation), R.Roll));
}

void ABrowserCharacter::LookUp(float Val)
{
	if (bEnable3D) {
		CameraBoom->AddRelativeRotation(FRotator(-Val * SpeedLookUp, 0.0f, 0.0f));
	}
}

//-------------------------CHARACTER MOVEMEMENT------------------------------

void ABrowserCharacter::MoveForward(float Val)
{
	if ((Val != 0.0f) && (Controller != NULL))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotationMatrix R = FRotationMatrix(FRotator(0, Rotation.Yaw, 0));
		const FVector WorldSpaceAccel = R.GetScaledAxis(EAxis::X);
		AddMovementInput(WorldSpaceAccel, Val * SpeedX * (CHARACTER_MOVEMENT_SPEED(cameraZoom_current)));
	}
}

void ABrowserCharacter::MoveRight(float Val)
{
	if ((Val != 0.0f) && (Controller != NULL))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotationMatrix R = FRotationMatrix(FRotator(0, Rotation.Yaw, 0));
		const FVector WorldSpaceAccel = R.GetScaledAxis(EAxis::Y);
		AddMovementInput(WorldSpaceAccel, Val * SpeedY * (CHARACTER_MOVEMENT_SPEED(cameraZoom_current)));
	}
}

void ABrowserCharacter::MoveUp(float Val)
{
	if ((Val != 0.0f) && (Controller != NULL) && bEnable3D)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotationMatrix R = FRotationMatrix(FRotator(Rotation.Pitch, 0, Rotation.Roll));
		const FVector WorldSpaceAccel = R.GetScaledAxis(EAxis::Z);
		AddMovementInput(WorldSpaceAccel, Val * SpeedZ * (CHARACTER_MOVEMENT_SPEED(cameraZoom_current)));
	}
}

//------------------------- OVERLAP EVENTS ------------------------------

void ABrowserCharacter::OnBeginOverlap(AActor* Other)
{
#ifdef ENABLE_OVERLAP_SELECTION
	/* Cast<> description: 
	 * [efficiency preset] it's the equivalent of C++ line
	 * AMapElementActor* element = dynamic_cast<AMapElementActor*>(Other);
	 * but magically works without turning on RTTI compiler settings.
	 * Pay attention to pointers. */
	AMapElementActor* element = Cast<AMapElementActor>(Other);

	if (element == NULL) {	// element is not a MapElementActor
		return;
	}
	if (Manager) {
		Manager->SelectElement(element->GetElementID());
	}
#endif

#ifdef ENABLE_OVERLAP_DEBUG_MESSAGES
	DEBUG("overlap begin")
#endif
}

void ABrowserCharacter::OnEndOverlap(AActor* Other)
{
#ifdef ENABLE_OVERLAP_SELECTION
	/* equivalent of C++ line
	 * AMapElementActor* element = dynamic_cast<AMapElementActor*>(Other);
	 * but works without turning on RTTI compiler settings.
	 * Pay attention to pointers. */
	AMapElementActor* element = Cast<AMapElementActor>(Other);

	if (element == NULL) {	// element is not a MapElementActor
		return;
	}
	if (Manager) {
		Manager->DeselectElement(element->GetElementID());
	}
#endif
#ifdef ENABLE_OVERLAP_DEBUG_MESSAGES
	DEBUG("overlap end")
#endif
}