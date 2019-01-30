/*
 * CPSC 612 Spring 2019
 * HW1
 * by Chengyi Min
 */
class MySocket{
private:

	SOCKET sock;
public:
	char *buf;
	MySocket(SOCKET sock);
	void clearBuf();
	long long Read(INT64 max = 0);
};
