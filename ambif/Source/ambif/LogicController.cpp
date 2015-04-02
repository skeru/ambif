//* Copyright 2015 Stefano Cherubin
//* stefano1{dot}cherubin{at}mail{dot}polimi{dot}it
//* ISPG - Image and Sound Processing Group
//* Politecnico di Milano

#include "ambif.h"
#include "CustomUtils/DebugUtils.h"
#include "CustomUtils/CustomUtils.h"
#include "LogicController.h"

//#define LogicController_VERBOSE_MODE
//#define LogicController_VERBOSE_MODE_LOGALL
#define LogicController_HIDE_ON_PLAY

ALogicController::ALogicController() {
	ViewList_cache = Utils::FHashMap<ViewDetails>();

	//tick
	PrimaryActorTick.bCanEverTick = true;

	//elements buffers
	CurrentlySelectedElements = TSet<FString>();
	CurrentlyClickedElements = TSet<FString>();
	LastClickedElementID = "";

	DeltaTimeElementUpdate = 0.0f;
}

//---------------MUSIC PLAYER FUNCTIONS---------------

void ALogicController::LoadElement(FString ElementID)
{
	SongDetails d;
	if (MusicPlayer && DataAgent->getElementDetails(ElementID, d)) {
		if (Utils::FileExists(d.Path)) {
			MusicPlayer->Load(d.Path);
			WidgetManager->UpdateMusicPlayerWidget(false, false); //load stops music

			lastLoadedElement = ElementID;
#ifdef LogicController_VERBOSE_MODE
			DebugUtils::LogString("LogicController: Loaded song");
		}
		else {
			DebugUtils::LogString("LogicController: file not found for element " + ElementID + " - path: " + d.Path);
		}
	}
	else {
		DebugUtils::LogString("LogicController: unable to load element to " + ElementID + " on music player.");
	}
#else
		}
	}
#endif
}

void ALogicController::MusicPlay()
{
	if (LastClickedElementID != lastLoadedElement) {
		LoadElement(LastClickedElementID);
	}
	if (MusicPlayer) {
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

void ALogicController::Spawn(FString ViewID)
{
	PresentationLayer->Initialize(ViewID);
	lastLoadedView = ViewID;
}

//------------------DATA FUNCTIONS-------------------

void ALogicController::ReadData()
{
	if (DataAgent)
	{
		//DataAgent->LoadCSVData();
		DataAgent->LoadOldWayCSVData();
#ifdef LogicController_VERBOSE_MODE_LOGALL
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
	TArray<FString> tmp = DataAgent->GetDimensionIdList();
	tmp.Add(NO_DIMENSION);
	return tmp;
}

FString ALogicController::GetDimensionDescription(FString DimensionId)
{
	DimensionDetails tmp;
	if (DataAgent->getDimensionDetails(DimensionId, tmp)) {
		return tmp.Description;
	}
	
	if (DimensionId == NO_DIMENSION) {
		return NO_DIMENSION_DESCR;
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

void ALogicController::SetEnable3DMode(bool Enable)
{
	PresentationLayer->SetEnable3D(Enable);
	WidgetManager->ForceUpdateParabolicLinear();
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

void ALogicController::UpdateWidgetInterface(FString SynchronizeWithViewID)
{
	WidgetManager->UpdateDimensionWidegt();
	if (SynchronizeWithViewID != "") {
		ViewDetails d;
		if (DataAgent->getViewDetails(SynchronizeWithViewID, d)) {
			WidgetManager->EnqueueApplyView(d);
		}
	}
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

	//element update
	DeltaTimeElementUpdate += DeltaTime;
	if (DeltaTimeElementUpdate >= ElmentUpdateGranularity)
	{
		DeltaTimeElementUpdate = 0.0f;
		HandleElement();
	}
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
	
	for (auto elem : detected_set) {
		SelectElement(elem);
	}
	for (auto elem : removed_set) {
		DeselectElement(elem);
	}

	//tooltip
	FString toolTip = "";
	FSongDetails d;
	for (auto elem : CurrentlySelectedElements) {
		if (DataAgent->getElementDetails(elem, d)) {	//should always be ok, but I'll check anyway
			if (toolTip != "") {	//not first element - add nice element separator
				toolTip += " | ";
			}
			toolTip += APresentationLayer::ElementToString(d);
		}
	}
	ShowToolTip(toolTip);
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
		LastClickedElementID = id;
	}

}

void ALogicController::HandleElement()
{
	if (MusicPlayer && MusicPlayer->IsPlaying()) {	//update only current playing element
		const double currentTime = MusicPlayer->GetPlayTime();
		TArray<FString> tmp = TArray<FString>();
		tmp.Add(LastClickedElementID);
		PresentationLayer->GlobalUpdateMap(currentTime, LastClickedElementID);
		//DebugUtils::LogString("HandleElement: " + LastClickedElementID);
#ifdef LogicController_HIDE_ON_PLAY
		PresentationLayer->HighlightElements(tmp);
	}
	else {
		PresentationLayer->HighlightElements(TArray<FString>());
#endif
	}
}

//--------------------PRIVATE STUFF--------------------

FString ALogicController::ReadMetadata(FString elementID)
{
	FSongDetails details;
	return (DataAgent->getElementDetails(elementID, details)) ? details.Name + " - " + details.Artist : "no data retrieved";
}

void ALogicController::ShowToolTip(FString Message)
{
	if (!TooltipHUD)
		return;
	TooltipHUD->SetText(Message);
}