// Stefano Cherubin stefano1.cherubin@mail.polimi.it

#pragma once

#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "Runtime/Engine/Public/VorbisAudioInfo.h"
#include "AudioDecompress.h"
#include "AudioDevice.h"
#include "ActiveSound.h"
#include "Audio.h"
#include "Developer/TargetPlatform/Public/Interfaces/IAudioFormat.h"

#include "MusicPlayerActor.generated.h"

/**
 * This actor allows to play arbitratry OggVorbis audio files from user hard disk.
 */
UCLASS()
class AMBIF_API AMusicPlayerActor : public AActor
{
	GENERATED_BODY()
public:
	//constructor
	AMusicPlayerActor();

private:
	//* Audio file name (OggVorbis) with path
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MusicPlayer)
	FString FileName;

	//* manually constructed SoundWave 
	USoundWave* sw;
	//* Game component used to play audio
	UAudioComponent* ac;
	//* loaded song file (binary, encoded)
	TArray < uint8 > rawFile;

	//* active sound object
	FActiveSound* activeSound;
	FAudioDevice* device;
	//* instance of USoundWave currently playing
	FWaveInstance* sw_instance;
	//* current sound audio source object
	FSoundSource* audioSource;

	//* If true the song was successfully loaded
	bool loaded;
	
	//* If true the song is playing
	bool isPlaying;

	//* If true the song was manually paused by the user
	bool isPaused;

	//* Played seconds of the current song. Updated every tick.
	double playTime;

public:
	UFUNCTION(BlueprintCallable, Category = "Player")
	void Load(FString NewFileName);
	
	UFUNCTION(BlueprintCallable, Category = "Player")
	void Stop();
	
	UFUNCTION(BlueprintCallable, Category = "Player")
	void Play();
	
	UFUNCTION(BlueprintCallable, Category = "Player")
	void Pause();
	
	UFUNCTION(BlueprintCallable, Category = "Player Status")
	/** True if the song playing, false otherwise. */
	bool IsPlaying();
	
	UFUNCTION(BlueprintCallable, Category = "Player Status")
	/** If true the song was manually paused by the user */
	bool IsPaused();
	
	UFUNCTION(BlueprintCallable, Category = "Player Status")
	/** Return true if the song was successfully loaded */
	bool IsLoaded();
	
	UFUNCTION(BlueprintCallable, Category = "Player Status")
	double GetPlayTime();

	//--------------------------------private stuff--------------------------------

private:

	/**
	* Read .ogg header file and refresh USoundWave metadata.
	* @param sw		wave to put metadata
	* @param rawFile	pointer to src file in memory
	* @return 0	if everything is ok
	*		  1 if couldn't read metadata.
	*/
	int fillSoundWaveInfo(USoundWave* sw, TArray<uint8>* rawFile);

	/**
	* Tries to find out FSoundSource object associated to the USoundWave.
	* @param sw     wave, search key
	* @return 0 if wave found and correctly set
	*        -1 if error: sound device not set
	*        -2 if error: sound wave not found
	*/
	int findSource(USoundWave* sw);

	//----------------------TICK-----------------------
public:
	UFUNCTION()
	void Tick(float DeltaTime) override;
};