/*
 * CPSC 612 Spring 2019
 * HW1
 * by Chengyi Min
 */
class MySocket{
private:


public:
	char *buf;
	char *host;
	char *request;
	char *method;
	char *pageStart;
	char *source;
	INT64 pageSize;
	int port;
	SOCKET sock;
	MySocket();
	MySocket(SOCKET sock);
	void clearBuf();
	long long Read(INT64 max = 0);
};
