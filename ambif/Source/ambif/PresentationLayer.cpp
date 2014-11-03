// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#include "ambif.h"
#include "CustomUtils/DebugUtils.h"
#include "stdexcept"
#include "PresentationLayer.h"

#define SAT_LOWER_BOUND 0.5f
//#define PresentationLayer_VERBOSE_MODE

APresentationLayer::APresentationLayer(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	DimensionOnMap = Utils::hashmap<int, _DimensionOnMapInfo>();

	//tick
	//PrimaryActorTick.bCanEverTick = true;

	//elements buffers
	CurrentlySelectedElements = TSet<FString>();
	CurrentlyClickedElements = TSet<FString>();

	//spawner stuff

	MapBound_x = MapBound_y = 0.0f;
	MapBound_X = MapBound_Y = 1.0f;

	MapUsage = 0.95f;
	MinZ = 50.0f;
	MaxZ = 50000.0f;

	DimensionOnMap[PlottableDimension::X].Id = NO_DIMENSION;
	DimensionOnMap[PlottableDimension::Y].Id = NO_DIMENSION;
	DimensionOnMap[PlottableDimension::Z].Id = NO_DIMENSION;

	DimensionOnMap[PlottableDimension::X].Caster = new Caster(ENormalize::Linear);
	DimensionOnMap[PlottableDimension::Y].Caster = new Caster(ENormalize::Linear);

	DefaultSpawnHeigth = 150.0f;

	// update should be called via blueprint to have referenecs properly set
	//UpdateVolumeCasterBounds();
}

//------------------------SPAWNER STUFF-------------------------

void APresentationLayer::SetSpawnTargetBounds(float MinX, float MaxX, float MinY, float MaxY)
{
	DimensionOnMap[PlottableDimension::X].Caster->SetTargetBounds(MinX, MaxX);
	DimensionOnMap[PlottableDimension::Y].Caster->SetTargetBounds(MinY, MaxY);

}

void APresentationLayer::UpdateVolumeCasterBounds()
{
	if (MinZ > MaxZ)
	{
		const float tmp = MinZ;
		MinZ = MaxZ;
		MaxZ = tmp;
	}

	if (Landscape)
	{
		FVector orig, ext;
		Landscape->GetActorBounds(true, orig, ext);

		MapBound_x = orig.X - ((ext.X * MapUsage) / 2);
		MapBound_y = orig.Y - ((ext.Y * MapUsage) / 2);
		MapBound_X = orig.X + ((ext.X * MapUsage) / 2);
		MapBound_Y = orig.Y + ((ext.Y * MapUsage) / 2);

		if (UsedVolume)	//can't really tell if it's working or not
		{
			UsedVolume->SetActorLocation(FVector(orig.X, orig.Y, (MaxZ + MinZ) / 2.0f));
			UsedVolume->SetActorRelativeScale3D(FVector(ext.X, ext.Y, (MaxZ - MinZ)));
		}
		FVector wall_orig, wall_size;
		if (TopBlocker)	//it works. updated at first spawn. need to update also other walls
		{
			TopBlocker->GetActorBounds(true, wall_orig, wall_size);
			TopBlocker->SetActorLocation(FVector(wall_orig.X, wall_orig.Y, MaxZ + (wall_size.Z / 2.0f)));
		}
		const float wall_orig_z = (MinZ > 0) ? (MaxZ / 2.0f) : ((MaxZ - MinZ) / 2.0f) + MinZ;
		const float wall_heigh_z = (MaxZ / 2.0f) + wall_size.Z;

		//actually this macro doesn't do what I expect.
		//problem is to set proper dimension (SetActorScale3D does weird things if used with bounds & extension)
#define UPDATE_INVSIBILE_WALL(Wall) {\
			if((Wall))\
						{\
				FVector _orig, _size;\
				(Wall)->GetActorBounds(true, _orig, _size);\
				(Wall)->SetActorLocation(FVector(_orig.X, _orig.Y, wall_orig_z));\
				(Wall)->SetActorRelativeScale3D(FVector(_size.X, _size.Y, wall_heigh_z));\
						}\
				}
		//UPDATE_INVSIBILE_WALL(EastBlocker);
		//UPDATE_INVSIBILE_WALL(WestBlocker);
		//UPDATE_INVSIBILE_WALL(NorthBlocker);
		//UPDATE_INVSIBILE_WALL(SouthBlocker);

#undef UPDATE_INVSIBILE_WALL

#ifdef PresentationLayer_VERBOSE_MODE
		DebugUtils::LogString(FString("Landscape goes from " + orig.ToString() + " and has extension " + ext.ToString()));
#endif
	}
	else
	{
		DEBUG("Presentation Layer: Can't set Landscape Bounds. Defaults [0;1] used.");
		MapBound_x = 0.0f;
		MapBound_y = 0.0f;
		MapBound_X = 1.0f;
		MapBound_Y = 1.0f;
	}

	DimensionOnMap[PlottableDimension::X].Caster->SetTargetBounds(MapBound_x, MapBound_X);
	DimensionOnMap[PlottableDimension::Y].Caster->SetTargetBounds(MapBound_y, MapBound_Y);
	DimensionOnMap[PlottableDimension::Z].Caster->SetTargetBounds(MinZ, MaxZ);
	DimensionOnMap[PlottableDimension::X].Caster->SetNormalizationType(ENormalize::Linear);
	DimensionOnMap[PlottableDimension::Y].Caster->SetNormalizationType(ENormalize::Linear);
	DimensionOnMap[PlottableDimension::Z].Caster->SetNormalizationType(ENormalize::Linear);
