// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#pragma once
#include "DataDefinitions/Genre.h"
#include "CustomUtils/CustomUtils.h"

/**
 * 
 */
class AMBIF_API MusicGenreAdapter
{
protected:
	static MusicGenreAdapter* instance;

private:
	Utils::FMap<GenreNodes::node> StrToNode;
	Utils::map < Genre::Type, GenreNodes::node > TypeToNode;

	//deprecated
	Utils::FHashMap<Genre::Type> StringToGenreConverter;

public:
	inline float GenreToFloat(Genre::Type elem);

	inline Genre::Type StringToGenre(FString elem);

	inline float StringToFloat(FString elem);

private:
	MusicGenreAdapter();
	
public:
	static MusicGenreAdapter* GetInstance();
};

