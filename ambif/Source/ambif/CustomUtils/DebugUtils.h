//* Copyright 2015 Stefano Cherubin
//* stefano1{dot}cherubin{at}mail{dot}polimi{dot}it
//* ISPG - Image and Sound Processing Group
//* Politecnico di Milano

#pragma once

#include "CustomUtils.h"

#define ENABLE_DEBUG
#define ENABLE_LOG
#define ENABLE_ON_SCREEN_DEBUG

//UE_LOG(LogTemp, Warning, TEXT("Your message"));
//UE_LOG(LogTemp, Log, TEXT("new line: \n%s\ndo you like it?"), *FString(line.c_str()));
//UE_LOG(LogTemp, Log, TEXT("%s"), *Utils::CustomUnquote(FString(linetoken.c_str())))

/**
 * 
 */
namespace DebugUtils{
	inline static void LogString(FString str)
	{
#ifdef ENABLE_LOG
		UE_LOG(LogTemp, Log, TEXT("%s"), *Utils::CustomUnquote(str));
#endif
	}
}

#ifdef ENABLE_DEBUG
#ifdef ENABLE_ON_SCREEN_DEBUG
#define DEBUG(msg) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, (msg));
#else
#define DEBUG(msg) DebugUtils::LogString(FString((msg)));
#endif
#else
#define DEBUG(msg) ;
#endif