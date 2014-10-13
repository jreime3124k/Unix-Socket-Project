/*
 * client.c
 */

#include <stdio.h>
#include <iostream>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstdlib>
#include <cstring>

using namespace std;

#define SERVER_PORT 5556
#define MAX_LINE 256

int main(int argc, char * argv[])
{

    struct sockaddr_in sin;
    char buf[MAX_LINE];
    char rbuf[MAX_LINE];
	char rbuf2[MAX_LINE];
	char msgstorsnd[MAX_LINE];
	char msgstorrce[MAX_LINE];
    int len;
	int len1;
    int s;
	char *message;
	
    if (argc < 2)
	{
		cout << "Usage: client <Server IP Address>" << endl;
		exit(1);
    }

    /* active open */
    if ((s = socket (AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket");
		exit(1);
    }

    /* build address data structure */
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
	/*inet_addr turns the string into internal four byte integers*/
    sin.sin_addr.s_addr  = inet_addr(argv[1]);	//Server's Address from Argument 1
    sin.sin_port = htons (SERVER_PORT);

    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
		perror("connect");
		close(s);
		exit(1);
    }
	cout << "Client: ";
    /* main loop; get and send lines of text */
    while (fgets(buf, sizeof(buf), stdin))
	{
		buf[MAX_LINE -1] = '\0';
		len = strlen(buf) + 1;
		send (s, buf, len, 0);
		//recv (s, rsig, sizeof(rsig), 0);	//Receive signal from server
		recv (s, rbuf, sizeof(rbuf), 0);
		//cout << "SERVER: " << rsig;
		cout << "SERVER: " << rbuf;
		cout << "Client: ";
		
		if (strcmp(rbuf,"Quitting program...\n----All your base are belong to us!----\n") == 0)
		{
			cout << endl;
			exit(1);
		}
		else if (strcmp(rbuf,"Shutting down the server interface...\nGoodbye!\n") == 0)
		{
			cout << "Server Shutdown" << endl;
			exit(1);
		}
		else if (strcmp(rbuf,"200 OK\n") == 0)
		{
			cout << "Logged In, Send buffer\n";
			char msgstorsnd[MAX_LINE] = "";
			cin.getline(msgstorsnd, 256);
			//string msgstring;
			//cin >> ios::skipws msgstring;
			//size_t length = msgstring.copy(msgstorsnd,MAX_LINE,0);
			//cout << "msgSend: " << msgstorsnd <<endl;
			len1 = strlen(msgstorsnd) +1;
			msgstorsnd[len1 -1] = '\0';
			//cout << "msgstring: " << msgstring << endl;
			//cout << "msgSend: " << msgstorsnd <<endl;
			//len1 = strlen(msgstorsnd) +1;
			//send (s, msgstorsnd, sizeof(msgstorsnd), 0);
			send (s, msgstorsnd, len1, 0);
			recv (s, rbuf2, sizeof(rbuf2), 0);
			cout << "SERVER: " << rbuf2 << endl;
			cout << "Client: ";
		}
	
    //cout << "Client: ";
	}

    close(s);
} 
 
