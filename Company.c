#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "Company.h"
#include "Airport.h"
#include "General.h"
#include "fileHelper.h"
#include "myMacros.h"

static const char* sortOptStr[eNofSortOpt] = {
	"None","Hour", "Date", "Airport takeoff code", "Airport landing code" };


int	initCompanyFromFile(Company* pComp, AirportManager* pManaer, const char* fileName)
{
	L_init(&pComp->flighDateList);
	if (loadCompanyFromFile(pComp, pManaer, fileName))
	{
		initDateList(pComp);
		return 1;
	}
	return 0;
}

void	initCompany(Company* pComp, AirportManager* pManaer)
{
	printf("-----------  Init Airline Company\n");
	L_init(&pComp->flighDateList);

	pComp->name = getStrExactName("Enter company name");
	pComp->flightArr = NULL;
	pComp->flightCount = 0;
}

void	initDateList(Company* pComp)
{
	for (int i = 0; i < pComp->flightCount; i++)
	{
		if (isUniqueDate(pComp, i))
		{
			char* sDate = createDateString(&pComp->flightArr[i]->date);
			L_insert(&(pComp->flighDateList.head), sDate);
		}
	}
}

int		isUniqueDate(const Company* pComp, int index)
{
	Date* pCheck = &pComp->flightArr[index]->date;
	for (int i = 0; i < index; i++)
	{
		if (equalDate(&pComp->flightArr[i]->date, pCheck))
			return 0;
	}
	return 1;
}

int		addFlight(Company* pComp, const AirportManager* pManager)
{
	if (pManager->count < 2)
	{
		printf("There are not enoght airport to set a flight\n");
		return 0;
	}
	pComp->flightArr = (Flight**)realloc(pComp->flightArr, (pComp->flightCount + 1) * sizeof(Flight*));
	CHECK_MSG_RETURN_0(pComp->flightArr, problem creating flightArr);

	pComp->flightArr[pComp->flightCount] = (Flight*)calloc(1, sizeof(Flight));
	CHECK_MSG_RETURN_0(pComp->flightArr[pComp->flightCount], problem creating flight);

	initFlight(pComp->flightArr[pComp->flightCount], pManager);
	if (isUniqueDate(pComp, pComp->flightCount))
	{
		char* sDate = createDateString(&pComp->flightArr[pComp->flightCount]->date);
		L_insert(&(pComp->flighDateList.head), sDate);
	}
	pComp->flightCount++;
	return 1;
}

void	printCompany(const Company* pComp, const char* str, ...)
{
	va_list strings;
	va_start(strings, str);
	const char* tempStr = str;
	int combainLength = 0;
	while (tempStr)
	{
		combainLength += ((int)strlen(tempStr)) + 1;//add 1 for "_" between word and we hav an extra space for '\0'
		tempStr = va_arg(strings, const char*);
	}
	char* combaindStr = (char*)malloc(combainLength * sizeof(char));//make new space for all the sentenc
	if (!combaindStr)
		return;
	strcpy(combaindStr, str);
	va_start(strings, str);
	tempStr = va_arg(strings, const char*);
	while (tempStr)
	{
		strcat(combaindStr, "_");
		strcat(combaindStr, tempStr);
		tempStr = va_arg(strings, const char*);
	}
	va_end(strings);
	printf("Company %s:\n", combaindStr);
	printf("Has %d flights\n", pComp->flightCount);
#ifdef DETAIL_PRINT
	generalArrayFunction((void*)pComp->flightArr, pComp->flightCount, sizeof(Flight**), printFlightV);
	printf("\nFlight Date List:");
	L_print(&pComp->flighDateList, printStr);
#endif 
}

