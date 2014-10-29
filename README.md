Unix-Socket-Project
===================
----------
Intro
----------
Socket Program 2 CIS 527
Version 2.0
Fall 14
Gareth Lawlor
Joshua Reimer
Responsibilites will be designated next to each functional area below and in greater detail at the bottom of the document.
Version 2.0 introduces MultiThreading and supports multiple clients to connect at a time.  SEND and WHO functions are also added.

-----------------
Table of Contents
-----------------
Intro
TOC
Functions
Instructions
Bugs
Sample Outputs
Files
Build Instructions
Responsibilities

----------
Functions:
----------
--MSGGET--Gareth
Reads a message from a file on the server and responds contents to the client.  It cycles through five pre-set messages sequentially and any added through the MSGSTORE command. It will start back at the beginning when it reaches the end.  There is a sub-function implemented in MSGGET command, ReadData, where it reads a string from the MoD.txt file and returns it to the MSGGET section were it is sent through the socket to the client.

--MSGSTORE--Gareth
Allows logged in users to add a message of the day to the MoD.txt file on the server.  Users will receive an error if they are not logged in.

--WHO--
List all active users, including the UserID and the users IP addresses.

--SEND--
Send a private message to an active user.  If the UserID is invalid or the receiver is not active, the server replies the client with an error message “420 either the user does not exist or is not logged in”; otherwise, the server forwards the message to the designated user.  In addition, the receiving client should process the message immediately.

--LOGOUT--Josh
Logs the user out of the system.

--QUIT--Gareth
Closes the client.  The server will wait for another connection.

--SHUTDOWN--Josh
Requires the user to be logged in as root.  The client will close and the server shutsdown all sockets and exits the program.

--LOGIN--Gareth+Josh
The user logs in with the format of LOGIN userid password.  Logged in users can add messages to the MSGSTORE and root may shutdown the server.

---------------------------------------
Instructions on how to run the program:
---------------------------------------
You will need two Linux/Unix terminal sessions.  Server can run without arguments, but client will need to be supplied with an IP address.  If it is the same Linux box for both you can use localhost or 127.0.0.1.  If the client is on a different box then ./client login.umd.umich.edu can be used.
The port number is a hard coded constant with 5556 as the value (SID).

--On server type ./server to start the listener
--On client type ./client IP ADDRESS, i.e. ./client 127.0.0.1

The program will run with the commands listed in the assignment, or you may use the corresponding numbers in the menu.  When the user types MENU or ? a list of commands and corresponding numbers will be displayed.

CLIENT: ?
SERVER:
1) MSGGET: Get message
2) MSGSTORE: Add message(Must be logged in)
3) LOGOUT: Logout to server
4) QUIT: Close client application
5) SHUTDOWN: Shut down Server (Must be logged in)
>) LOGIN USER PASS : Login to server with username and password
CLIENT:
*see the sample outputs for line by line detail on how the program works.

------
Bugs: 
------
There are no known bugs.

---------------
Sample outputs:
---------------
~~~~~output from a LOGIN command
CLIENT: LOGIN john john01
SERVER: 200 OK Logged In

CLIENT: 1
SERVER: 200 OK (returns the first message in the file)

CLIENT: 2
SERVER: 200 OK 
CLIENT: Logged In, Send buffer


CLIENT: 3
SERVER: 200 OK: Logged out

CLIENT: 4
SERVER: Quitting program…

~~~~~output from a non root user trying to run the shutdown command
Client: 5
SERVER: 402 User not allowed to execute this command

~~~~~output from a root user running the shutdown command
CLIENT: 5
SERVER: Shutting down the server interface …
Goodbye!
CLIENT: Server Shutdown
--At other clients
s: 210 the server is about to shutdown ......

~~~~~output from a logged in user sending a message to be written to the file and then retrieving it.

Client: LOGIN john john01
SERVER: 200 OK: Logged In
Client: 2
SERVER: 200 OK
Client: Logged In, Send buffer
today is the first day of the rest of your life
SERVER: 200 OK

Client: 1
SERVER: 200 OK
It is impossible to build a fool proof system; because fools are so ingenious.
Client: 1
SERVER: 200 OK
Information travels more surely to those with a lesser need to know.
Client: 1
SERVER: 200 OK
A bug in the code is worth two in the documentation.
Client: 1
SERVER: 200 OK
A computer scientist is someone who fixes things that aren't broken.
Client: 1
SERVER: 200 OK
The answer to the ultimate question of life, the universe and everything is 42. ~Douglas Adams, The Hitchhiker's Guide to the Galaxy~
Client: 1
SERVER: 200 OK
today is the first day of the rest of your life

~~~~~output from WHO command
c: WHO
s: 200 OK
  The list of the active users:
  john    141.215.10.30
  root    127.0.0.1
  
~~~~~output from a user running the SEND command
--David's window
c: SEND john
s: 200 OK
c: Hello John
s: 200 OK
--John's window
s: 200 OK you have a message from david
david: Hello John

--------------
Files Included
--------------
server.c	Listener program that performs functions as requested
client.c	Client program that sends commands to server
README.txt	This file
Makefile	Makefile to build server/client
MoD.txt		Text file that contains the Messages of the Day
PASS		Text file that contains the userids and passwords of all users

------------------
Build Instructions
------------------
Build in Unix by extracting the tar file to a directory and then typing "make".
If you wish to clean the directory, "make clean" will erase all object and program files.

----------------
Responsibilities
----------------
The project was split amounst us fairly as we had varying experience with C\C++.
We both refined each other's work with editing and validating code, as well as debugging.
SEND      --??
WHO       --??
MultiThread --??
MSGGET		--Gareth
MSGSTORE	--Gareth
LOGOUT		--Josh
QUIT		--Gareth
SHUTDOWN	--Josh
LOGIN		--Gareth+Josh
Readme 		--Josh+Gareth
PASS		--Gareth
MoD		--Gareth
