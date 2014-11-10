/*
 * sclient.c
 */

#include <iostream>
#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <cstdlib>

using namespace std;

#define SERVER_PORT 5556
#define MAX_LINE 256
#define STDIN 0

int main(int argc, char * argv[]) {

    if (argc < 2)
	{
		cout << "Usage: client <Server IP Address>" << endl;
		exit(1);
    }
		//comment out to include localhost
		
	fd_set master;   // master file descriptor list
    fd_set read_fds; // temp file descriptor list for select()
    int fdmax;       // maximum file descriptor number

    struct sockaddr_in sin;
    char buf[MAX_LINE];
    int len;
    int s;
	char rbuf[MAX_LINE];
	char rbuf2[MAX_LINE];
	char msgstorsnd[MAX_LINE];
	char msgstorrce[MAX_LINE];
    int len1;
	char *message;
	
    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);

    /* active open */
    if ((s = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("select client: socket");
		exit(1);
    }

    /* build address data structure */
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr  = inet_addr(argv[1]);
    sin.sin_port = htons (SERVER_PORT);

    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
		perror("select client: connect");
		close(s);
		exit(1);
    }

    // add the STDIN to the master set
    FD_SET(STDIN, &master);

    // add the listener to the master set
    FD_SET(s, &master);

    // keep track of the biggest file descriptor
    fdmax = s; // so far, it's this one

    /* main loop; get and send lines of text */
    while (1) {
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(1);
        }

        // looking for data to read either from the server or the user
        if (FD_ISSET(STDIN, &read_fds)) {
			// handle the user input
			if (fgets(buf, sizeof(buf), stdin)){
				buf[MAX_LINE -1] = '\0';
				len = strlen(buf) + 1;
				send (s, buf, len, 0);
			} else {
				break;
            }
		}

		if (FD_ISSET(s, &read_fds)) {
			// handle data from the server
			if (recv(s, buf, sizeof(buf), 0) > 0)
			{
				cout << buf;
				//add menu branching here
				if (strcmp(buf,"Quitting program...\n----All your base are belong to us!----\n") == 0)
				{
					cout << endl;
					exit(1);
				}
			else if (strcmp(buf,"Shutting down the server interface...\nGoodbye!\n") == 0)
			{
				cout << "Server Shutdown" << endl;
				exit(1);
			}
			else if (strcmp(buf,"200 OK\n") == 0)
			{
				cout << "Logged In, Send buffer\n";
				char msgstorsnd[MAX_LINE] = "";
				cin.getline(msgstorsnd, 256);
				//string msgstring;
				//cin >> ios::skipws msgstring;
				//size_t length = msgstring.copy(msgstorsnd,MAX_LINE,0);
				cout << "msgSend: " << msgstorsnd <<endl;
				
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
				
				
				
				
			}
        }
    }

    close(s);
}

