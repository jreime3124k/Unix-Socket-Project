/*
 * server.c
 */

#include <stdio.h>
#include <iostream>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <netdb.h>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <string>
//#include "msgget.h"


using namespace std;

#define SERVER_PORT 5556  //change to last 4/5 of UMID#
#define MAX_PENDING 5     //# of connections allowed
#define MAX_LINE 256	  //size of the character buffer
int line_count;				//set global variable for function
int rootuser = 0;			//Flag if root user	

int main(int argc, char **argv) {

    struct sockaddr_in sin;
    socklen_t addrlen;
    char buf[MAX_LINE];
	char buf1[MAX_LINE];
	int compstring;
	int len;
    int s;		//first socket
    int new_s;		//second socket
	int msglen;		//find length of char array to send
	int msglen1;
	int mnum = 0;		//Initialize to get first message
	int shutdowncmd =0; //Flag to signal server shutdown request
	int loggedin = 0;	//Flag for user logged in
	
	string ReadData(int a);	//Declare function for reading Message of the Day data
	int UserLogin(string b);
	
    /* build address data structure */
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;		//Internet Domain
    sin.sin_addr.s_addr = INADDR_ANY;	//Allow connection from anywhere(value=0)
    sin.sin_port = htons (SERVER_PORT);	//Host format to Network format on Port #

    /* setup passive open */
    if (( s = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
    }
	/* bind socket ^^ to port # */
    if ((bind(s, (struct sockaddr *) &sin, sizeof(sin))) < 0) {
		perror("bind");
		exit(1);
    }

    listen (s, MAX_PENDING);	//listen up to MAX_PENDING, i.e. 256 bytes

    addrlen = sizeof(sin);	
    cout << "The server is up, waiting for connection" << endl;

    /* wait for connection, then receive and print text */
    while (shutdowncmd != 1) {
//Client Info is included here
		if ((new_s = accept(s, (struct sockaddr *)&sin, &addrlen)) < 0) { 
			perror("accept");
			exit(1);
		}
//Output client details (IP Address)
		cout << "new connection from " << inet_ntoa(sin.sin_addr) << endl;
		
//Loop with client : buf is a Char array	
		while (len = recv(new_s, buf, sizeof(buf), 0)) {
			//Print message (buf) received from client
			cout << "Client: " << buf;
			
			//Begin Menu Choice from client input (buf)
//Gareth -- Done
//MSGGET
			if( (strcmp(buf, "1\n")) == 0 || (strcmp(buf, "MSGGET\n")) == 0 )
			{	
	
				//These two lines are for manually selecting a saying - Debug use only
				//cout << "Message #: "<< mnum << endl;
				//cin >> mnum;
				//End debug feature
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
				send (new_s, msgres, msglen, 0);
				//debugging output
				//cout << endl << "buffer contains: " << msgres;	//Debug use only
			}
//Gareth - Done
//MSGSTORE
			else if( (strcmp(buf, "2\n")) == 0 || (strcmp(buf, "MSGSTORE\n")) == 0 )
			{
				//Append to MoD.txt
				//Check if user is logged in
				if (loggedin == 1)
				{
					char msgres[MAX_LINE] = "200 OK\n";
					msglen = strlen (msgres)+1;
					send (new_s, msgres, msglen, 0);	//send 200 OK
					recv(new_s, buf1, sizeof(buf1), 0);	//receive msg to store
					int buf1size = strlen(buf1) + 1;	//size of second receive
					cout << "Client(MessageStore): " << buf1 <<endl<<"   size: "<<buf1size<<endl;	//Display received message and size
					//append buf1 text to file
					//return error if file can't open
					ofstream outfile;	//initialize "outfile"
					
					outfile.open("MoD.txt",ios::app);	//open MoD.txt for appending
					//send error if file can't open
					if ( outfile )
					{
						outfile << buf1 << endl;	//output buf1 contents
						char msgres1[MAX_LINE] = "200 OK\n";	//send confirmation
						msglen1 = strlen (msgres1)+1;	//Calculate size of buffer
						send (new_s, msgres1, msglen1, 0);	//send message to client
					}
					else 
					{
						char msgres1[MAX_LINE] = "403 Error opening file\n";
						msglen1 = strlen (msgres1)+1;	//Calculate size of buffer
						send (new_s, msgres1, msglen1, 0);	//send message to client
					}
					
					outfile.close();			//close the open file
					
				}
				else
				{
					char msgres[MAX_LINE] = "401 You are not currently logged in, login first.\n";
					msglen = strlen (msgres)+1;
					send (new_s, msgres, msglen, 0);
				}
			}
			
//Josh - Done
			else if( (strcmp(buf, "3\n")) == 0 || (strcmp(buf, "LOGOUT\n")) == 0 )
			{
				char msgres[MAX_LINE] = "200 OK: Logged out\n";
				
				loggedin = 0;	//flip secure bit off
				msglen = strlen (msgres)+1;
				send (new_s, msgres, msglen, 0); 
			}
//Gareth - Done	
			else if( (strcmp(buf, "4\n")) == 0 || (strcmp(buf, "QUIT\n")) == 0 )
			{
				//cout<< "Quitting program...\n----All your base are belong to us!----\n";
				char msgres[MAX_LINE] = "Quitting program...\n----All your base are belong to us!----\n";
				msglen = strlen (msgres)+1;
				send (new_s, msgres, msglen, 0); 
				//client will close, server awaits new client
			}
//Josh - Needs Work
			else if( (strcmp(buf, "5\n")) == 0 || (strcmp(buf, "SHUTDOWN\n")) == 0 )
			{
				if (rootuser == 1)
				{
					//Tell the client to turn off
					char msgres[MAX_LINE] = "Shutting down the server interface...\nGoodbye!\n";
					msglen = strlen (msgres)+1;
					send (new_s, msgres, msglen, 0); 
					//shutdown command: take care of shutting down server side items
					int shutdown(int new_s, int how);
					shutdowncmd = 1;
					//close all open sockets and files, then terminate.  In case of error the string
					//need "300 message format error" should be returned.
				}	
				else
                {
                    char msgres[MAX_LINE] = "402 User not allowed to execute this command\n";
                    msglen = strlen (msgres)+1;
                    send (new_s, msgres, msglen, 0);
                }
				
				
			}
//Gareth - Done
			else if( (strcmp(buf, "MENU\n")) == 0 || (strcmp(buf, "?\n")) == 0 )
			{
				//Provide help to client user: # or COMMANDS are acceptable
				char msgres[MAX_LINE] = "\n1) MSGGET: Get message\n2) MSGSTORE: Add message(Must be logged in)\n3) LOGOUT: Logout to server\n4) QUIT: Close client application\n5) SHUTDOWN: Shut down Server (Must be logged in)\n>) LOGIN USER PASS : Login to server with username and password\n";
				msglen = strlen (msgres)+1;
				send (new_s, msgres, msglen, 0);
			}
//Josh+Gareth
			//strstr searches the array for LOGIN; This allows for LOGIN and then variable user names
			else if( (strstr(buf, "LOGIN")) != NULL )
			{
				int buflen = strlen (buf)-1;	//Get length of string -1: where the \n is
				buf[buflen] = '\0';				//Get rid of \n so string compare works
				string bufstring = buf;			//Convert to string so we can pass to function
				loggedin = UserLogin(bufstring);//Find if the user is in our flat file
				
				//Send message of loggedin status
				if ( loggedin == 1)
				{
				char msgres[MAX_LINE] = "200 OK: Logged In\n";
				msglen = strlen (msgres)+1;
				send (new_s, msgres, msglen, 0);
				}
				else
				{
				char msgres[MAX_LINE] = "410 Wrong UserID or Password\n";
				msglen = strlen (msgres)+1;
				send (new_s, msgres, msglen, 0);
				}
			}
			else if( (strcmp(buf, "\n")) == 0 )
			{
				//If null, return null
				char msgres[MAX_LINE] = "\n";
				msglen = strlen (msgres)+1;
				send (new_s, msgres, msglen, 0);
			}
			else  //Catch everything else; tell client we don't understand the option
			{
				char msgres[MAX_LINE] = "Command not recognized\n--Type MENU or ? for available options--\n";
				msglen = strlen (msgres)+1;
				send (new_s, msgres, msglen, 0); 
			}
			
		}
		cout << "Connection from client (" << inet_ntoa(sin.sin_addr) << ")closed.\n";  //display which client disconnected
		close(new_s);
		if ( shutdowncmd == 1 )
		{
			cout << "Client(" << inet_ntoa(sin.sin_addr) << ") Initiated Shut Down" << endl;
		}		
		
    }
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
int UserLogin(string b)
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
						}
						if ( (b) == "LOGIN root root01")
						{
							rootuser = 1;
						}
						line_countUsers++;
					}
				}
				infile.close();	//Close the file after reading
				return userValid;	//Return 1 if valid or 0 if not found
}
