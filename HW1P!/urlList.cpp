/*
 * CPSC 612 Spring 2019
 * HW1
 * by Chengyi Min
 */
#include "pch.h"

int URLRead(char *source, struct sockaddr_in &server, MySocket &mysock, bool robot = FALSE, INT64 maxRecvSize = 0);


class Parameters {
public:
	HANDLE	mutex;
	HANDLE	finished;
	HANDLE	eventQuit;
	char* url;
	unordered_set<DWORD> seenIPs;
	unordered_set<string> seenHosts;

};


UINT crawlingThread(LPVOID pParam)
{
	Utilities ut;
	Parameters *p = ((Parameters*)pParam);
	printf("URL: %s\n", p->url);

	//source url passed to HTMLparser
	char *source = new char[strlen(p->url) + 1];
	memcpy(source, p->url, strlen(p->url) + 1);

	MySocket mysock;
	//getting host, request and port
	char * actualRequest = new char[MAX_REQUEST_LEN];
	mysock.port = ut.request_parse(mysock.host, actualRequest, p->url);
	if (mysock.port < 0)
		exit(0);
	printf("host %s, port %d\n", mysock.host, mysock.port);

	//uniqueness check
	printf("\tChecking host uniqueness... ");
	int prevHostSize = p->seenHosts.size();	
	p->seenHosts.insert(mysock.host);
	if (p->seenHosts.size() <= prevHostSize)
	{
		printf("failed\n");
		return FAILURECODE;
	}
	printf("passed\n");

	//finding dns
	DWORD IP;
	struct sockaddr_in server;
	if ((IP=ut.DNSParse(mysock.host, server)) == INADDR_NONE) //dns error
	{
		return FAILURECODE;
	}

	//IP check
	printf("\tChecking IP uniqueness... ");
	int prevIPSize = p->seenIPs.size();
	p->seenIPs.insert(IP);
	if (p->seenIPs.size() <= prevIPSize)
	{
		printf("failed\n");
		return FAILURECODE;
	}
	printf("passed\n");
	//uniqueness check finished

	//send robot request and parse it
	char robots[] = "/robots.txt";
	char met[] = "HEAD";
	mysock.method = met;
	mysock.request = robots;
	int status_code = URLRead(source, server, mysock, TRUE ,MAX_ROBOT);

	//send page request
	char methodGet[]= "GET";
	if (status_code / 100 == 4)
	{
		mysock.method = methodGet;
		mysock.request = actualRequest;
		URLRead(source, server, mysock, FALSE, MAX_RECV);
	}
}

long long urlListParse(char * filename, int nThreads)
{


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