#ifdef PresentationLayer_VERBOSE_MODE
	DebugUtils::LogString(FString("PresentationLayer:: UpdateVolumeCasterBounds target x: ") +
		FString::SanitizeFloat(MapBound_x) + " - " +
		FString::SanitizeFloat(MapBound_X) + " - y: " +
		FString::SanitizeFloat(MapBound_y) + " - " +
		FString::SanitizeFloat(MapBound_Y) + " - z: " +
		FString::SanitizeFloat(MinZ) + " - " + FString::SanitizeFloat(MaxZ));
#endif
}

//------------------------ACTIONS-------------------------

void APresentationLayer::Initialize(FString ViewID)
{
	ViewDetails vd;
	if (!DataAgent->getViewDetails(ViewID, vd)) //load fail
	{
		DEBUG("PresentationLayer::Initialize: view loading failed");
		return;
	}
	DimensionDetails xd, yd, zd;
	if ((!DataAgent->getDimensionDetails(vd.XDimension, xd)) || (!DataAgent->getDimensionDetails(vd.YDimension, yd)))
	{
		DEBUG("PresentationLayer::Initialize: dimension loading failed");
		return;
	}

	DimensionOnMap.clear();
	DimensionOnMap[PlottableDimension::X].Id = xd.Id;
	DimensionOnMap[PlottableDimension::Y].Id = yd.Id;
	DimensionOnMap[PlottableDimension::X].Caster = DataAgent->getCaster(xd);
	DimensionOnMap[PlottableDimension::Y].Caster = DataAgent->getCaster(yd);

	//first load X & Y dimensions (Z is optional)

	//load or default-initialiaze z dimension

	if (!DataAgent->getDimensionDetails(vd.ZDimension, zd))
	{
		zd.Id = vd.Id;
		zd.LowerBound = MinZ;
		zd.UpperBound = MaxZ;
		zd.ReplaceMissingValues = true;
		zd.DefaultValue = DefaultSpawnHeigth;
		zd.OrderAs = FString("Numbers");

#ifdef PresentationLayer_VERBOSE_MODE
		DebugUtils::LogString(FString("Z dimension details not read"));
#endif
	}
	DimensionOnMap[PlottableDimension::Z].Id = zd.Id;
	DimensionOnMap[PlottableDimension::Z].Caster = DataAgent->getCaster(zd);

	//have to wait to know create & set Z caster before updating volume bounds
	UpdateVolumeCasterBounds();

	MapElementsManagerAgent->DeselectAllElements();
	Utils::FHashMap<SongDetails> data = *DataAgent->GetElementMap();

	//---------------------SPAWN ELEEMENTS---------------------

	float element_xd, element_yd, element_zd;
	bool insert_this_element;
	for (auto elem : data)
	{
		insert_this_element = TakeValue(elem.second, xd, DimensionOnMap[PlottableDimension::X].Caster, element_xd);

		if (insert_this_element)
		{
			insert_this_element = TakeValue(elem.second, yd, DimensionOnMap[PlottableDimension::Y].Caster, element_yd);
		}
		if (insert_this_element)
		{
			if (!TakeValue(elem.second, zd, DimensionOnMap[PlottableDimension::Z].Caster, element_zd))
			{	//this should never happen. but just in case...
				element_zd = DefaultSpawnHeigth;

#ifdef PresentationLayer_VERBOSE_MODE
				DebugUtils::LogString(FString("Default value roughly applied to Z"));
#endif
			}
			MapElementsManagerAgent->Spawn(elem.first, element_xd, element_yd, element_zd);
		}
#ifdef PresentationLayer_VERBOSE_MODE
		(insert_this_element) ? DebugUtils::LogString("Spawned") : DebugUtils::LogString("Not Spawned");
#endif
	}//end for

	//--------------------LOAD & UPDATE OTHER DIMENSIONS--------------------

	DimensionDetails dd;
#define LOAD_DIM(FIELD, PLOTTABLE_DIM) {\
	if(!DataAgent->getDimensionDetails((FIELD), dd))\
		{\
			DebugUtils::LogString(FString("Presentation Layer: cannot read anything for ") + dd.Id);\
		}\
		else\
		{\
		DimensionOnMap[(PLOTTABLE_DIM)].Id = dd.Id;\
		DimensionOnMap[(PLOTTABLE_DIM)].Caster = DataAgent->getCaster(dd);\
		}\
	}

	LOAD_DIM(vd.HueDimension, PlottableDimension::Color_Hue);
	LOAD_DIM(vd.SatDimension, PlottableDimension::Color_Sat);
