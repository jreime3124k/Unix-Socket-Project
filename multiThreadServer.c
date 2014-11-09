/*
 * multiThreadServer.c -- a multithreaded server
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <strings.h>

using namespace std;

#define PORT 5556  // port we're listening on
#define MAX_LINE 256

fd_set master;   // master file descriptor list
int listener;    // listening socket descriptor
int fdmax;
int line_count;	 // Set global variable for MSG Tracking
int shutdowncmd = 0; //Flag to signal server shutdown

// the child thread
void *ChildThread(void *newfd) {
    char buf[MAX_LINE];
    int nbytes;
    int i, j;
    int childSocket = (size_t) newfd;
	int new_s;		//second socket
	int msglen;		//find length of char array to send
	int msglen1;
	int mnum = 0;		//Initialize to get first message
	int shutdowncmd =0; //Flag to signal server shutdown request
	int loggedin = 0;	//Flag for user logged in
	int rootuser = 0;	//Flag if root user
	
	string ReadData(int a);  //Declare function to read Message of the Day data
	int UserLogin(string b);
	string SendMessage(int a);
	
    while(1) {
        // handle data from a client
        if ((nbytes = recv(childSocket, buf, sizeof(buf), 0)) <= 0) {
            // got error or connection closed by client
            if (nbytes == 0) {
                // connection closed
                cout << "multiThreadServer: socket " << childSocket <<" hung up" << endl;
            } else {
                perror("recv");
            }
            close(childSocket); // bye!
            FD_CLR(childSocket, &master); // remove from master set
            pthread_exit(0);
        } else {
            // we got some data from a client
            cout << childSocket << ": "<< buf;
//Need to incorporate decision tree here
//I'm thinking a function is required, maybe an external C++
//shutdown needs to close all ports first
			if( (strcmp(buf, "1\n")) == 0 || (strcmp(buf, "MSGGET\n")) == 0 )
			{
				cout << "MSGGET Received" << endl;
				char msgres[MAX_LINE] = "MSGGET Received\n";
				msglen = strlen (msgres)+1;
				//send(j,msgres, msglen, 0);
				send (childSocket, msgres, msglen, 0);		
			}
			else  //Catch everything else; tell client we don't understand the option
			{
				cout << "Not Recognized"<<endl;
				char msgres[MAX_LINE] = "Command not recognized\n--Type MENU or ? for available options--\n";
				msglen = strlen (msgres)+1;
				//j = childSocket;
				send (childSocket, msgres, msglen, 0);
				
				//send(j, buf, nbytes, 0);			
			}
		
            
			//send to all
			/*
			for(j = 0; j <= fdmax; j++) {
                // send to everyone!
                if (FD_ISSET(j, &master)) {
                    // except the listener and ourselves
                    if (j != listener && j != childSocket) {
                        cout << "j: " << j<< endl;
						if (send(j, buf, nbytes, 0) == -1) {
                            perror("send");
                        }
                    }
                }
            }//send to all end*/
        }
    }
}


int main(void)
{
    struct sockaddr_in myaddr;     // server address
    struct sockaddr_in remoteaddr; // client address
    int newfd;        // newly accept()ed socket descriptor
    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    socklen_t addrlen;

    pthread_t cThread;

    FD_ZERO(&master);    // clear the master and temp sets

    // get the listener
    if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // lose the pesky "address already in use" error message
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    // bind
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = INADDR_ANY;
    myaddr.sin_port = htons(PORT);
    memset(&(myaddr.sin_zero), '\0', 8);
    if (bind(listener, (struct sockaddr *)&myaddr, sizeof(myaddr)) == -1) {
        perror("bind");
        exit(1);
    }

    // listen
    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(1);
    }

    // add the listener to the master set
    FD_SET(listener, &master);

    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one

    addrlen = sizeof(remoteaddr);

    // main loop
    for(;;) {
        // handle new connections
        if ((newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen)) == -1) {
            perror("accept");
	        exit(1);
        } else {
            FD_SET(newfd, &master); // add to master set
            cout << "multiThreadServer: new connection from "
		 		 << inet_ntoa(remoteaddr.sin_addr)
                 << " socket " << newfd << endl;

            if (newfd > fdmax) {    // keep track of the maximum
                fdmax = newfd;
            }

	        if (pthread_create(&cThread, NULL, ChildThread, (void *) newfd) <0) {
                perror("pthread_create");
                exit(1);
            }
        }

    }
    return 0;
}

string SendMessage(int a)
{
	string data[] = "Send Message\n";
	string data2 = data[a];
	return data2;
}

string ReadData(int a)
{
				string data[20];
				//char null[1];
				string line;
				line_count = 0;  //reset # of lines
				//char numstr[21];
				//string result;
				//Pull from file; Have to read everytime in case their are new entries
				ifstream infile;
				infile.open("MoD.txt");
				if ( ! infile ){
					cout << "Error opening file" << endl;
				}
				else {
					while ( getline( infile, line )){
						data[line_count] = line;
					//	cout << "Line_count: " << line_count << endl;
					//	cout << "Data: " << data[line_count] << endl;
						line_count++;
					}
				}
				//cout <<"	Results From File"<<endl;
				//cout << "Lines Read: " << line_count << "\n";
				infile.close();	//Close the file after reading
				string data2 = data[a];
				return data2;
}