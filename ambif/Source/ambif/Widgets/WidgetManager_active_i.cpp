// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#include "ambif.h"
#include "WidgetManager_active_i.h"


AWidgetManager_active_i::AWidgetManager_active_i(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}

//--------------------MUSIC PLAYER CONTROLS-------------------------
inline void AWidgetManager_active_i::Play()
{
	LogicController->MusicPlay();
}

inline void AWidgetManager_active_i::Pause()
{
	LogicController->MusicPause();
}

inline void AWidgetManager_active_i::Stop()
{
	LogicController->MusicStop();
}

