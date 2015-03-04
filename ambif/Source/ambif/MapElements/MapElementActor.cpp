// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#include "ambif.h"
#include "CustomUtils/DebugUtils.h"
#include "MapElementActor.h"

//#define MapElementActor_DEBUG_COLORS
//#define MapElemensActor_DEBUG_EVENTS
//#define _INTERNAL_READ_COLOR
#define MapElemensActor_GHOST_TRANSLUCENT

AMapElementActor::AMapElementActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ElementId = "";
	RootScene = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, "RootScene");
	RootComponent = RootScene;
	Mesh = ObjectInitializer.CreateAbstractDefaultSubobject<UStaticMeshComponent>(this, "ball mesh");
	static ConstructorHelpers::FObjectFinder <UStaticMesh>StaticMesh(TEXT(_ElementAsset_Ball));
	Mesh->AttachTo(RootComponent);
	if (!StaticMesh.Object) {
		DebugUtils::LogString(FString("Element Ball Static Mesh not found"));
	}
	else {
		Mesh->SetStaticMesh(StaticMesh.Object);
	}
	Mesh->SetRelativeScale3D(FVector(_scale_normal, _scale_normal, _scale_normal));
	_material = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(0), this);
	_color = FLinearColor::Red;
	_material->SetVectorParameterValue(FName(_ParamName_color), _color);

	_color_backup = FLinearColor::White;
	_activeColorIndex = 0;
	AvailableColorsNum = 2;

	//Mesh->SetMaterial(0, Material.Object);
	//static ConstructorHelpers::FObjectFinder <UMaterialInterface>BallMaterial(TEXT(_ElementAsset_BallMaterial));

	//collection of listeners
	Managers = TArray<MapElementActorListener*>();

	//mouse events callback function registration
	//!!! apply to Actor, not to Mesh. Otherwise got problem not firing BeginCursorOver
	OnClicked.AddDynamic(this, &AMapElementActor::NotifyMouseClick);
	OnEndCursorOver.AddDynamic(this, &AMapElementActor::NotifyMouseEndOver);
	OnBeginCursorOver.AddDynamic(this, &AMapElementActor::NotifyMouseBeginOver);

	Mesh->SetEnableGravity(false);
}

//---------------------------- MOVE ----------------------------
void AMapElementActor::MoveTo(float x, float y, float z)
{
	//const FVector actual_pos = Mesh->RelativeLocation;
	//const FVector actual_pos = RootComponent->RelativeLocation;
	//no hit check, only translate root component
	//RootComponent->MoveComponent(FVector(x, y, z) - actual_pos, FRotator(0, 0, 0), false);
	//Mesh->MoveComponent(FVector(x, y, z) - actual_pos, FRotator(0, 0, 0), false);
	this->SetActorLocation(FVector(x, y, z));
	//RootComponent->MoveComponent(FVector(x, y, z) - actual_pos, FRotator(0, 0, 0), false);
	//DebugUtils::LogString("MapElementActor::MoveTo: movement called. Moving " + ElementId + " to " + FString::SanitizeFloat(x) + " " + FString::SanitizeFloat(y) + " " + FString::SanitizeFloat(z));
}

//------------------- ELEMENT STATE ACCESSORS ------------------
void AMapElementActor::SetElementID(FString NewElementID)
{
	ElementId = NewElementID;
}

FString AMapElementActor::GetElementID()
{
	return ElementId;
}

void AMapElementActor::Ghost(bool IsGhost)
{
	//transparency
#ifdef MapElemensActor_GHOST_TRANSLUCENT
	float s;
	if (IsGhost) {
		_color.A = _color_alpha_ghost;
		_color_backup.A = _color_alpha_ghost;
		s = _scale_ghost;
	}
	else {
		_color.A = _color_alpha_normal;
		_color_backup.A = _color_alpha_normal;
		s = _scale_normal;
	}
	ApplyColor(_activeColorIndex);
	Mesh->SetRelativeScale3D(FVector(s, s, s));
#else
	Mesh->SetVisibility(!IsGhost);
#endif
}

FLinearColor AMapElementActor::GetColor()
{
#ifdef _INTERNAL_READ_COLOR
	_material->GetVectorParameterValue(FName(_ParamName_color), _color);
#endif
#ifdef MapElementActor_DEBUG_COLORS
	DebugUtils::LogString(FString("Element::GetColor: R: ") + FString::SanitizeFloat(_color.R) + FString(" - G: ") + FString::SanitizeFloat(_color.G) + FString(" - B: ") + FString::SanitizeFloat(_color.B));
#endif

#ifdef _INTERNAL_READ_COLOR
#undef _INTERNAL_READ_COLOR
#endif
	return _color;
}

void AMapElementActor::SetColor(FColor NewColor)
{
	_color = NewColor;
	_applyColor(NewColor);
}

void AMapElementActor::SetBgColor(FColor NewColor)
{
	_color_backup = NewColor;
}

void AMapElementActor::_applyColor(FColor NewColor)
{
	_material->SetVectorParameterValue(FName(_ParamName_color), NewColor);
#ifdef MapElementActor_DEBUG_COLORS
	DebugUtils::LogString(FString("Element::SetColor: R: ") + FString::SanitizeFloat(NewColor.R) + FString(" - G: ") + FString::SanitizeFloat(NewColor.G) + FString(" - B: ") + FString::SanitizeFloat(NewColor.B));
#endif
	Mesh->SetMaterial(0, _material);
}

void AMapElementActor::ToggleColor()
{
	FLinearColor tmp = _color_backup;
	_color_backup = _color;
	_color = tmp;
	_applyColor(tmp);
}

uint32 AMapElementActor::GetAvailableColorNum()
{
	return AvailableColorsNum;
}

void AMapElementActor::ApplyColor(uint32 ColorIndex)
{
	_applyColor((ColorIndex % AvailableColorsNum == 0) ? _color : _color_backup);
}

//------------------------ MOUSE INTERACTION ------------------------

void AMapElementActor::RegisterElementListener(MapElementActorListener* Manager)
{
	if (!Managers.Contains(Manager))
	{
#ifdef MapElemensActor_DEBUG_EVENTS
		DebugUtils::LogString(FString("MapElementActor:: Listener Registered")); //ok
#endif
		Managers.Add(Manager);
	}
}

void AMapElementActor::UnregisterElementListener(MapElementActorListener* Manager)
{
	if (Managers.Contains(Manager))
	{
		Managers.Remove(Manager);
	}
}

void AMapElementActor::NotifyMouseBeginOver()
{
#ifdef MapElemensActor_DEBUG_EVENTS
	DebugUtils::LogString(FString("MapElementActor:: Notification received - begin over")); // ok
#endif

	for (auto callback : Managers)
	{
#ifdef MapElemensActor_DEBUG_EVENTS
		DebugUtils::LogString(FString("MapElementActor:: Notification thrown - begin over")); // ok
#endif
		callback->OnMouseBeginHover(this);
	}
}

void AMapElementActor::NotifyMouseEndOver()
{
	for (auto callback : Managers)
	{
#ifdef MapElemensActor_DEBUG_EVENTS
		DebugUtils::LogString(FString("MapElementActor:: Notification thrown - end over")); // ok
#endif
		callback->OnMouseEndHover(this);
	}
}

void AMapElementActor::NotifyMouseClick()
{
	for (auto callback : Managers)
	{
#ifdef MapElemensActor_DEBUG_EVENTS
		DebugUtils::LogString(FString("MapElementActor:: Notification thrown - click")); // ok
#endif
		callback->OnMouseClick(this);
	}
}