// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#include "ambif.h"
#include "CustomUtils/DebugUtils.h"
#include "MapElementActor.h"

//#define MapElementActor_DEBUG_COLORS
//#define MapElemensActor_DEBUG_EVENTS

AMapElementActor::AMapElementActor(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	ElementId = "";
	Mesh = PCIP.CreateAbstractDefaultSubobject<UStaticMeshComponent>(this, "ball mesh");
	static ConstructorHelpers::FObjectFinder <UStaticMesh>StaticMesh(TEXT(_ElementAsset_Ball));
	if (!StaticMesh.Object)
	{
		DebugUtils::LogString(FString("Element Ball Static Mesh not found"));
	}
	else
	{
		Mesh->SetStaticMesh(StaticMesh.Object);
	}
	Mesh->SetRelativeScale3D(FVector(25, 25, 25));
	RootComponent = Mesh;

	_material = UMaterialInstanceDynamic::Create(Mesh->GetMaterial(0), this);
	_color = FLinearColor::Red;
	_material->SetVectorParameterValue(FName(_ParamName_color), _color);

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

void AMapElementActor::SetElementID(FString NewElementID)
{
	ElementId = NewElementID;
}

FString AMapElementActor::GetElementID()
{
	return ElementId;
}

void AMapElementActor::SetColor(FColor NewColor)
{
	_material->SetVectorParameterValue(FName(_ParamName_color), NewColor);
#ifdef MapElementActor_DEBUG_COLORS
	DebugUtils::LogString(FString("Element::SetColor: R: ") + FString::SanitizeFloat(NewColor.R) + FString(" - G: ") + FString::SanitizeFloat(NewColor.G) + FString(" - B: ") + FString::SanitizeFloat(NewColor.B));
#endif
	Mesh->SetMaterial(0, _material);
}

FLinearColor AMapElementActor::GetColor()
{
	_material->GetVectorParameterValue(FName(_ParamName_color), _color);
#ifdef MapElementActor_DEBUG_COLORS
	DebugUtils::LogString(FString("Element::GetColor: R: ") + FString::SanitizeFloat(_color.R) + FString(" - G: ") + FString::SanitizeFloat(_color.G) + FString(" - B: ") + FString::SanitizeFloat(_color.B));
#endif
	return _color;
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