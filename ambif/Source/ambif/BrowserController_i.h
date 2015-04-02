//* Copyright 2015 Stefano Cherubin
//* stefano1{dot}cherubin{at}mail{dot}polimi{dot}it
//* ISPG - Image and Sound Processing Group
//* Politecnico di Milano

#pragma once

#include "BrowserController_i.generated.h"
/** Just an interface to avoid circular references when changing camera settings. */
UINTERFACE()
class UBrowserController_i : public UInterface
{
	//GENERATED_BODY()
	GENERATED_UINTERFACE_BODY()
};

class IBrowserController_i 
{
	GENERATED_IINTERFACE_BODY()
public:
	/** Reset Camera to default values */
	virtual void ResetCamera() {};

	/** Camera zoom step in */
	virtual void CameraZoomIn() {};

	/** Camera zoom step out */
	virtual void CameraZoomOut() {};

	/** Camera zoom set to value.
	 * Zoom 0 means close to object.
	 *
	 * Does not update Widget Zoom
	 * (could cause loop due to circular reference)
	 * (I know it's shitty by design but quickly just works) */
	virtual void CameraZoomTo(float ZoomPercent) {};

	/** Activate / Deactivate parabolic zoom.
	 *
	 * Parabolic zoom quadratically increase view angle when getting closer. */
	virtual void SetParabolicZoomEnable(bool Enable) {};
	
	/** Returns true if and only if parabolic zoom is in use. */
	virtual bool IsParabolicZoomEnable() {
		check(0 && "This function must be implemented");
		return true;
	};

	/** Returns true if and only if 3D navigation mode is enabled. */
	virtual bool Is3DEnabled() {
		check(0 && "This function must be implemented");
		return true;
	};

	/** Enable / Disable 3D navigation mode. */
	virtual void Set3DEnabled(bool Enabled) {};
};

//copy this line in every class that implements this interface
//just because otherwise interfaces in 4.7 make UHT crash badly
//UBrowserController_i::UBrowserController_i(const FObjectInitializer & ObjectInitializer) : Super(ObjectInitializer) {};
