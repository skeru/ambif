// Stefano Cherubin stefano1.cherubin@mail.polimi.it

#include "ambif.h"
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
}


inline void AMusicPlayerActor::Load(FString NewFileName = "")
{
	if (NewFileName != "")
	{
		FileName = NewFileName;
		Stop();
	}
	rawFile.Empty();
	loaded = FFileHelper::LoadFileToArray(rawFile, FileName.GetCharArray().GetData());

	if (loaded){
#ifdef MUSIC_PLAYER_ACTOR_VERBOSE
		DEBUG("loaded");
#endif
		FByteBulkData* bulkData = &sw->CompressedFormatData.GetFormat(TEXT("OGG"));
		bulkData->Lock(LOCK_READ_WRITE);
		FMemory::Memcpy(bulkData->Realloc(rawFile.Num()), rawFile.GetData(), rawFile.Num());
		bulkData->Unlock();
		fillSoundWaveInfo(sw, &rawFile);
		ac->SetSound(sw);
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

	if (isPaused){
		audioSource->Play();
		isPaused = false;
	}
	else
	{
		ac->Play();
	}
}

void AMusicPlayerActor::Pause()
{
	int status = findSource(sw);
	//Debug("Surce location returned " + FString::FromInt(status));

	if (audioSource)
	{
		audioSource->Pause();
		isPaused = true;
	}
}

void AMusicPlayerActor::Stop()
{
	ac->Stop();
	isPaused = false;
	if (audioSource)
	{
		audioSource->Stop();
	}
}

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
		for (auto activeSoundIt(tmpActualSounds.CreateIterator()); activeSoundIt; ++activeSoundIt){
			activeSound = *activeSoundIt;
			for (auto WaveInstanceIt(activeSound->WaveInstances.CreateIterator()); WaveInstanceIt; ++WaveInstanceIt){
				sw_instance = WaveInstanceIt.Value();
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