void	printFlightsCount(const Company* pComp)
{
	char codeOrigin[CODE_LENGTH + 1];
	char codeDestination[CODE_LENGTH + 1];

	if (pComp->flightCount == 0)
	{
		printf("No flight to search\n");
		return;
	}

	printf("Origin Airport\n");
	getAirportCode(codeOrigin);
	printf("Destination Airport\n");
	getAirportCode(codeDestination);

	int count = countFlightsInRoute(pComp->flightArr, pComp->flightCount, codeOrigin, codeDestination);
	if (count != 0)
		printf("There are %d flights ", count);
	else
		printf("There are No flights ");

	printf("from %s to %s\n", codeOrigin, codeDestination);
}

int		saveCompanyToFile(const Company* pComp, const char* fileName)
{
	FILE* fp;
	fp = fopen(fileName, "wb");

	CHECK_NULL__MSG_CLOSE_FILE(fp, fp, Error open copmpany file to write);
#ifdef BIN_BIT_SAVE
	if (!saveCompanyToFileBit(pComp, fp))
	{
		fclose(fp);
		return 0;
	}
#else

	if (saveCompanyToFileRegular(pComp, fp) == 0)
	{
		fclose(fp);
		return 0;
	}
#endif // BIN_BIT_SAVE
	fclose(fp);
	return 1;
}

int loadCompanyFromFile(Company* pComp, const AirportManager* pManager, const char* fileName)
{
	FILE* fp;
	fp = fopen(fileName, "rb");
	CHECK_NULL__MSG_CLOSE_FILE(fp, fp, Error open company file);

	pComp->flightArr = NULL;
#ifdef BIN_BIT_SAVE
	if (!loadCompanyFromFileBit(pComp, pManager, fp))
	{
		fclose(fp);
		return 0;
	}
#else
	if (loadCompanyFromFileRegular(pComp, pManager, fp) == 0)
	{
		fclose(fp);
		return 0;
	}
#endif // BIN_BIT_SAVE

	fclose(fp);
	return 1;
}
int		saveCompanyToFileRegular(const Company* pComp, FILE* fp)
{
	if (!writeStringToFile(pComp->name, fp, "Error write comapny name\n"))
		return 0;

	if (!writeIntToFile(pComp->flightCount, fp, "Error write flight count\n"))
		return 0;

	if (!writeIntToFile((int)pComp->sortOpt, fp, "Error write sort option\n"))
		return 0;

	for (int i = 0; i < pComp->flightCount; i++)
	{
		if (!saveFlightToFile(pComp->flightArr[i], fp))
			return 0;
	}
	return 1;
}
int		saveCompanyToFileBit(const Company* pComp, FILE* fp)
{
	BYTE data[2] = { 0 };
	data[0] = pComp->flightCount << 7 | pComp->sortOpt << 4 | strlen(pComp->name);
	data[1] = pComp->flightCount >> 1;
	if (fwrite(data, sizeof(BYTE), 2, fp) != 2)
	{
		printf("Error write company\n");
		return 0;
	}
	if (fwrite(pComp->name, sizeof(char), (strlen(pComp->name) + 1), fp) != (strlen(pComp->name) + 1))
	{
		printf("Error write company\n");
		return 0;
	}
	for (int i = 0; i < pComp->flightCount; i++)
		if (saveFlightToFileBit(pComp->flightArr[i], fp) == 0)
			return 0;
	return 1;
}
int loadCompanyFromFileBit(Company* pComp, const AirportManager* pManager, FILE* fp)
{
	BYTE data[2] = { 0 };
	if (fread(data, sizeof(BYTE), 2, fp) != 2)
		return 0;
	pComp->flightCount = (data[0] & 0x80) >> 7 | data[1] << 1;
	pComp->sortOpt = (data[0] >> 4) & 0x7;
	int len = (data[0] & 0xF);
	char* name = (char*)malloc(sizeof(char)*(len + 1));
	CHECK_RETURN_0(name);
	if (fread(name, sizeof(char), (len + 1), fp) != (len + 1))
		return 0;
	pComp->name = name;
	if (pComp->flightCount == 0)
		return 1;
	pComp->flightArr = (Flight**)calloc((pComp->flightCount), sizeof(Flight*));
	CHECK_MSG_RETURN_0(pComp->flightArr, problem creating flightArr);

	for (int i = 0; i < pComp->flightCount; i++)
	{
		pComp->flightArr[i] = (Flight*)calloc(1, sizeof(Flight));
		CHECK_MSG_RETURN_0(pComp->flightArr[i], problem creating flight);
		if (loadFlightFromFileBit(pComp->flightArr[i], fp, pManager) == 0)
			return 0;
	}
	return 1;
}