#undef LOAD_DIM

	//On start render every dimension indipendently. GlobalUpdate can be called on update 
	UpdateSingleMapDimension(PlottableDimension::Color_Hue);
	UpdateSingleMapDimension(PlottableDimension::Color_Sat);

	//GlobalUpdateMap();

	//color testing
	//HSVColor foo_color = HSVColor(FLinearColor::Blue);
	//MapElementsManagerAgent->SetColor(FString("001"), foo_color.ToFLinearColor());
}

void APresentationLayer::SetDimension(PlottableDimension::Type DimensionSpace, FString DimensionID, bool Update)
{
	DimensionDetails dd;
	DimensionOnMap[DimensionSpace].Id = DimensionID;
	if (DataAgent->getDimensionDetails(DimensionID, dd))
	{
		DimensionOnMap[DimensionSpace].Caster = DataAgent->getCaster(dd);
	}

	if (Update)
	{
		UpdateSingleMapDimension(DimensionSpace);
	}
}

void APresentationLayer::GlobalUpdateMap()
{
	//update x-y
	UpdateMapXY();
	//update other dimensions
	for (auto d : DimensionOnMap)
	{
		if (d.first != PlottableDimension::X && d.first != PlottableDimension::Y)
		{
			UpdateSingleMapDimension((PlottableDimension::Type) d.first);
		}
	}
}

void APresentationLayer::UpdateMapXY()
{
	DimensionDetails xd, yd;
	if (!DataAgent->getDimensionDetails(DimensionOnMap[PlottableDimension::X].Id, xd) || !DataAgent->getDimensionDetails(DimensionOnMap[PlottableDimension::Y].Id, yd))
	{
		DEBUG("PresentationLayer::UpdateMapXY: Failed to load Dimension Details.");
		return;
	}
	DimensionOnMap[PlottableDimension::X].Caster = DataAgent->getCaster(xd);
	DimensionOnMap[PlottableDimension::Y].Caster = DataAgent->getCaster(yd);
	UpdateVolumeCasterBounds();

	float x_elem_pos, y_elem_pos;
	FString tmp;
	Utils::FHashMap<SongDetails> map = *DataAgent->GetElementMap();
	for (auto el : map)
	{
		//TODO perform existence and format check
		bool conversion_ok;
		conversion_ok = DimensionOnMap[PlottableDimension::X].Caster->Cast(el.second[xd.Id], x_elem_pos);
		conversion_ok = (conversion_ok) ? DimensionOnMap[PlottableDimension::Y].Caster->Cast(el.second[yd.Id], y_elem_pos) : false;
		if (conversion_ok)
		{
			MapElementsManagerAgent->MoveElementTo(el.first, x_elem_pos, y_elem_pos);
		}
#ifdef PresentationLayer_VERBOSE_MODE
		else
		{
			DebugUtils::LogString(FString("PresentationLayer: Failed to load details of element ") + el.first);
		}
#endif
	}
}

