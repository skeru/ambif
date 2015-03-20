// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#include "ambif.h"
#include "WidgetManager_i.h"


AWidgetManager_i::AWidgetManager_i() {

}

void AWidgetManager_i::ApplyView(FViewDetails view)
{
	SetSelectedDimension(PlottableDimension::X, view.XDimension);
	SetSelectedDimension(PlottableDimension::Y, view.YDimension);
	SetSelectedDimension(PlottableDimension::Z, view.ZDimension);
	SetSelectedDimension(PlottableDimension::Color_Hue, view.HueDimension);
	SetSelectedDimension(PlottableDimension::Color_Sat, view.SatDimension);

}