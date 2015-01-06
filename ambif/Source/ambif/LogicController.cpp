// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#include "ambif.h"
#include "CustomUtils/DebugUtils.h"
#include "CustomUtils/CustomUtils.h"
#include "LogicController.h"

//#define LogicController_VERBOSE_MODE

ALogicController::ALogicController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ViewList_cache = Utils::FHashMap<ViewDetails>();

	//tick
	PrimaryActorTick.bCanEverTick = true;

	//elements buffers
	CurrentlySelectedElements = TSet<FString>();
	CurrentlyClickedElements = TSet<FString>();
}

//---------------MUSIC PLAYER FUNCTIONS---------------

void ALogicController::LoadElement(FString ElementID)
{
	SongDetails d;
	if (MusicPlayer && DataAgent->getElementDetails(ElementID, d))
	{
		if (Utils::FileExists(d.Path))
		{
			MusicPlayer->Load(d.Path);
			WidgetManager->UpdateMusicPlayerWidget(false, false); //load stops music
#ifdef LogicController_VERBOSE_MODE
			DebugUtils::LogString("LogicController: Loaded song");
		}
		else
		{
			DebugUtils::LogString("LogicController: file not found fot element " + ElementID);
		}
	}
	else
	{
		DebugUtils::LogString("LogicController: unable to load element to " + ElementID + " on music player.");
	}
#else
		}
	}
#endif
}

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

TArray<FString> ALogicController::GetDimensionList()
{
	return DataAgent->GetDimensionIdList();
}

FString ALogicController::GetDimensionDescription(FString DimensionId)
{
	DimensionDetails tmp;
	if (DataAgent->getDimensionDetails(DimensionId, tmp))
	{
		return tmp.Description;
	}
	return FString(NO_DIMENSION_FOUND);
}

TArray<FString> ALogicController::GetPlottableDimensionList()
{
	TArray<FString> tmp = TArray<FString>();
	TArray<TEnumAsByte<PlottableDimension::Type>> typed_list = APresentationLayer::GetAvailablePlottableDimensions();
	for (auto elem : typed_list)
	{
		tmp.Add(APresentationLayer::ToString(elem));
	}
	return tmp;
}

void ALogicController::SetDimension(PlottableDimension::Type DimensionSpace, FString DimensionID, bool UpdateNow)
{
	PresentationLayer->SetDimension(DimensionSpace, DimensionID, UpdateNow);
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

void ALogicController::UpdateDimensionsOnMap()
{
	PresentationLayer->GlobalUpdateMap();
}
//----------------------GUI-----------------------

void ALogicController::UpdateWidgetInterface()
{
	WidgetManager->UpdateDimensionWidegt();
}

void ALogicController::ForceUpdateZoomWidget(float NewZoomPercentage)
{
	WidgetManager->ForceUpdateZoomWidget(NewZoomPercentage);
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
		LoadElement(id);
	}

}

//--------------------PRIVATE STUFF--------------------

FString ALogicController::ReadMetadata(FString elementID)
{
	FSongDetails details;
	return (DataAgent->getElementDetails(elementID, details)) ? details.Name + " - " + details.Artist : "no data retrieved";
}