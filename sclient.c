/**********************************************************
PROGRAM: 		Client for MultiThread Unix Socket
AUTHOR:			Gareth Lawlor, Joshua Reimer
CLASS:			CIS 527: Project 2
DATE:			11/10/2014
VERSION#:		2.0
DESCRIPTION:	See README
FIX HISTORY:	See README
FILENAME:		sclient.c
NOTES:			Available on GitHub: https://github.com/GLawlor-/Unix-Socket-Project
***********************************************************/
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

	// Send Welcome Message
	cout << "Welcome to the server!\nPlease send a ? or MENU to see your options\nClient: " << flush;
	
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
				cout << "\nServer: " << buf << flush;
				//Client reactions start here
				if (strcmp(buf,"Quitting program...\n----All your base are belong to us!----\n") == 0)
				{
					cout << endl;
					exit(1);
				}
				else if (strcmp(buf,"200 OK\nShutting down the server interface...\nGoodbye!\n") == 0)
				{
					cout << "Server Shutdown" << endl;
					exit(1);
				}
				else if (strcmp(buf,"210 The server is about to shutdown...\nGoodbye\n") == 0)
				{
					//cout << buf << endl;
					cout << endl;
					exit(1);
				}
				else if (strcmp(buf,"200 OK\n") == 0)
				{
					//Received Signal that MSGSTORE is writable
					cout << "Logged In, Send buffer\n";
					char msgstorsnd[MAX_LINE] = "";
					//Get user's input for MSGSTORE
					cin.getline(msgstorsnd, 256);
					len1 = strlen(msgstorsnd) +1;
					msgstorsnd[len1 -1] = '\0';
					
					send (s, msgstorsnd, len1, 0);
					recv (s, rbuf2, sizeof(rbuf2), 0);
					cout << "SERVER: " << rbuf2 << endl;
					
				}
				
				cout << "Client: " << flush;  //Repeat every cycle and force the output to screen
				
				
				
			}
        }
    }

    close(s);
}

