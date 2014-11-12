/**********************************************************
PROGRAM: 		Server for MultiThread Unix Socket
AUTHOR:			Gareth Lawlor, Joshua Reimer
CLASS:			CIS 527: Project 2
DATE:			11/10/2014
VERSION#:		2.0
DESCRIPTION:	See README
FIX HISTORY:	See README
FILENAME:		multiThreadServer.c
NOTES:			Available on GitHub: https://github.com/GLawlor-/Unix-Socket-Project
***********************************************************/
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
#include <vector>
#include <sstream>

using namespace std;

#define PORT 5556  // port we're listening on
#define MAX_LINE 512

fd_set master;   // master file descriptor list
int listener;    // listening socket descriptor
int fdmax;
int line_count;	 // Set global variable for MSG Tracking
int shutdowncmd = 0; //Flag to signal server shutdown
string connInfo[10];  //List of 10 connections, with information: first part is socket#, second part '0' is IP address, '1' is UserName
string connUser[10];

// the child thread
void *ChildThread(void *newfd) {
    char buf[MAX_LINE];
    int nbytes;
    int i, j;
    int childSocket = (size_t) newfd;
	//Dont Need--int new_s;		//second socket
	int msglen;		//find length of char array to send
	int msglen1;
	int mnum = 0;		//Initialize to get first message
	//int shutdowncmd =0; //Flag to signal server shutdown request
	int loggedin = 0;	//Flag for user logged in
	int rootuser = 0;	//Flag if root user
	
	string ReadData(int a);  //Declare function to read Message of the Day data
	int UserLogin(string b, int c);  //Function to check if user and password is correct
	
    while(1) {
        // handle data from a client
        if ((nbytes = recv(childSocket, buf, sizeof(buf), 0)) <= 0) {
            // got error or connection closed by client
            if (nbytes == 0) {
                // connection closed
                cout << "Server: socket " << childSocket <<" hung up" << endl;
				connInfo[childSocket] = "disconnect";	//Remove from Array
            } else {
                perror("recv");
            }
            close(childSocket); // bye!
            FD_CLR(childSocket, &master); // remove from master set
            pthread_exit(0);
        } else {
            // we got some data from a client
            cout << "Client " << childSocket << ": "<< buf;

			if( (strcmp(buf, "1\n")) == 0 || (strcmp(buf, "MSGGET\n")) == 0 )
			{
				string currentsay = "200 OK\n" + ReadData(mnum);  //retrieve Message of the Day from ReadData function
				mnum++;		//Increment so next request provides different message
				//Reset mnum if end of file reached
				if (mnum >= line_count)
				{
					mnum = 0;
				}
				//convert from string to char array so we can send through socket
				char msgres[MAX_LINE] = "";	//blank out msgres char array
				size_t length = currentsay.copy(msgres,MAX_LINE,0);
				msgres[length]='\n';	//add newline at end of Message of the Day
				msglen = strlen (msgres)+1;
				send (childSocket, msgres, msglen, 0);
				//debugging output
				//cout << endl << "buffer contains: " << msgres;	//Debug use only
						
			}
			
			
			else if( (strcmp(buf, "2\n")) == 0 || (strcmp(buf, "MSGSTORE\n")) == 0 )
			{
				//Append to MoD.txt
				//Check if user is logged in as general user or root
				if (( loggedin == 1) || (loggedin == 2))
				{
					char msgres[MAX_LINE] = "200 OK\n";
					msglen = strlen (msgres)+1;
					send (childSocket, msgres, msglen, 0);	//send 200 OK
					recv(childSocket, buf, sizeof(buf), 0);	//receive msg to store
					int buf1size = strlen(buf) + 1;	//size of second receive
					cout << "Client(MessageStore): " << buf <<endl<<"   size: "<<buf1size<<endl;	//Display received message and size
					//append buf1 text to file
					//return error if file can't open
					ofstream outfile;	//initialize "outfile"
					
					outfile.open("MoD.txt",ios::app);	//open MoD.txt for appending
					//send error if file can't open
					if ( outfile )
					{
						outfile << buf << endl;	//output buf1 contents
						char msgres1[MAX_LINE] = "200 OK\n";	//send confirmation
						msglen1 = strlen (msgres1)+1;	//Calculate size of buffer
						send (childSocket, msgres1, msglen1, 0);	//send message to client
					}
					else 
					{
						char msgres1[MAX_LINE] = "403 Error opening file\n";  //Send error msg if we can't find file
						msglen1 = strlen (msgres1)+1;	//Calculate size of buffer
						send (childSocket, msgres1, msglen1, 0);	//send message to client
					}
					
					outfile.close();			//close the open file
					
				}
				else
				{
					//access denied... must log in first
					char msgres[MAX_LINE] = "401 You are not currently logged in, login first.\n";
					msglen = strlen (msgres)+1;
					send (childSocket, msgres, msglen, 0);
				}
			}
			
//Josh - Done
			else if( (strcmp(buf, "3\n")) == 0 || (strcmp(buf, "LOGOUT\n")) == 0 )
			{
				char msgres[MAX_LINE] = "200 OK: Logged out\n";
				
				loggedin = 0;	//flip secure bit off
				msglen = strlen (msgres)+1;
				send (childSocket, msgres, msglen, 0); 
			}
//Gareth - Done	
			else if( (strcmp(buf, "4\n")) == 0 || (strcmp(buf, "QUIT\n")) == 0 )
			{
				char msgres[MAX_LINE] = "Quitting program...\n----All your base are belong to us!----\n";
				msglen = strlen (msgres)+1;
				send (childSocket, msgres, msglen, 0); 
				//client will close, server awaits new client
			}
			
			else if( (strcmp(buf, "5\n")) == 0 || (strcmp(buf, "SHUTDOWN\n")) == 0 )
			{
				if (loggedin == 2)
				{
					//Tell the client to turn off
					char msgres[MAX_LINE] = "200 OK\nShutting down the server interface...\nGoodbye!\n";
					msglen = strlen (msgres)+1;
					send (childSocket, msgres, msglen, 0); 
					//shutdown command: take care of shutting down server side items
					int shutdown(int childSocket, int how);
					
					//shutdowncmd = 1;
					
					//close all open sockets and files, then terminate.  In case of error the string
					//"300 message format error" should be returned.
					
					//send to all
					char shutd[MAX_LINE] = "\n210 The server is about to shutdown...\nGoodbye\n";
					msglen = strlen (shutd)+1;
					for(j = 0; j <= fdmax; j++) {
						// send to everyone!
						if (FD_ISSET(j, &master)) {
							// except the listener and ourselves
							if (j != listener && j != childSocket) {
								//cout << "j: " << j<< endl;
								if (send(j, shutd, msglen, 0) == -1) {
									perror("send");
								}
							}
						}
						//Shut all sockets
						int shutdown(int j, int how);
					}
					
					//Sockets are shutdown, so stop server program
					//if ( shutdowncmd == 1 )
					//{
						cout << "Client " << childSocket << " Initiated Shut Down" << endl;
						exit(1);
						cout << "300 message format error" << endl;  //Something went wrong if this happens
					//}	
					
				}	
				else
                {
                    char msgres[MAX_LINE] = "402 User not allowed to execute this command\n";
                    msglen = strlen (msgres)+1;
                    send (childSocket, msgres, msglen, 0);
                }
				
				
			}
//Gareth - Done
			else if( (strcmp(buf, "MENU\n")) == 0 || (strcmp(buf, "?\n")) == 0 )
			{
				//Provide help to client user: # or COMMANDS are acceptable
				char msgres[MAX_LINE] = "\n1) MSGGET: Get message\n2) MSGSTORE: Add message(Must be logged in)\n3) LOGOUT: Logout to server\n4) QUIT: Close client application\n5) SHUTDOWN: Shut down Server (Must be logged in)\n>) LOGIN USER PASS : Login to server with username and password\n>) WHO: Get a list of all logged-in users and IP address\n>) SEND username :Sends a message to the  specified user\n";
				msglen = strlen (msgres)+1;
				send (childSocket, msgres, msglen, 0);
			}
//Josh+Gareth
			//strstr searches the array for LOGIN; This allows for LOGIN and then variable user names
			else if( (strstr(buf, "LOGIN")) != NULL )
			{
				int buflen = strlen (buf)-1;	//Get length of string -1: where the \n is
				buf[buflen] = '\0';				//Get rid of \n so string compare works
				string bufstring = buf;			//Convert to string so we can pass to function
				/*if ((loggedin ==1) || (loggedin ==2))
				{
					
				}
				else
				{*/
				loggedin = UserLogin(bufstring,childSocket);//Find if the user is in our flat file
				//}
				
				//Send message of loggedin status
				if (( loggedin == 1) || (loggedin == 2))
				{
				//extract userid from buf and add to array
				
				string tokbuf;  //Buffer for token extract
				stringstream ss(bufstring);  //Insert string to stream
				
				vector<string> tokens;	//Vector to hold words;
				while (ss >> tokbuf)
					tokens.push_back(tokbuf);
				
				//connInfo[childSocket][1] = tokens[1];
				connUser[childSocket] = tokens[1];
				cout << "Client " << childSocket << ": logged in as " << connUser[childSocket] << endl;//tokarray[1] << endl;
				
				char msgres[MAX_LINE] = "200 OK: Logged In\n";
				msglen = strlen (msgres)+1;
				send (childSocket, msgres, msglen, 0);
				}
				else
				{
				char msgres[MAX_LINE] = "410 Wrong UserID or Password\n";
				msglen = strlen (msgres)+1;
				send (childSocket, msgres, msglen, 0);
				}
			}
			//WHO -- Josh
			else if( (strcmp(buf, "WHO\n")) == 0 )
			{
				//convert from string to char array so we can send through socket
				/*
				char msgres[MAX_LINE] = "";	//blank out msgres char array
				string listUsers = "200 OK\nThe list of active users:\njohn  127.0.0.1"
				size_t length = listUsers.copy(msgres,MAX_LINE,0);
				msgres[length]='\n';	//add newline at end of Message of the Day
				*/
				string listUsers = "200 OK\nThe list of active users:\n";
				
				int a;
				for(a = 4; a <= fdmax; a++)
				{
					string connUser1 = connInfo[a];
					if (connUser1.compare("disconnect") != 0)
					{
						//cout << connInfo[a][1] << "    " << connInfo[a][0] << endl;
						//cout << "ConnUser " << a << ": " << connUser[a] << endl;
						//cout << connUser[a] << "       " << connInfo[a] << endl;
						listUsers = listUsers + connUser[a]+ "       " + connInfo[a] + "\n";
					}
					//else
					//{
					//	cout << "Connuser " << a << ": Anonymous" << endl;
					//}
				}
				//need to build as an array
				
				//printf( "%s\n", connInfo ); cout << endl;
				
				char msgres[MAX_LINE] = "";	//blank out msgres char array
				//string listUsers = "200 OK\nThe list of active users:\n";
				
				size_t length = listUsers.copy(msgres,MAX_LINE,0);
				msgres[length]='\n';	//add newline at end of Message of the Day
				//cout << "length: " << length << endl;
				//cout << "msgres: " << msgres << endl;
				
				//char msgres[MAX_LINE] = "200 OK\nThe list of active users:\njohn		127.0.0.1\n";
				msglen = strlen (msgres)+1;
				send (childSocket, msgres, msglen, 0);
			}
			
			//SEND -- Gareth
			else if( (strstr(buf, "SEND")) != NULL )
			{
				//cout << "buf: " << buf << endl;
				
				//need to increment and find user
				//cout << "LOGGEDIN: " << logUser[0][4];
				
				//find all open sockets
				
				//cout << "LOGGEDIN: " << connInfo[0][childSocket] << endl;
				//cout << "ALL LoggedIN: " << logUser << endl;
				
				//need to string find user
				
				//need to return Login Socket
				int a;
				for(a = 4; a <= fdmax; a++)
				{
					//if ( (strstr(connInfo[a][0],"\n")) != NULL )
					//{
						
						//cout << "UsersLoggedIn: " << connInfo[a][1] << endl;
					//}
				}
				
				if ( 1==1 )
				{
					char msgres[MAX_LINE] = "200 OK\n";
					msglen = strlen (msgres)+1;
					send (childSocket, msgres, msglen, 0); //Send OK message
					recv(childSocket, buf, sizeof(buf), 0);	//receive msg to transmit
					int buf1size = strlen(buf) + 1;	//size of second receive
					cout << "Client(MessageRelay): " << buf << "   size: "<<buf1size<<endl;	//Display received message and size
					//cout << buf << endl;	//output buf contents
					char msgres1[MAX_LINE] = "200 OK\n";	//send confirmation
					msglen1 = strlen (msgres1)+1;	//Calculate size of buffer
					send (childSocket, msgres1, msglen1, 0);	//send message to client
					buf[sizeof(buf)-1] = '\n';
					send (5, buf, sizeof(buf), 0);
				}
				else 
				{
					char msgres[MAX_LINE] = "420 either the user does not exist or is not logged in\n"; //Send error msg if we can't find user
					msglen1 = strlen (msgres)+1;	//Calculate size of buffer
					send (childSocket, msgres, msglen1, 0);	//send message to another user
				}
				
				
			}
			
			else if( (strcmp(buf, "\n")) == 0 )
			{
				//If null, return null
				char msgres[MAX_LINE] = "\n";
				msglen = strlen (msgres)+1;
				send (childSocket, msgres, msglen, 0);
			}
			
			else  //Catch everything else; tell client we don't understand the option
			{
				cout << "Server: Not Recognized" <<endl;
				char msgres[MAX_LINE] = "Command not recognized\n--Type MENU or ? for available options--\n";
				msglen = strlen (msgres)+1;
				send (childSocket, msgres, msglen, 0);
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
	cout << "Server has been started...Awaiting connections\n";
    // main loop
    for(;;) {
        // handle new connections
		
        if ((newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen)) == -1) {
            perror("accept");
	        exit(1);
        } else {
            FD_SET(newfd, &master); // add to master set
            cout << "Server: new connection from "
		 		 << inet_ntoa(remoteaddr.sin_addr)
                 << " socket " << newfd << endl;
			//set the socket info here
			connInfo[newfd] = inet_ntoa(remoteaddr.sin_addr);
			connUser[newfd] = "Anonymous";
			
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

string ReadData(int a)
{
				string data[20];
				//char null[1];
				string line;
				line_count = 0;  //reset # of lines
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
int UserLogin(string b, int c)
{
				string lineUsers;		//read in value from file
				//string dataUsers[20];	//Array to hold all the users: 20 total
				int line_countUsers = 0;  //reset # of lines
				int userValid = 0;		//Flag: 1 if user is found in Array
				ifstream infile;
				infile.open("PASS");
				//Validate that file exists
				if ( ! infile )
				{
					cout << "Error opening file" << endl;	//Didn't find file: show at server level
				}
				else
				{
					while ( getline( infile, lineUsers ))
					{
						//Debugging output: Uncomment to view on server side
						/*cout << "Line_count: " << line_countUsers << endl;
						cout << "DataUsers: " << dataUsers[line_countUsers] << endl;
						cout << "test: " <<test<<endl;
						cout << "b: " << b << endl;*/
						if ( ("LOGIN " + lineUsers) == b){
							userValid = 1;	//Set User is Valid Flag; User will be logged in at end of function
							//connInfo[0][c] = lineUsers;  //add user login to connection array
							
						}
						if ( (b) == "LOGIN root root01")
						{
							userValid = 2; //SuperUser
							//rootuser = 1;
						}
						line_countUsers++;
					}
				}
				infile.close();	//Close the file after reading
				return userValid;	//Return 1 if valid or 0 if not found
}
