// Stefano Cherubin stefano1.cherubin@mail.polimi.it

#include "ambif.h"
#include "CustomUtils/DebugUtils.h"
#include "MusicPlayerActor.h"

//#define MUSIC_PLAYER_ACTOR_VERBOSE

AMusicPlayerActor::AMusicPlayerActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FileName = "My Song.ogg";

	sw = (USoundWave*)StaticConstructObject(USoundWave::StaticClass(), this, TEXT("MyTestSoundWave"));
	ac = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("audio_component"));
	device = GEngine ? GEngine->GetAudioDevice() : NULL; //gently ask for the audio device

	sw->SoundGroup = ESoundGroup::SOUNDGROUP_Music;
	ac->bIsUISound = true;
	ac->bIsMusic = true;
	ac->bAutoActivate = false;

	loaded = false;
	isPlaying = false;
	isPaused = false;
	playTime = 0.0f;

	//tick
	PrimaryActorTick.bCanEverTick = true;
}


inline void AMusicPlayerActor::Load(FString NewFileName = "")
{
	if (NewFileName != "")
	{
		FileName = NewFileName;
	}
	Stop();
	
	rawFile.Empty();
	
	loaded = FFileHelper::LoadFileToArray(rawFile, FileName.GetCharArray().GetData());

	if (loaded){
		FByteBulkData* bulkData = &sw->CompressedFormatData.GetFormat(TEXT("OGG"));
		bulkData->Lock(LOCK_READ_WRITE);
		FMemory::Memcpy(bulkData->Realloc(rawFile.Num()), rawFile.GetData(), rawFile.Num());
		bulkData->Unlock();
		fillSoundWaveInfo(sw, &rawFile);
		ac->SetSound(sw);
#ifdef MUSIC_PLAYER_ACTOR_VERBOSE
		DEBUG("loaded");
	}
	else
	{
		DebugUtils::LogString("MusicPlayerActor: can't load song file.");
#endif
	}
}

void AMusicPlayerActor::Play()
{
	if (!loaded){
		Load();
	}
	if (!loaded){
		return;
	}

	isPlaying = true;
	if (isPaused) {
		audioSource->Play();
		isPaused = false;
	}
	else {
		ac->Play();
	}
}

void AMusicPlayerActor::Pause()
{
	int status = findSource(sw);
	//Debug("Surce location returned " + FString::FromInt(status));

	if (audioSource) {
		audioSource->Pause();
		isPaused = true;
		isPlaying = false;
	}
}

void AMusicPlayerActor::Stop()
{
	ac->Stop();
	isPaused = false;
	isPlaying = false;
	playTime = 0.0f;
	if (audioSource)
	{
		audioSource->Stop();
	}
}

bool AMusicPlayerActor::IsPaused()
{
	return isPaused;
}

bool AMusicPlayerActor::IsLoaded()
{
	return loaded;
}

bool AMusicPlayerActor::IsPlaying()
{
	return isPlaying;
}

double AMusicPlayerActor::GetPlayTime()
{
	return playTime;
}

//--------------------------------private stuff--------------------------------

int AMusicPlayerActor::fillSoundWaveInfo(USoundWave* sw, TArray<uint8>* rawFile)
{
	FSoundQualityInfo info;
	FVorbisAudioInfo vorbis_obj = FVorbisAudioInfo();
	if (!vorbis_obj.ReadCompressedInfo(rawFile->GetData(), rawFile->Num(), &info))
	{
		//Debug("Can't load header");
		return 1;
	}
	sw->NumChannels = info.NumChannels;
	sw->Duration = info.Duration;
	sw->RawPCMDataSize = info.SampleDataSize;
	sw->SampleRate = info.SampleRate;
	return 0;
}

int AMusicPlayerActor::findSource(USoundWave* sw)
{
	if (!device)
	{
		activeSound = NULL;
		audioSource = NULL;
		return -1;
	}
	TArray<FActiveSound*> tmpActualSounds = device->GetActiveSounds();
	if (tmpActualSounds.Num()){
		//for every sound in game (there shouldn't be many)
		for (auto activeSoundIt(tmpActualSounds.CreateIterator()); activeSoundIt; ++activeSoundIt){
			activeSound = *activeSoundIt;
			//for every wave contained in this sound source
			for (auto WaveInstanceIt(activeSound->WaveInstances.CreateIterator()); WaveInstanceIt; ++WaveInstanceIt){
				sw_instance = WaveInstanceIt.Value();
				//if contains same data (same track)
				if (sw_instance->WaveData->CompressedDataGuid == sw->CompressedDataGuid){
					audioSource = device->WaveInstanceSourceMap.FindRef(sw_instance);
					return 0;
				}
			}
		}
	}
	audioSource = NULL;
	activeSound = NULL;
	return -2;
}

//----------------------TICK-----------------------

void AMusicPlayerActor::Tick(float DeltaTime)
{
	if (ac->IsPlaying() && isPlaying)
	{
		playTime += (double) DeltaTime;
	}
	else if (isPlaying && playTime > DeltaTime)
	{	//song just ended (playTime > DeltaTime is needed to avoid first tick)
		Stop();
#ifdef MUSIC_PLAYER_ACTOR_VERBOSE
		DebugUtils::LogString("MusicPlayerActor: detected song end");
#endif
	}
}