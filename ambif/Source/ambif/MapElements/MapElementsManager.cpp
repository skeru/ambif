// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#include "ambif.h"
#include "stdexcept"
#include "CustomUtils/DebugUtils.h"
#include "MapElementsManager.h"

//#define BIND_HSVCOLOR_VALUE_TO_SATURATION
//#define MapElementsManager_VERBOSE_MODE

AMapElementsManager::AMapElementsManager(const class FPostConstructInitializeProperties& PCIP) : Super(PCIP)
{
	ActorMap = Utils::FHashMap<AMapElementActor*>();

	ThisTickSelectedElements = TSet<FString>();
	ThisTickDeselectedElements = TSet<FString>();
	ThisTickClickedElements = TSet<FString>();

	static ConstructorHelpers::FObjectFinder<UBlueprint> MapElementBP(TEXT("Blueprint'/Game/Blueprints/MapElementActor_BP.MapElementActor_BP'"));
	if (!MapElementBP.Object)
	{
		DebugUtils::LogString(FString("Map Element Blueprint not found"));
	}
	else
	{
		ActorClass = MapElementBP.Object->GeneratedClass;
	}

	//PrimaryActorTick.bCanEverTick = true;	//nope!
}

inline void AMapElementsManager::AddElement(FString ElementID, AMapElementActor* Element)
{
	ActorMap[ElementID] = Element;
}

inline void AMapElementsManager::SetVisible(FString ElementID, bool bVisible)
{
	try
	{
		ActorMap.at(ElementID);//exception if ElementID not valid
		ActorMap[ElementID]->SetActorHiddenInGame(bVisible);
	}
	catch (const std::out_of_range i_care_really_much)
	{
		//desert dust ball.gif
	}
}

void AMapElementsManager::SetVisibleToSet(TArray<FString> ElementsID, bool bVisible)
{
	for (TArray<FString>::TConstIterator iter = ElementsID.CreateConstIterator(); iter; ++iter)
	{
		SetVisible(*iter, bVisible);
	}
}

void AMapElementsManager::SetVisibleToAll(bool bVisible)
{
	//TODO (maybe) parallelize
	//for (Utils::FHashMap<AMapElementActor*>::iterator iter = ActorMap.begin(); iter != ActorMap.end(); ++iter)
	for (auto iter : ActorMap)
	{
		iter.second->SetActorHiddenInGame(bVisible);
	}
}

inline void AMapElementsManager::SetColor(FString ElementID, FColor NewColor) //TODO test
{
	try
	{
		ActorMap.at(ElementID);//exception if ElementID not valid
		ActorMap[ElementID]->SetColor(NewColor);
	}
	catch (const std::out_of_range i_care_really_much)
	{
		DebugUtils::LogString(FString("MapElementsManager::SetColor: ElementID not found"));
		//desert dust ball.gif
	}
}

inline void AMapElementsManager::SetColorHue(FString ElementID, float Hue)
{
	HSVColor NewColor;
	try
	{
		ActorMap.at(ElementID);//exception if ElementID not valid
		NewColor = HSVColor(ActorMap[ElementID]->GetColor());
		NewColor.SetH(Hue);
		ActorMap[ElementID]->SetColor(NewColor.ToFLinearColor());

	}
	catch (const std::out_of_range i_care_really_much)
	{
		DebugUtils::LogString(FString("MapElementsManager::SetColor: ElementID not found"));
		//desert dust ball.gif
	}
}

inline void AMapElementsManager::SetColorSat(FString ElementID, float Sat)
{
	HSVColor NewColor;
	try
	{
		ActorMap.at(ElementID);//exception if ElementID not valid
		NewColor = HSVColor(ActorMap[ElementID]->GetColor());
		NewColor.SetS(Sat);
#ifdef BIND_HSVCOLOR_VALUE_TO_SATURATION
		NewColor.SetV(0.5f + (Sat / 2.0f));
#endif
		ActorMap[ElementID]->SetColor(NewColor.ToFLinearColor());
	}
	catch (const std::out_of_range i_care_really_much)
	{
		DebugUtils::LogString(FString("MapElementsManager::SetColor: ElementID not found"));
		//desert dust ball.gif
	}
}