void APresentationLayer::UpdateSingleMapDimension(PlottableDimension::Type Dimension)
{
	DimensionDetails dim;
	FString dim_id;
	bool valid;
	float fval;

	try
	{
		dim_id = DimensionOnMap.at(Dimension).Id;
		if (!DataAgent->getDimensionDetails(dim_id, dim))
		{
			DebugUtils::LogString(FString("PresentationLayer:: Update Single Map Dimension:: Failed to load Dimension Details ") + dim_id);
		}
	}
	catch (std::out_of_range exception){ return; }//safety check

	Utils::FHashMap<SongDetails> data = *DataAgent->GetElementMap();

	switch (Dimension)
	{
	case PlottableDimension::X: case PlottableDimension::Y:
		//refresh position
		UpdateMapXY();
		break;
	case PlottableDimension::Color_Hue:
		DimensionOnMap[PlottableDimension::Color_Hue].Caster = DataAgent->getCaster(dim);
		DimensionOnMap[PlottableDimension::Color_Hue].Caster->SetTargetBounds(0.0f, 360.0f);
		DimensionOnMap[PlottableDimension::Color_Hue].Caster->SetNormalizationType(ENormalize::Linear);

		for (auto elem : data)
		{
			valid = TakeValue(elem.second, dim, DimensionOnMap[PlottableDimension::Color_Hue].Caster, fval);
			if (valid)	//update the element using fval
			{
				MapElementsManagerAgent->SetColorHue(elem.first, fval);
			}
#ifdef PresentationLayer_VERBOSE_MODE
			else
			{
				DebugUtils::LogString(FString("PresentationLayer::UpdateColorHue: failed to load color Hue. Invalid value."));
			}
#endif
		}
		break;
	case PlottableDimension::Color_Sat:
		DimensionOnMap[PlottableDimension::Color_Sat].Caster = DataAgent->getCaster(dim);
		DimensionOnMap[PlottableDimension::Color_Sat].Caster->SetTargetBounds(SAT_LOWER_BOUND, 1.0f);
		DimensionOnMap[PlottableDimension::Color_Sat].Caster->SetNormalizationType(ENormalize::Log);

		for (auto elem : data)
		{
			valid = TakeValue(elem.second, dim, DimensionOnMap[PlottableDimension::Color_Sat].Caster, fval);
			if (valid)	//update the element using fval
			{
				MapElementsManagerAgent->SetColorSat(elem.first, fval);
			}
#ifdef PresentationLayer_VERBOSE_MODE
			else
			{
				DebugUtils::LogString(FString("PresentationLayer::UpdateColorSat: failed to load color saturation. Invalid value."));
			}
#endif
		}
		break;
	default:
		break;
	}
}

inline void APresentationLayer::ShowElements(TArray<FString> ElementIDs)
{
	MapElementsManagerAgent->SetVisibleToSet(ElementIDs, true);
}

inline void APresentationLayer::ApplySelectionToElement(FString ElementID)
{
	MapElementsManagerAgent->SelectElement(ElementID);
}

inline void APresentationLayer::RemoveSelectionFromElement(FString ElementID)
{
	MapElementsManagerAgent->DeselectElement(ElementID);
}


//----------------------------PRIVATE STUFF----------------------------
inline bool APresentationLayer::TakeValue(FSongDetails elem, DimensionDetails dim, Caster* c, float& OutputValue)
{
	FString val;
	bool valid;
	float fval;
	try
	{
		val = elem.Properties.at(dim.Id).SummaryValue;
		valid = true;
	}
	catch (std::out_of_range do_not_panic){//carry a towel
		valid = false; //element not found
	}
	if (!valid && !dim.ReplaceMissingValues)
	{
#ifdef PresentationLayer_VERBOSE_MODE
		DebugUtils::LogString(FString("PresentationLayer::TakeValue: didn't find a value for element ") + elem.Id);
#endif
		return false;
	}

	if (!valid)	//use default value
	{
		fval = c->CastFloat(dim.DefaultValue);
		valid = true;
#ifdef PresentationLayer_VERBOSE_MODE
		DebugUtils::LogString(FString("PresentationLayer::TakeValue: recovered default value for element ") + elem.Id);
#endif
	}
	else
	{	//value successfully read
		valid = c->Cast(val, fval);
#ifdef PresentationLayer_VERBOSE_MODE
		DebugUtils::LogString(FString("PresentationLayer::TakeValue: casted numeric value for element ") +
			elem.Id + FString(" -> value: ") + FString::SanitizeFloat(fval));
#endif		
	}
	OutputValue = fval;
	return valid;
}

//-----------------------------------TICK-----------------------------------
//deprecated
void APresentationLayer::Tick(float DeltaTime)
{
	//move this method to Logic Controller
	Super::Tick(DeltaTime);
	DetectSelectedActors();
	DetectClickedActors();
}

//deprecated
void APresentationLayer::DetectSelectedActors()
{
	TSet<FString> detected_set = MapElementsManagerAgent->CollectTickSelectedElements();
	CurrentlySelectedElements = CurrentlySelectedElements.Union(detected_set);
	TSet<FString> removed_set = MapElementsManagerAgent->CollectTickDeselectedElements();
	CurrentlySelectedElements = CurrentlySelectedElements.Difference(removed_set);

#ifdef PresentationLayer_VERBOSE_MODE
	for (auto elem : detected_set)
		DebugUtils::LogString(FString("Presentation Layer: Detected mouse over ") + elem);
	for (auto elem : removed_set)
		DebugUtils::LogString(FString("Presentation Layer: Detected mouse out of ") + elem);
#endif
	//TODO apply selection / deselection to elements
}

//deprecated
void APresentationLayer::DetectClickedActors()
{
	TSet<FString> detected_new = MapElementsManagerAgent->CollectTickClickedElements();
	TSet<FString> diff_set = CurrentlyClickedElements.Difference(detected_new);
	CurrentlyClickedElements = detected_new;

	//TODO apply clik to elements
}