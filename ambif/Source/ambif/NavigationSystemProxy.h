// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#pragma once

#include "GameFramework/Actor.h"
#include "BrowserController_i.h"
#include "NavigationSystemProxy.generated.h"

/**
 * 
 */
UCLASS()
class AMBIF_API ANavigationSystemProxy : public AActor
{
	GENERATED_BODY()

	ANavigationSystemProxy();

private:
	IBrowserController_i* Manager;

public:
	UFUNCTION(BlueprintCallable, Category = "Camera Settings")
	void SetManager(TScriptInterface<IBrowserController_i> NewManager);

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

	UFUNCTION(BlueprintCallable, Category = "Camera Management")
	/** Enable / Disable 3D navigation mode. */
	void Set3DEnabled(bool Enable);

	UFUNCTION(BlueprintCallable, Category = "Camera Management")
	/** Returns true if and only if 3D navigation mode is enabled. */
	bool Is3DEnabled();
};
	
