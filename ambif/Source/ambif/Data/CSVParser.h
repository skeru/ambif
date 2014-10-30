// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

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
