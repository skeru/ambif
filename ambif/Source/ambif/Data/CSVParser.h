//* Copyright 2015 Stefano Cherubin
//* stefano1{dot}cherubin{at}mail{dot}polimi{dot}it
//* ISPG - Image and Sound Processing Group
//* Politecnico di Milano

#pragma once

#include "CustomUtils/CustomUtils.h"
#include "DataDefinitions/TableDefinitions.h"
#include "iostream"
#include "fstream"
#include "sstream"
#include "stdio.h"
#include "string"
#include "string.h"

#define CSV_COLUMN_DELIMITER ','
#define CSV_ROW_DELIMITER '\n' //right now does not work with ';' 

//#define DEBUG_CSV_CONTENT

/**
 * 
 */
class AMBIF_API CSVParser
{
public:

public:
	template<typename FileStructure>
	/** Read CSV content in FileName and fills given FHashMap.
	*
	* @param FileName path to CSV file
	* @param OutputMap already instanciated FHashMap to be filled
	* @param FirstRowDiscarded [default true] discard header line in csv file
	* @return number of line read (header not included)
	*
	* FileStructure MUST have
	* - FString Id field
	*   used as Identificator in the FHashMap
	* - FileStructure& operator<<(TArray<FString>)
	*   used to fill the FileStructure element to put in the FHashMap
	*  */
	static int ReadPlainTextFile(const char* FileName, Utils::FHashMap<FileStructure>& OutputMap, bool FirstRowDiscarded = true, char elem_delim = CSV_COLUMN_DELIMITER, char row_delim = CSV_ROW_DELIMITER);

	static int ReadPropertyDataCSV(const char* FileName, Utils::FHashMap<SongDetails>& OutputMap, bool FirstRowDiscarded = true, char elem_delim = CSV_COLUMN_DELIMITER, char row_delim = CSV_ROW_DELIMITER);
};


//####################_implementation_####################

template<typename FileStructure>
static int CSVParser::ReadPlainTextFile(const char* FileName, Utils::FHashMap<FileStructure>& OutputMap, bool FirstRowDiscarded, char elem_delim, char row_delim)
{
	FileStructure element;
	int lineNumber = 0;
	std::string line, linetoken;
	std::ifstream ifile(FileName);
	if (!ifile)
	{//file can't be opened
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
		str_parser.str(line);
		TArray<FString> SeparatedLine = TArray<FString>();
		while (std::getline(str_parser, linetoken, elem_delim))
		{
			SeparatedLine.Add(Utils::CustomUnquote(FString(linetoken.c_str())));	//append
#ifdef DEBUG_CSV_CONTENT
			UE_LOG(LogTemp, Log, TEXT("%s"), *Utils::CustomUnquote(FString(linetoken.c_str())));
			//DEBUG(linetoken.c_str())
#endif
		}
		element << SeparatedLine;
		OutputMap[element.Id] = element;
		lineNumber++;
#ifdef DEBUG_CSV_CONTENT
		UE_LOG(LogTemp, Log, TEXT(" . . . . . . ."));
#endif
	}
	ifile.close();
	return lineNumber;
}


#undef CSV_COLUMN_DELIMITER
#undef CSV_ROW_DELIMITER

#ifdef DEBUG_CSV_CONTENT
#undef DEBUG_CSV_CONTENT
#endif