int loadCompanyFromFileRegular(Company* pComp, const AirportManager* pManager, FILE* fp)
{
	pComp->name = readStringFromFile(fp, "Error reading company name\n");

	CHECK_RETURN_0(pComp->name);

	if (!readIntFromFile(&pComp->flightCount, fp, "Error reading flight count name\n"))
		return 0;

	int opt;
	if (!readIntFromFile(&opt, fp, "Error reading sort option\n"))
		return 0;

	pComp->sortOpt = (eSortOption)opt;

	if (pComp->flightCount > 0)
	{
		pComp->flightArr = (Flight**)malloc(pComp->flightCount * sizeof(Flight*));

		if (!pComp->flightArr)
		{
			MSG_CLOSE_RETURN_0(fp, Alocation error);
		}
	}
	else
		pComp->flightArr = NULL;

	for (int i = 0; i < pComp->flightCount; i++)
	{
		pComp->flightArr[i] = (Flight*)calloc(1, sizeof(Flight));
		CHECK_NULL__MSG_CLOSE_FILE(pComp->flightArr[i], fp, Alocation error);

		if (!loadFlightFromFile(pComp->flightArr[i], pManager, fp))
			return 0;
	}
	return 1;
}
void	sortFlight(Company* pComp)
{
	pComp->sortOpt = showSortMenu();
	int(*compare)(const void* air1, const void* air2) = NULL;

	switch (pComp->sortOpt)
	{
	case eHour:
		compare = compareByHour;
		break;
	case eDate:
		compare = compareByDate;
		break;
	case eSorceCode:
		compare = compareByCodeOrig;
		break;
	case eDestCode:
		compare = compareByCodeDest;
		break;

	}

	if (compare != NULL)
		qsort(pComp->flightArr, pComp->flightCount, sizeof(Flight*), compare);

}

void	findFlight(const Company* pComp)
{
	int(*compare)(const void* air1, const void* air2) = NULL;
	Flight f = { 0 };
	Flight* pFlight = &f;


	switch (pComp->sortOpt)
	{
	case eHour:
		f.hour = getFlightHour();
		compare = compareByHour;
		break;
	case eDate:
		getchar();
		getCorrectDate(&f.date);
		compare = compareByDate;
		break;
	case eSorceCode:
		getchar();
		getAirportCode(f.originCode);
		compare = compareByCodeOrig;
		break;
	case eDestCode:
		getchar();
		getAirportCode(f.destCode);
		compare = compareByCodeDest;
		break;
	}

	if (compare != NULL)
	{
		Flight** pF = bsearch(&pFlight, pComp->flightArr, pComp->flightCount, sizeof(Flight*), compare);
		if (pF == NULL)
			printf("Flight was not found\n");
		else {
			printf("Flight found, ");
			printFlight(*pF);
		}
	}
	else {
		printf("The search cannot be performed, array not sorted\n");
	}

}

eSortOption showSortMenu()
{
	int opt;
	printf("Base on what field do you want to sort?\n");
	do {
		for (int i = 1; i < eNofSortOpt; i++)
			printf("Enter %d for %s\n", i, sortOptStr[i]);
		scanf("%d", &opt);
	} while (opt < 0 || opt >eNofSortOpt);

	return (eSortOption)opt;
}

void	freeCompany(Company* pComp)
{
	generalArrayFunction((void*)pComp->flightArr, pComp->flightCount, sizeof(Flight**), freeFlight);
	free(pComp->name);
	L_free(&pComp->flighDateList, freePtr);
	free(pComp->flightArr);
}
