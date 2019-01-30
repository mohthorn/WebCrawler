/*
 * CPSC 612 Spring 2019
 * HW1
 * by Chengyi Min
 */
#include "pch.h"

int URLRead(char *source, struct sockaddr_in &server, char *method, char *host, char *request, int port, bool robot =FALSE, INT64 max =0)
{
	clock_t start;
	clock_t end;
	clock_t duration;
	Utilities ut;
	//*********************************
	// Refered from 463 sample
	// string pointing to an HTTP server (DNS name or IP)

	//char str [] = "128.194.135.72";
	
	WSADATA wsaData;

	//Initialize WinSock; once per program run
	WORD wVersionRequested = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequested, &wsaData) != 0) {
		printf("WSAStartup error %d\n", WSAGetLastError());
		WSACleanup();
		exit(0);
	}

	// open a TCP socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		printf("socket() generated error %d\n", WSAGetLastError());
		WSACleanup();
		exit(0);
	}

	// setup the port # and protocol type
	server.sin_family = AF_INET;
	server.sin_port = htons(port);		// host-to-network flips the byte order

	if(robot)
		printf("\tConnecting on robot... ");
	else
		printf("\t\b\b* Connecting on page... ");
	// connect to the server on port 
	start = clock();
	if (connect(sock, (struct sockaddr*) &server, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
	{
		printf("failed with %d\n", WSAGetLastError());
		return(0);
	}
	end = clock();
	duration = 1000.0*(end - start) / (double)(CLOCKS_PER_SEC);
	printf("done in %dms\n", duration);
	//printf("Successfully connected to %s (%s) on port %d\n", host, inet_ntoa(server.sin_addr), htons(server.sin_port));
	//****************************************


	printf("\tLoading... ");
	start = clock();
	// Form request
	char *sendBuf = new char[10240];
	sprintf(sendBuf,
		"%s %s HTTP/1.0\r\n"
		"User-agent: myTAMUcrawler/1.0\r\n"
		"Host: %s\r\n"
		"Connection: close\r\n"
		"\r\n",
		method, request, host
	);

	// Send request

	if (send(sock, sendBuf, (int)strlen(sendBuf), 0) == SOCKET_ERROR)
	{
		printf("Send error: %d\n", WSAGetLastError());
		return(0);
	}
	//printf("\trequest sent\n");

	//Receive Response
	MySocket sockRECV(sock);
	long long ret = sockRECV.Read(max);
	if (ret < 0)
	{
		return(0);
	}
	char *pos = strstr(sockRECV.buf, "\r\n\r\n");
	if (pos != NULL)
	{
		*(pos + 2) = 0;
		pos = pos + 4;
	}

	end = clock();
	duration = 1000.0*(end - start) / (double)(CLOCKS_PER_SEC);
	printf("done in %dms, ", duration);
	printf("with %d bytes\n", ret);
	//printf("Successfully connected to %s (%s) on port %d\n", host, inet_ntoa(server.sin_addr), htons(server.sin_port));


	//Get Status Code
	printf("\tVerifying header... ");
	char * status = NULL;
	if ((status = strstr(sockRECV.buf, "HTTP/")) != NULL)
	{
		status = status + 5;
	}
	else
	{
		printf("failed with non-HTTP header\n");
		return(0);
	}
	if ((status = strchr(status, ' ')) != NULL)
	{
		status += 1;
	}
	else
	{
		printf("can't find status code\n");
		return(0);
	}


	UINT status_code = 0;
	for (int i = 0; *status != ' '; status++)
	{
		status_code *= 10;
		status_code += (*status - '0');
	}
	printf("status code %d\n", status_code);

	if (strcmp(method,"GET"))
		return status_code;

	//Page Parsing
	if (status_code / 100 == 2) //status code 2
	{
		printf("\t\b\b+ Parsing page...");
		start = clock();
		long long nLinks = ut.link_count(source, pos, ret);
		end = clock();
		duration = 1000.0*(end - start) / (double)(CLOCKS_PER_SEC);
		printf("done in %dms, ", duration);
		printf("with %lld links\n", nLinks);

	}



	// close the socket to this server; open again for the next one
	sockRECV.clearBuf();
	closesocket(sock);

	// call cleanup when done with everything and ready to exit program
	WSACleanup();
	return status_code;
}