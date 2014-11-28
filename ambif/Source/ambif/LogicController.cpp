// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#include "ambif.h"
#include "CustomUtils/DebugUtils.h"
#include "LogicController.h"

//#define LogicController_VERBOSE_MODE

ALogicController::ALogicController(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	ViewList_cache = Utils::FHashMap<ViewDetails>();

	//tick
	PrimaryActorTick.bCanEverTick = true;

	//elements buffers
	CurrentlySelectedElements = TSet<FString>();
	CurrentlyClickedElements = TSet<FString>();
}

//---------------MUSIC PLAYER FUNCTIONS---------------

void ALogicController::MusicPlay()
{
	if (MusicPlayer)
	{
		MusicPlayer->Play();
	}
}

void ALogicController::MusicPause()
{
	if (MusicPlayer)
	{
		MusicPlayer->Pause();
	}
}

void ALogicController::MusicStop()
{
	if (MusicPlayer)
	{
		MusicPlayer->Stop();
	}
}

//------------------SPAWN FUNCTIONS------------------

void ALogicController::Spawn()
{

	PresentationLayer->Initialize(FString("Rnd"));//TODO change & use something parametrical
}

//------------------DATA FUNCTIONS-------------------

void ALogicController::ReadData()
{
	if (DataAgent)
	{
		//DataAgent->LoadCSVData();
		DataAgent->LoadOldWayCSVData();
#ifdef LogicController_VERBOSE_MODE
		DataAgent->LogData();
#endif
	}
}

void ALogicController::RefreshViewList()
{
	ViewList_cache = *DataAgent->GetViewMap();
	if (ViewList_cache.empty())
	{
		return;
	}

	//plot out views
}

//-------------------MAP ACTIONS--------------------

void ALogicController::SelectElement(FString ElementID)
{
	PresentationLayer->ApplySelectionToElement(ElementID);

}

void ALogicController::DeselectElement(FString ElementID)
{
	PresentationLayer->RemoveSelectionFromElement(ElementID);
}

//----------------------TICK-----------------------
void ALogicController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DetectClickedActors();
	DetectSelectedActors();
}

void ALogicController::DetectSelectedActors()
{
	TSet<FString> detected_set = MapElementsAgent->CollectTickSelectedElements();
	CurrentlySelectedElements = CurrentlySelectedElements.Union(detected_set);
	TSet<FString> removed_set = MapElementsAgent->CollectTickDeselectedElements();
	CurrentlySelectedElements = CurrentlySelectedElements.Difference(removed_set);

#ifdef LogicController_VERBOSE_MODE
	for (auto elem : detected_set)
		DebugUtils::LogString(FString("LogicController: Detected mouse over ") + elem);
	for (auto elem : removed_set)
		DebugUtils::LogString(FString("LogicController: Detected mouse out of ") + elem);
#endif
	
	for (auto elem : detected_set)
	{
		SelectElement(elem);
	}
	for (auto elem : removed_set)
	{
		DeselectElement(elem);
	}
}

void ALogicController::DetectClickedActors()
{
	TSet<FString> detected_new = MapElementsAgent->CollectTickClickedElements();
	TSet<FString> diff_set = CurrentlyClickedElements.Difference(detected_new);
	CurrentlyClickedElements = detected_new;

	//TODO apply clik to elements
	if (detected_new.Num() > 0)
	{
		FString id = detected_new.Array()[0];
		FString metadata_str = ReadMetadata(id);
		WidgetManager->UpdateMetadataWid(metadata_str);
#ifdef LogicController_VERBOSE_MODE
		DebugUtils::LogString("identified click id: " + id);
#endif
	}

}

//--------------------PRIVATE STUFF--------------------

FString ALogicController::ReadMetadata(FString elementID)
{
	FSongDetails details;
	return (DataAgent->getSongDetails(elementID, details)) ? details.Name + " - " + details.Artist : "no data retrieved";
}