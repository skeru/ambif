//* Copyright 2015 Stefano Cherubin
//* stefano1{dot}cherubin{at}mail{dot}polimi{dot}it
//* ISPG - Image and Sound Processing Group
//* Politecnico di Milano

#include "ambif.h"
#include "CustomUtils/CustomUtils.h"
#include "CustomUtils/DebugUtils.h"
#include "MapElementsManager.h"

//#define BIND_HSVCOLOR_VALUE_TO_SATURATION
//#define MapElementsManager_VERBOSE_MODE

AMapElementsManager::AMapElementsManager() {
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

//-------------------------ELEMENTS MANAGING-------------------------

inline void AMapElementsManager::AddElement(FString ElementID, AMapElementActor* Element)
{
	ActorMap[ElementID] = Element;
}

inline void AMapElementsManager::SetVisible(FString ElementID, bool bVisible)
{
	if (Utils::FHMContains(ActorMap, ElementID))
	{
		ActorMap[ElementID]->SetActorHiddenInGame(bVisible);
	}
	else
	{
		DebugUtils::LogString(FString("MapElementsManager::SetVisible: ElementID not found"));
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

void AMapElementsManager::Highlight(TArray<FString> ElementsIDs)
{
	if (!ElementsIDs.Num()) {	//restore all to normal
		for (auto a : ActorMap)
			a.second->Ghost(false);
		//DebugUtils::LogString("MapElementManager::Highlight: restore all to normal");
	}
	else {	//ghost to all but ElementIDs
		for (auto a : ActorMap) {
			//if found set ghost = false, if not found set ghost = true
			/*if (ElementsIDs.Contains(a.first)) {
				a.second->Ghost(false);
				DebugUtils::LogString("MapElementManager::Highlight: highlight element " + a.first);
			}
			else {
				a.second->Ghost();
			}*/
			a.second->Ghost(!ElementsIDs.Contains(a.first));
		}
	}
}

inline void AMapElementsManager::SetColor(FString ElementID, FColor NewColor) //TODO test
{
	if (Utils::FHMContains(ActorMap, ElementID))
	{
		ActorMap[ElementID]->SetColor(NewColor);
	}
	else
	{
		DebugUtils::LogString(FString("MapElementsManager::SetColor: ElementID not found"));
	}
}

inline void AMapElementsManager::SetColorHue(FString ElementID, float Hue)
{
	HSVColor NewColor;
	if (Utils::FHMContains(ActorMap, ElementID))
	{
		NewColor = HSVColor(ActorMap[ElementID]->GetColor());
		NewColor.SetH(Hue);
		ActorMap[ElementID]->SetColor(NewColor.ToFLinearColor());
	}
	else
	{
		DebugUtils::LogString(FString("MapElementsManager::SetColor: ElementID not found"));
	}
}

inline void AMapElementsManager::SetColorSat(FString ElementID, float Sat)
{
	HSVColor NewColor;
	if (Utils::FHMContains(ActorMap, ElementID))
	{
		NewColor = HSVColor(ActorMap[ElementID]->GetColor());
		NewColor.SetS(Sat);
#ifdef BIND_HSVCOLOR_VALUE_TO_SATURATION
		NewColor.SetV(0.5f + (Sat / 2.0f));
#endif
		ActorMap[ElementID]->SetColor(NewColor.ToFLinearColor());
	}
	else
	{
		DebugUtils::LogString(FString("MapElementsManager::SetColor: ElementID not found"));
	}
}

void AMapElementsManager::MoveElementTo(FString ElementID, float x, float y, float z)
{
	if (Utils::FHMContains(ActorMap, ElementID))
	{
		ActorMap[ElementID]->MoveTo(x, y, z);
	}
	else
	{
		DebugUtils::LogString(FString("MapElementsManager::MoveElementTo: ElementID not found"));
	}
	//DebugUtils::LogString(FString("MapElementsManager::MoveElementTo: not implemented method called for element ") + ElementID);
}

void AMapElementsManager::SelectElement(FString ElementID)
{
	ActorMap[ElementID]->ApplyColor(1);
}

void AMapElementsManager::DeselectElement(FString ElementID)
{
	ActorMap[ElementID]->ApplyColor(0);
}

void AMapElementsManager::DeselectAllElements()
{
	for (auto a : ActorMap)
	{
		DeselectElement(a.first);
	}
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
}

//--------------------------------MOUSE EVENTS--------------------------------

void AMapElementsManager::OnMouseBeginHover(AActor* TouchedComponent)
{
	AMapElementActor* element = Cast<AMapElementActor>(TouchedComponent);
	if (element) {
		if (ThisTickDeselectedElements.Contains(element->GetElementID())) {
			ThisTickDeselectedElements.Remove(element->GetElementID());
		}
		else if (!ThisTickSelectedElements.Contains(element->GetElementID())) {
			ThisTickSelectedElements.Add(element->GetElementID());
		}
	}//null check end if
}

void AMapElementsManager::OnMouseEndHover(AActor* TouchedComponent)
{
	AMapElementActor* element = Cast<AMapElementActor>(TouchedComponent);
	if (element) {
		if (ThisTickSelectedElements.Contains(element->GetElementID())) {
			ThisTickSelectedElements.Remove(element->GetElementID());
		}
		else {
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
#ifdef MapElementsManager_VERBOSE_MODE
		DebugUtils::LogString("click detected");
#endif
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
