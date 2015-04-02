//* Copyright 2015 Stefano Cherubin
//* stefano1{dot}cherubin{at}mail{dot}polimi{dot}it
//* ISPG - Image and Sound Processing Group
//* Politecnico di Milano

#include "ambif.h"
#include "WidgetManager_i.h"


AWidgetManager_i::AWidgetManager_i() {
	PrimaryActorTick.bCanEverTick = true;
	ApplyViewQueue = TArray<std::pair<int32, ViewDetails>>();
}

void AWidgetManager_i::ApplyView(FViewDetails view)
{
	SetSelectedDimension(PlottableDimension::X, view.XDimension);
	SetSelectedDimension(PlottableDimension::Y, view.YDimension);
	SetSelectedDimension(PlottableDimension::Z, view.ZDimension);
	SetSelectedDimension(PlottableDimension::Color_Hue, view.HueDimension);
	SetSelectedDimension(PlottableDimension::Color_Sat, view.SatDimension);

}

void AWidgetManager_i::Tick(float DeltaTime)
{
	bool done = false;
	for (int p = 0; p < ApplyViewQueue.Num(); p++) {
		ApplyViewQueue[p].first = ApplyViewQueue[p].first - 1;
		if (ApplyViewQueue[p].first <= 0) {
			ApplyView(ApplyViewQueue[p].second);
			done = true;
		}
	}
	if (done) {	//if any applied, empty the queue
		ApplyViewQueue.Empty();
	}
}

void AWidgetManager_i::EnqueueApplyView(FViewDetails Details, int32 DeferBy)
{
	std::pair<int32, FViewDetails> p;
	p.first = DeferBy;
	p.second = Details;
	ApplyViewQueue.Add(p);
}