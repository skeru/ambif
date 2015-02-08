// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#include "ambif.h"
#include "CustomUtils/DebugUtils.h"
#include "PresentationLayer.h"

#define SAT_LOWER_BOUND 0.5f
#define ENABLE_DIMENSION_COLOR_SAT
//#define ENABLE_DIMENSION_SIZE
//#define PresentationLayer_VERBOSE_MODE

//------------------------PLOTTABLE DIMENSION------------------------
FString APresentationLayer::ToString(PlottableDimension::Type var)
{
	switch (var)
	{
	case PlottableDimension::X:
		return "X";
	case PlottableDimension::Y:
		return "Y";
	case PlottableDimension::Z:
		return "Z";
	case PlottableDimension::Color_Hue:
		return "Color Hue";
	case PlottableDimension::Color_Sat:
		return "Color Saturation";
	case PlottableDimension::Size:
		return "Size";
	default:
		return "";
	}
}

bool APresentationLayer::ToPlottableDimension(FString var, TEnumAsByte<PlottableDimension::Type>& output)
{
	if (var.Equals("X"))
	{
		output = PlottableDimension::X;
		return true;
	}
	if (var.Equals("Y"))
	{
		output = PlottableDimension::Y;
		return true;
	}
	if (var.Equals("Z"))
	{
		output = PlottableDimension::Z;
		return true;
	}
	if (var.Equals("Color Hue"))
	{
		output = PlottableDimension::Color_Hue;
		return true;
	}
	if (var.Equals("Color Saturation"))
	{
		output = PlottableDimension::Color_Sat;
		return true;
	}
	if (var.Equals("Size"))
	{
		output = PlottableDimension::Size;
		return true;
	}
	return false;
}

TArray<TEnumAsByte<PlottableDimension::Type>> APresentationLayer::GetAvailablePlottableDimensions()
{
	TArray<TEnumAsByte<PlottableDimension::Type>> tmp = TArray<TEnumAsByte<PlottableDimension::Type>>();
	tmp.Add(PlottableDimension::X);
	tmp.Add(PlottableDimension::Y);
	tmp.Add(PlottableDimension::Z);
	tmp.Add(PlottableDimension::Color_Hue);
#ifdef ENABLE_DIMENSION_COLOR_SAT
	tmp.Add(PlottableDimension::Color_Sat);
#endif
#ifdef ENABLE_DIMENSION_SIZE
	tmp.Add(PlottableDimension::Size);
#endif
	return tmp;
}

