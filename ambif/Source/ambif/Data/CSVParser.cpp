// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#include "ambif.h"
#include "CSVParser.h"
#include "stdexcept"


int CSVParser::ReadPropertyDataCSV(const char* FileName, Utils::FHashMap<SongDetails>& OutputMap, bool FirstRowDiscarded, char elem_delim, char row_delim)
{
	int lineNumber = 0;
	std::string line, linetoken;
	std::ifstream ifile(FileName);
	if (!ifile) {	//file can't be opened
		return -1;
	}
	/* before going on I want to point out one thing:
	* I HATE std::getline
	* and obviously std::getline hates me.
	* It is important you remember that
	* if you touch std::getline code
	* std::getline will take its revenge on you. */
	if (FirstRowDiscarded)
	{
		if (!std::getline(ifile, line, row_delim))
			return 0;
	}
	while (std::getline(ifile, line, row_delim))
	{
#ifdef DEBUG_CSV_CONTENT
		UE_LOG(LogTemp, Log, TEXT("new line: \n%s\ntokenized as"), *FString(line.c_str()));
		//DEBUG(line.c_str())
#endif
		std::istringstream str_parser;
		bool found = false;
		str_parser.str(line);
		TArray<FString> SeparatedLine = TArray<FString>();
		FString ElementID;		//first column is Element ID.
		FString DimensionID = "";	//second column is Dimension ID
		std::getline(str_parser, linetoken, elem_delim);
		ElementID = Utils::CustomUnquote(FString(linetoken.c_str()));
		try {
			OutputMap.at(ElementID);
			found = true;
		}
		catch (std::out_of_range e) {
			found = false;
		}
		if (found) {	//if failed to load line, jump to next line
			FSongProperty property;
			std::getline(str_parser, linetoken, elem_delim);
			DimensionID = Utils::CustomUnquote(FString(linetoken.c_str()));

			while (std::getline(str_parser, linetoken, elem_delim)) {	//from third element to end of line
				SeparatedLine.Add(Utils::CustomUnquote(FString(linetoken.c_str())));	//append
#ifdef DEBUG_CSV_CONTENT
				UE_LOG(LogTemp, Log, TEXT("%s"), *Utils::CustomUnquote(FString(linetoken.c_str())));
				//DEBUG(linetoken.c_str())
#endif
			}
			property << SeparatedLine;
			if (DimensionID.Len() > 0)
			{
				OutputMap[ElementID].Properties[DimensionID] = property;
			}
			lineNumber++;
		}
#ifdef DEBUG_CSV_CONTENT
		UE_LOG(LogTemp, Log, TEXT(" . . . . . . ."));
#endif
	}
	ifile.close();
	return lineNumber;
}
