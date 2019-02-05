/*
 * CPSC 612 Spring 2019
 * HW1
 * by Chengyi Min
 */
#include "pch.h"

int URLRead(char *source, struct sockaddr_in &server, char *method, char *host, char *request, int port, bool robot = FALSE, INT64 max = 0);


class Parameters {
public:
	HANDLE	mutex;
	HANDLE	finished;
	HANDLE	eventQuit;
};


UINT crawlingThread(LPVOID pParams)
{

}

long long urlListParse(char * filename, int nThreads)
{
	unordered_set<DWORD> seenIPs;
	unordered_set<string> seenHosts;

	// **************Opening and Reading File from HTMLParser
	HANDLE hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	// process errors
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("CreateFile failed with %d\n", GetLastError());
		return 0;
	}

	// get file size
	LARGE_INTEGER li;
	BOOL bRet = GetFileSizeEx(hFile, &li);
	// process errors
	if (bRet == 0)
	{
		printf("GetFileSizeEx error %d\n", GetLastError());
		return 0;
	}

	// read file into a buffer
	int fileSize = (DWORD)li.QuadPart;			// assumes file size is below 2GB; otherwise, an __int64 is needed
	DWORD bytesRead;
	// allocate buffer
	char *fileBuf = new char[fileSize];
	// read into the buffer
	bRet = ReadFile(hFile, fileBuf, fileSize, &bytesRead, NULL);
	char method[] = "HEAD";
	// process errors
	if (bRet == 0 || bytesRead != fileSize)
	{
		printf("ReadFile failed with %d\n", GetLastError());
		return 0;
	}

	// done with the file
	CloseHandle(hFile);
	//*****************************
	fileBuf[fileSize] = NULL;
	printf("Opened %s with size %d\n", filename,fileSize);
	char * pStart = fileBuf;
	char * pEnd = pStart;
	Utilities ut;
	if (ut.winsockInitialize() == -1)
		return -1;

	// a pointer list storing starting points of every link
	char ** links = new char*[TRUNC] ;
	UINT64 currentMax = TRUNC;
	UINT64 nLinks = 0;

	while ((pEnd = strstr(pStart, "\r\n")) != NULL )
	{
		*pEnd = 0;

		if (*pStart != '\r' && * pStart != 0 && *pStart != '\n')
		{
			links[nLinks] = pStart;
			pStart = pEnd + 2;
			nLinks++;
			if (currentMax - nLinks < LINKTHRESHOLD)
			{
				char **temp = (char**)realloc(links, currentMax * 2); 
				if (!temp)
					printf("not enough space\n");
				currentMax *= 2;
				//memcpy(temp, buf, allocatedSize);
				links = temp;
			}
		}

		//if (*pStart != '\r' && * pStart != 0 && *pStart != '\n')
		//{
		//	printf("URL: %s\n", pStart);

		//	char *source = new char[strlen(pStart) + 1];
		//	memcpy(source, pStart, strlen(pStart) + 1);

		//	char *host = new char[MAX_HOST_LEN];
		//	char *request = new char[MAX_REQUEST_LEN];
		//	int port = 80;
		//	//getting useful infos
		//	port = ut.request_parse(host, request, pStart);
		//	if (port < 0)
		//		exit(0);
		//	printf("host %s, port %d\n", host, port);

		//	printf("\tChecking host uniqueness... ");
		//	int prevHostSize = seenHosts.size();
		//	
		//	seenHosts.insert(host);


		//	if (seenHosts.size() <= prevHostSize)
		//	{
		//		printf("failed\n");
		//		pStart = pEnd + 2;
		//		continue;
		//	}
		//	printf("passed\n");

		//	DWORD IP;
		//	struct sockaddr_in server;
		//	if ((IP=ut.DNSParse(host, server)) == INADDR_NONE) //dns error
		//	{
		//		pStart = pEnd + 2;
		//		continue;
		//	}
		//	printf("\tChecking IP uniqueness... ");
		//	int prevIPSize = seenIPs.size();
		//	seenIPs.insert(IP);
		//	if (seenIPs.size() <= prevIPSize)
		//	{
		//		printf("failed\n");
		//		pStart = pEnd + 2;
		//		continue;
		//	}
		//	printf("passed\n");

		//	char robots[] = "/robots.txt";
		//	int status_code = URLRead(source, server, method, host, robots, port, TRUE ,MAX_ROBOT);
		//	char methodGet[]= "GET";
		//	if (status_code / 100 == 4)
		//	{
		//		URLRead(source, server, methodGet, host, request, port, FALSE, MAX_RECV);
		//	}
		//	pStart = pEnd + 2;
		//}
		
	}
	
	for (int i = 0; i < nLinks; i++)
	{
		printf("%s\n", links[i]);
	}

	//printf("%s\r\n", fileBuf);
	return 1;
}