//------------------------PRESENTATION LAYER-----------------------------
APresentationLayer::APresentationLayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DimensionOnMap = Utils::hashmap<PlottableDimension::Type, _DimensionOnMapInfo>();

	//tick
	//PrimaryActorTick.bCanEverTick = true;

	//elements buffers
	CurrentlySelectedElements = TSet<FString>();
	CurrentlyClickedElements = TSet<FString>();

	//no dimension position

	NoDimensionX = 150.0f;
	NoDimensionY = 150.0f;
	NoDimensionZ = 150.0f;
	NoDimensionColorHue = 0.0f;
	NoDimensionColorSat = 1.0f;
	
	LandscapeBounds_orig = FVector::ZeroVector;
	LandscapeBounds_ext = FVector::ZeroVector;

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

	bEnable3D = true;

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

	UpdateLandscapeBounds();

	if (Landscape)
	{
		if (UsedVolume)	{ //can't really tell if it's working or not
			UsedVolume->SetActorLocation(FVector(LandscapeBounds_orig.X, LandscapeBounds_orig.Y, (MaxZ + MinZ) / 2.0f));
			UsedVolume->SetActorRelativeScale3D(FVector(LandscapeBounds_ext.X, LandscapeBounds_ext.Y, (MaxZ - MinZ)));
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
		DebugUtils::LogString(FString("Landscape goes from " + LandscapeBounds_orig.ToString() + " and has extension " + LandscapeBounds_ext.ToString()));
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

	UpdateCasterX();
	UpdateCasterY();
	UpdateCasterZ();

#ifdef PresentationLayer_VERBOSE_MODE
	DebugUtils::LogString(FString("PresentationLayer:: UpdateVolumeCasterBounds target x: ") +
		FString::SanitizeFloat(MapBound_x) + " - " +
		FString::SanitizeFloat(MapBound_X) + " - y: " +
		FString::SanitizeFloat(MapBound_y) + " - " +
		FString::SanitizeFloat(MapBound_Y) + " - z: " +
		FString::SanitizeFloat(MinZ) + " - " + FString::SanitizeFloat(MaxZ));
#endif
}

FString APresentationLayer::ElementToString(FSongDetails element)
{
	FString s = "";
	s += element.Name;
	s += " - ";
	s += element.Artist;
	return s;
}

void APresentationLayer::UpdateLandscapeBounds()
{
	if (Landscape && LandscapeBounds_ext == FVector::ZeroVector) {
		Landscape->GetActorBounds(true, LandscapeBounds_orig, LandscapeBounds_ext);
		MapBound_x = LandscapeBounds_orig.X - ((LandscapeBounds_ext.X * MapUsage) / 2);
		MapBound_y = LandscapeBounds_orig.Y - ((LandscapeBounds_ext.Y * MapUsage) / 2);
		MapBound_X = LandscapeBounds_orig.X + ((LandscapeBounds_ext.X * MapUsage) / 2);
		MapBound_Y = LandscapeBounds_orig.Y + ((LandscapeBounds_ext.Y * MapUsage) / 2);
	}
}

void APresentationLayer::UpdateCasterX()
{
	DimensionOnMap[PlottableDimension::X].Caster->SetTargetBounds(MapBound_x, MapBound_X);
	DimensionOnMap[PlottableDimension::X].Caster->SetNormalizationType(ENormalize::Linear);
}

void APresentationLayer::UpdateCasterY()
{
	DimensionOnMap[PlottableDimension::Y].Caster->SetTargetBounds(MapBound_y, MapBound_Y);
	DimensionOnMap[PlottableDimension::Y].Caster->SetNormalizationType(ENormalize::Linear);
}

void APresentationLayer::UpdateCasterZ()
{
	DimensionOnMap[PlottableDimension::Z].Caster->SetTargetBounds(MinZ, MaxZ);
	DimensionOnMap[PlottableDimension::Z].Caster->SetNormalizationType(ENormalize::Linear);
}

void APresentationLayer::UpdateCasterColorHue()
{
	DimensionOnMap[PlottableDimension::Color_Hue].Caster->SetTargetBounds(0.0f, 360.0f);
	DimensionOnMap[PlottableDimension::Color_Hue].Caster->SetNormalizationType(ENormalize::Linear);
}

void APresentationLayer::UpdateCasterColorSat()
{
	DimensionOnMap[PlottableDimension::Color_Sat].Caster->SetTargetBounds(SAT_LOWER_BOUND, 1.0f);
	DimensionOnMap[PlottableDimension::Color_Sat].Caster->SetNormalizationType(ENormalize::Log);
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
}

void APresentationLayer::SetEnable3D(bool Enable3D)
{
	if (!Enable3D) {
		DimensionOnMap[PlottableDimension::Z].Id = NO_DIMENSION;
		UpdateMapXYZ();
	}
	bEnable3D = Enable3D;
}

void APresentationLayer::SetDimension(PlottableDimension::Type DimensionSpace, FString DimensionID, bool Update)
{
	DimensionDetails dd;
	DimensionOnMap[DimensionSpace].Id = DimensionID;
	if (DataAgent->getDimensionDetails(DimensionID, dd)) {
		DimensionOnMap[DimensionSpace].Caster = DataAgent->getCaster(dd);
	}

	if (Update) {
		UpdateSingleMapDimension(DimensionSpace);
	}
}

void APresentationLayer::GlobalUpdateMap(float CurrentTime, FString ElementID)
{
	//update x-y-z
	UpdateMapXYZ(CurrentTime, ElementID);

	//update other dimensions
	for (auto d : DimensionOnMap) {
		if (d.first != PlottableDimension::X && d.first != PlottableDimension::Y && d.first != PlottableDimension::Z) {
			UpdateSingleMapDimension(d.first, CurrentTime, ElementID);
		}
	}
}

void APresentationLayer::UpdateMapXYZ(float CurrentTime, FString ElementID)
{
	DimensionDetails xd, yd, zd;
	if (!DataAgent->getDimensionDetails(DimensionOnMap[PlottableDimension::X].Id, xd) 
		|| !DataAgent->getDimensionDetails(DimensionOnMap[PlottableDimension::Y].Id, yd) 
		|| !DataAgent->getDimensionDetails(DimensionOnMap[PlottableDimension::Z].Id, zd))
	{
		DebugUtils::LogString("PresentationLayer::UpdateMapXYZ: Failed to load Dimension Details.");
		return;
	}

	//retieve or create Caster objects (input bounds set)
	DimensionOnMap[PlottableDimension::X].Caster = DataAgent->getCaster(xd);
	DimensionOnMap[PlottableDimension::Y].Caster = DataAgent->getCaster(yd);
	DimensionOnMap[PlottableDimension::Z].Caster = DataAgent->getCaster(zd);

	//updates Caster objects (recompute x, y, z output bounds)
	UpdateVolumeCasterBounds();

	float x_elem_pos, y_elem_pos, z_elem_pos;
	bool conversion_ok;

	if (ElementID == "") {		//update every element
		Utils::FHashMap<SongDetails> map = *DataAgent->GetElementMap();
		for (auto el : map) {
			if (DimensionOnMap[PlottableDimension::X].Id == NO_DIMENSION) {
				x_elem_pos = NoDimensionX;
				conversion_ok = true;
			} else {
				UpdateCasterX();
				conversion_ok = DimensionOnMap[PlottableDimension::X].Caster->Cast(el.second.Properties[xd.Id][CurrentTime], x_elem_pos);
			}

			if (DimensionOnMap[PlottableDimension::Y].Id == NO_DIMENSION) {
				y_elem_pos = NoDimensionY;
			} else {
				UpdateCasterY();
				conversion_ok = (conversion_ok) ? DimensionOnMap[PlottableDimension::Y].Caster->Cast(el.second.Properties[yd.Id][CurrentTime], y_elem_pos) : false;
			}

			if (DimensionOnMap[PlottableDimension::Z].Id == NO_DIMENSION) {
				z_elem_pos = NoDimensionZ;
			} else {
				UpdateCasterZ();
				conversion_ok = (conversion_ok) ? DimensionOnMap[PlottableDimension::Z].Caster->Cast(el.second.Properties[zd.Id][CurrentTime], z_elem_pos) : false;
			}

			if (conversion_ok) {
				MapElementsManagerAgent->MoveElementTo(el.first, x_elem_pos, y_elem_pos, z_elem_pos);
			}
#ifdef PresentationLayer_VERBOSE_MODE
			else {
				DebugUtils::LogString(FString("PresentationLayer: Failed to load details of element ") + el.first);
			}
#endif
		}	//end for each element of map
	}		//end global update if

	else {	//update only ElementID
		FSongDetails elem;
		conversion_ok = DataAgent->getElementDetails(ElementID, elem);
		if (conversion_ok) {	//element found
			if (DimensionOnMap[PlottableDimension::X].Id == NO_DIMENSION) {
				x_elem_pos = NoDimensionX;
				conversion_ok = true;
			}
			else {
				UpdateCasterX();
				conversion_ok = DimensionOnMap[PlottableDimension::X].Caster->Cast(elem.Properties[xd.Id][CurrentTime], x_elem_pos);
			}

			if (DimensionOnMap[PlottableDimension::Y].Id == NO_DIMENSION) {
				y_elem_pos = NoDimensionY;
			}
			else {
				UpdateCasterY();
				conversion_ok = (conversion_ok) ? DimensionOnMap[PlottableDimension::Y].Caster->Cast(elem.Properties[yd.Id][CurrentTime], y_elem_pos) : false;
			}

			if (DimensionOnMap[PlottableDimension::Z].Id == NO_DIMENSION) {
				z_elem_pos = NoDimensionZ;
			}
			else {
				UpdateCasterZ();
				conversion_ok = (conversion_ok) ? DimensionOnMap[PlottableDimension::Z].Caster->Cast(elem.Properties[zd.Id][CurrentTime], z_elem_pos) : false;
			}
			
			if (conversion_ok) {
				MapElementsManagerAgent->MoveElementTo(ElementID, x_elem_pos, y_elem_pos, z_elem_pos);
			}
#ifdef PresentationLayer_VERBOSE_MODE
			else {	//element values failed to load
				DebugUtils::LogString(FString("PresentationLayer: Failed to load details of element ") + ElementID);
			}
		}
		else {	//element not found
			DebugUtils::LogString("PresentationLayer::UpdateMapXYZ: Failed to find element " + ElementID);
#endif
		}
	}		//end update only ElementID else case
}

void APresentationLayer::UpdateSingleMapDimension(PlottableDimension::Type Dimension, float CurrentTime, FString ElementID)
{
	DimensionDetails dim;
	FString dim_id;

	if (Utils::HMContains(DimensionOnMap, Dimension)) {
		dim_id = DimensionOnMap.at(Dimension).Id;
		if (!DataAgent->getDimensionDetails(dim_id, dim)) {
			DebugUtils::LogString(FString("PresentationLayer:: Update Single Map Dimension:: Failed to load Dimension Details ") + dim_id);
		}
	}
	else{ //safety check
		DebugUtils::LogString("PresentationLayer::UpdateSingleMapDimension: Tried to update unknown PlottableDimension.");
		return; 
	}

	switch (Dimension)
	{
	case PlottableDimension::X: case PlottableDimension::Y: case PlottableDimension::Z:
		//refresh position
		UpdateMapXYZ(CurrentTime, ElementID);
		break;
	case PlottableDimension::Color_Hue:
		UpdateColorHue(dim, CurrentTime, ElementID);
		break;
	case PlottableDimension::Color_Sat:
		UpdateColorSat(dim, CurrentTime, ElementID);
		break;
	default:
		break;
	}
}

inline void APresentationLayer::HighlightElements(TArray<FString> ElementIDs)
{
	MapElementsManagerAgent->Highlight(ElementIDs);
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
inline bool APresentationLayer::TakeValue(FSongDetails elem, DimensionDetails dim, Caster* c, float& OutputValue, float time)
{
	FString val;	//FString value representation
	bool valid;		//value found
	float fval;		//float value
	try {
		val = elem.Properties.at(dim.Id)[time];//.SummaryValue;
		valid = true;
	}
	catch (std::out_of_range do_not_panic) {//carry a towel
		valid = false; //element not found
	}
	if (!valid && !dim.ReplaceMissingValues) {
#ifdef PresentationLayer_VERBOSE_MODE
		DebugUtils::LogString(FString("PresentationLayer::TakeValue: didn't find a value for element ") + elem.Id);
#endif
		return false;
	}

	if (!valid)	{	//use default value
		fval = c->CastFloat(dim.DefaultValue);
		valid = true;
#ifdef PresentationLayer_VERBOSE_MODE
		DebugUtils::LogString(FString("PresentationLayer::TakeValue: recovered default value for element ") + elem.Id);
#endif
	}
	else {	//value successfully read
		valid = c->Cast(val, fval);
#ifdef PresentationLayer_VERBOSE_MODE
		DebugUtils::LogString(FString("PresentationLayer::TakeValue: casted numeric value for element ") +
			elem.Id + FString(" -> value: ") + FString::SanitizeFloat(fval));
#endif		
	}
	OutputValue = fval;
	return valid;
}


//update single plottable dimensions
void APresentationLayer::UpdateColorHue(const DimensionDetails dim, float CurrentTime, FString ElementID)
{
	bool valid;
	float fval;

	DimensionOnMap[PlottableDimension::Color_Hue].Caster = DataAgent->getCaster(dim);
	UpdateCasterColorHue();

	if (ElementID != "") {	//update only one element
		SongDetails elem;
		valid = DataAgent->getElementDetails(ElementID, elem);
		if (DimensionOnMap[PlottableDimension::Color_Hue].Id == NO_DIMENSION) {	//default value
			fval = NoDimensionColorHue;
		}
		else {				//update from a database dimension
			valid = valid && TakeValue(elem, dim, DimensionOnMap[PlottableDimension::Color_Hue].Caster, fval, CurrentTime);
		}

		if (valid) {
			MapElementsManagerAgent->SetColorHue(ElementID, fval);
		}
#ifdef PresentationLayer_VERBOSE_MODE
		else {
			DebugUtils::LogString(FString("PresentationLayer::UpdateColorHue: failed update element ") + ElementID);
		}
#endif
	}
	else {					//update every element on map
		const Utils::FHashMap<SongDetails> data = *DataAgent->GetElementMap();
		if (DimensionOnMap[PlottableDimension::Color_Hue].Id == NO_DIMENSION) {	//default value
			for (auto elem : data) {
				fval = NoDimensionColorHue;
				MapElementsManagerAgent->SetColorHue(elem.first, fval);
			}
		} else {				//update from a database dimension
			for (auto elem : data) {
				valid = TakeValue(elem.second, dim, DimensionOnMap[PlottableDimension::Color_Hue].Caster, fval, CurrentTime);
				if (valid) {	//update the element using fval
					MapElementsManagerAgent->SetColorHue(elem.first, fval);
				}
#ifdef PresentationLayer_VERBOSE_MODE
				else {
					DebugUtils::LogString(FString("PresentationLayer::UpdateColorHue: failed to load color Hue. Invalid value."));
				}
#endif
			}	//end for each element
		}		//end update from db dimension
	}			//end update every element
}

void APresentationLayer::UpdateColorSat(const DimensionDetails dim, float CurrentTime, FString ElementID)
{
	bool valid;
	float fval;

	DimensionOnMap[PlottableDimension::Color_Sat].Caster = DataAgent->getCaster(dim);
	UpdateCasterColorSat();

	if (ElementID != "") {		//update only one element
		SongDetails elem;
		valid = DataAgent->getElementDetails(ElementID, elem);
		if (DimensionOnMap[PlottableDimension::Color_Sat].Id == NO_DIMENSION) { //default value
			fval = NoDimensionColorSat;
		} else {				//update from a database dimension
			valid = valid && TakeValue(elem, dim, DimensionOnMap[PlottableDimension::Color_Sat].Caster, fval, CurrentTime);
		}

		if (valid) {
			MapElementsManagerAgent->SetColorSat(ElementID, fval);
		}
#ifdef PresentationLayer_VERBOSE_MODE
		else {
			DebugUtils::LogString(FString("PresentationLayer::UpdateColorSat: failed update element ") + ElementID);
		}
#endif
	}
	else {						//update every element on map
		const Utils::FHashMap<SongDetails> data = *DataAgent->GetElementMap();
		if (DimensionOnMap[PlottableDimension::Color_Sat].Id == NO_DIMENSION) { //default value
			for (auto elem : data) {
				fval = NoDimensionColorSat;
				MapElementsManagerAgent->SetColorSat(elem.first, fval);
			}
		} else {				//update from a database dimension
			for (auto elem : data) {
				valid = TakeValue(elem.second, dim, DimensionOnMap[PlottableDimension::Color_Sat].Caster, fval, CurrentTime);
				if (valid) {	//update the element using fval
					MapElementsManagerAgent->SetColorSat(elem.first, fval);
				}
#ifdef PresentationLayer_VERBOSE_MODE
				else {
					DebugUtils::LogString(FString("PresentationLayer::UpdateColorSat: failed to load color Sat. Invalid value."));
				}
#endif
			}	//end for each element
		}		//end update from db dimension
	}			//end update every element
}