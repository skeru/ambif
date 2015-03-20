// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#include "ambif.h"
#include "CustomUtils/DebugUtils.h"
#include "NavigationSystemProxy.h"


ANavigationSystemProxy::ANavigationSystemProxy() {
	Manager = nullptr;
}

void ANavigationSystemProxy::SetManager(TScriptInterface<IBrowserController_i> NewManager)
{
	//Manager = InterfaceCast<IBrowserController_i>(NewManager);
	Manager = Cast<IBrowserController_i>(NewManager.GetObjectRef());
	if (!Manager) {
		DebugUtils::LogString("NavigationSystemProxy::SetManager: given Manager does not implement required interface.");
	}
}

void ANavigationSystemProxy::ResetCamera()
{
	if (Manager) {
		Manager->ResetCamera();
	}
	else {
		DebugUtils::LogString("NavigationSystemProxy: Error. Manager not set.");
	}
}

void ANavigationSystemProxy::CameraZoomIn()
{
	if (Manager) {
		Manager->CameraZoomIn();
	}
	else {
		DebugUtils::LogString("NavigationSystemProxy: Error. Manager not set.");
	}
}

void ANavigationSystemProxy::CameraZoomOut()
{
	if (Manager) {
		Manager->CameraZoomOut();
	}
	else {
		DebugUtils::LogString("NavigationSystemProxy: Error. Manager not set.");
	}
}

void ANavigationSystemProxy::CameraZoomTo(float Percentage)
{
	if (Manager) {
		Manager->CameraZoomTo(Percentage);
	}
	else {
		DebugUtils::LogString("NavigationSystemProxy: Error. Manager not set.");
	}
}

void ANavigationSystemProxy::SetParabolicZoomEnable(bool Enable)
{
	if (Manager) {
		Manager->SetParabolicZoomEnable(Enable);
	}
	else {
		DebugUtils::LogString("NavigationSystemProxy: Error. Manager not set.");
	}
}

bool ANavigationSystemProxy::IsParabolicZoomEnable()
{
	if (Manager) {
		return Manager->IsParabolicZoomEnable();
	}
	else {
		DebugUtils::LogString("NavigationSystemProxy: Error. Manager not set.");
	}
	return true;
}

void ANavigationSystemProxy::Set3DEnabled(bool Enable)
{
	if (Manager) {
		Manager->Set3DEnabled(Enable);
	}
	else {
		DebugUtils::LogString("NavigationSystemProxy: Error. Manager not set.");
	}
}

bool ANavigationSystemProxy::Is3DEnabled()
{
	if (Manager) {
		return Manager->Is3DEnabled();
	}
	else {
		DebugUtils::LogString("NavigationSystemProxy: Error. Manager not set.");
	}
	return true;
}