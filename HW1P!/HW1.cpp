// HW1.cpp : This file contains the 'main' function. Program execution begins and ends there.
/*
 * CPSC 612 Spring 2019
 * HW1
 * by Chengyi Min
 */

#include "pch.h"

int URLRead(char *source, struct sockaddr_in &server, char *method, char *host, char *request, int port, bool robot = FALSE, INT64 max = 0);


long long urlListParse(char * filename, int nThreads);


int main(int argc, char **argv)
{
	if (argc != 2)
	{
		if (argc != 3)
		{
			printf("Usage: executable number_of_threads filename\r\nor\r\nexecutable URL\r\nexiting...\r\n");
			exit(0);
		}
		else
		{
			if (atoi(argv[1])<1)
			{
				printf("Threads: %d\r\n",atoi(argv[1]));
				printf("Usage: executable number_of_threads filename\r\nor\r\nexecutable URL\r\nexiting...\r\n");
				exit(0);
			}
		}
	}

	if (argc == 3)
	{
		urlListParse(argv[2],atoi(argv[1]));
		return 0;
	}

	Utilities ut ;
	if (ut.winsockInitialize() == -1)
		return 0;

	printf("URL: %s\n", argv[1]);
	char method[] = "GET";
	char *host = new char[MAX_HOST_LEN];
	char *request = new char[MAX_REQUEST_LEN];
	int port = 80;
	char *source = new char[strlen(argv[1]) + 1];
	memcpy(source, argv[1], strlen(argv[1]) + 1);
	port = ut.request_parse(host, request, argv[1]);
	if (port < 0)
		return (0);
	printf("host %s, port %d, request %s\n", host, port, request);
	
	struct sockaddr_in server;
	if(ut.DNSParse(host, server)!= INADDR_NONE)
		URLRead(source, server, method,host, request, port, FALSE,0);
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
