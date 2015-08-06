Connect 4 Client Server Game
Program where multiple clients can play against a server, over a network connection.
This program was written for a third year University of Melbourne subject (Computer Systems - COMP30023).
Server handles multiple clients connecting and playing simultaneously through threads. The program also makes use of mutex's to ensure concurrency across the log file.

Usage:

Run makefile to create executables: 'make server' and 'make client'

Must ensure the server program is run before the client program.
To run the two executable programs you must follow the protocol:
  
  server port_number
  client server_IP_address port_number
  
Where port_number is a port number you choose at run time to connect on (Must be the same for both server and client), and server_IP_address is the IP address of the machine which is running the server executable program.