void AMapElementsManager::MoveElementTo(FString ElementID, float x, float y)
{
	//TODO implement
	DebugUtils::LogString(FString("MapElementsManager::MoveElementTo: not implemented method called for element ") + ElementID);
}

//-------------------------ELEMENTS MANAGING-------------------------

void AMapElementsManager::SelectElement(FString ElementID)
{
	//CurrentlySelectedElements.Add(ElementID);
}

void AMapElementsManager::DeselectElement(FString ElementID)
{
	/*
	if (CurrentlySelectedElements.Contains(ElementID))
	{
	CurrentlySelectedElements.Remove(ElementID);
	}*/
}

void AMapElementsManager::DeselectAllElements()
{
	/*
	for (auto a : CurrentlySelectedElements)
	{
	DeselectElement(a);
	}*/
}
//------------------------SPAWNER STUFF-------------------------

//deprecated
void AMapElementsManager::SpawnExactlyAt(AActor* actor, FVector2D location, float DefaultSpawnHeigth)//used only for debug purpose
{
	GetWorld()->SpawnActor<AMapElementActor>(ActorClass, FVector(location, DefaultSpawnHeigth), FRotator(0, 0, 0));
}

AMapElementActor* AMapElementsManager::Spawn(FString ElementID, float x, float y, float z)
{
	return Spawn(ElementID, x, y, z, ActorClass);
}

AMapElementActor* AMapElementsManager::Spawn(FString ElementID, float x, float y, float z, UClass* ActorClass)
{
	AMapElementActor* tmp;
	tmp = GetWorld()->SpawnActor<AMapElementActor>(ActorClass, FVector(x, y, z), FRotator(0, 0, 0));
	tmp->SetElementID(ElementID);
	tmp->RegisterElementListener(this);
	ActorMap[ElementID] = tmp;
	return tmp;
}

//------------------------------TICK------------------------------
//deprecated
void AMapElementsManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/* //element collection should be performed by the presentation layer
	CollectTickSelectedElements();
	CollectTickDeselectedElements();
	CollectTickClickedElements();*/

	//DebugUtils::LogString(FString("MapElementsManager::Tick: something")); //ok, it works
}

//--------------------------------MOUSE EVENTS--------------------------------

void AMapElementsManager::OnMouseBeginHover(AActor* TouchedComponent)
{
	AMapElementActor* element = Cast<AMapElementActor>(TouchedComponent);
	if (element)
	{
		if (ThisTickDeselectedElements.Contains(element->GetElementID()))
		{
			ThisTickDeselectedElements.Remove(element->GetElementID());
		}
		else if (!ThisTickSelectedElements.Contains(element->GetElementID()))
		{
			ThisTickSelectedElements.Add(element->GetElementID());
		}
	}//null check end if
}

void AMapElementsManager::OnMouseEndHover(AActor* TouchedComponent)
{
	AMapElementActor* element = Cast<AMapElementActor>(TouchedComponent);
	if (element)
	{
		if (ThisTickSelectedElements.Contains(element->GetElementID()))
		{
			ThisTickSelectedElements.Remove(element->GetElementID());
		}
		else
		{
			ThisTickDeselectedElements.Add(element->GetElementID());
		}
	}//null check end if
}

void AMapElementsManager::OnMouseClick(AActor* TouchedComponent)
{
	AMapElementActor* element = Cast<AMapElementActor>(TouchedComponent);
	if (element && !ThisTickClickedElements.Contains(element->GetElementID()))
	{
		ThisTickClickedElements.Add(element->GetElementID());
	}
}

//--------------------------------EVENTS COLLECTION--------------------------------

TSet<FString> AMapElementsManager::CollectTickSelectedElements()
{
	TSet<FString> aux;
	aux = ThisTickSelectedElements;
	ThisTickSelectedElements = TSet<FString>();
	return aux;
}

TSet<FString> AMapElementsManager::CollectTickDeselectedElements()
{
	TSet<FString> aux;
	aux = ThisTickDeselectedElements;
	ThisTickDeselectedElements = TSet<FString>();
	return aux;
}

TSet<FString> AMapElementsManager::CollectTickClickedElements()
{
	TSet<FString> aux;
	aux = ThisTickClickedElements;
	ThisTickClickedElements = TSet<FString>();
	return aux;
}
