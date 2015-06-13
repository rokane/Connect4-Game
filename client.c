/* 
   Client for the classic game Connect-4
   Written by Ryan O'Kane: rokane - 587723
   COMP20023 - Computer Systems: Project 2A
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#include "connect4.h"



int main(int argc, char**argv)
{
	/* Connection Building Blocks */
	int sockfd, portno;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	/* Variables for Connect-4 Game */
	int client_move, server_move;
	c4_t board;

	/* Check for correct number of input args */
	if (argc < 3) 
	{
		fprintf(stderr,"usage %s hostname port\n", argv[0]);
		exit(0);
	}

	/* Assign Port number */
	portno = atoi(argv[2]);

	/* Translate host name into peer's IP address ;
	 * This is name translation service by the operating system 
	 */
	server = gethostbyname(argv[1]);
	
	/* Make sure translation worked */
	if (server == NULL) 
	{
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}

	/* Building data structures for socket */
	bzero((char *) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;

	bcopy((char *)server->h_addr, 
			(char *)&serv_addr.sin_addr.s_addr,
			server->h_length);

	serv_addr.sin_port = htons(portno);

	/* Create TCP socket -- active open 
	* Preliminary steps: Setup: creation of active open socket
	*/
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	/* Check to see if it all worked */
	if (sockfd < 0) 
	{
		perror("ERROR opening socket");
		exit(0);
	}
	
	/* Try to connect to the server address */
	if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
	{
		perror("ERROR connecting");
		exit(0);
	}

	/* Get ready to play the game - Initialize board */
	init_empty(board);
	print_config(board);

	/* Go through process of sending client move and 
	   recieving servers move, until the game is done */
	while((client_move = get_move(board)) != EOF)
	{
		/* Send the clients move to the server */
		send(sockfd, &client_move, sizeof(client_move), 0);

		/* process the client's move */
		if (do_move(board, client_move, YELLOW)!=1) {
			printf("Panic\n");
			exit(EXIT_FAILURE);
		}
		/* Print the current board */
		printf("Users move is: %d \n", client_move);
		print_config(board);

		/* Check to see if the client has won */
		if (winner_found(board) == YELLOW) {
			/* rats, the person beat us! */
			printf("Ok, you beat me, beginner's luck!\n");
			exit(EXIT_SUCCESS);
		}

		/* Recieve the move from server */
		recv(sockfd, &server_move, sizeof(server_move), 0);

		printf("Servers move is: %d \n", server_move);

		/* process the server's move */
		if (do_move(board, server_move, RED)!=1) {
			printf("Panic\n");
			exit(EXIT_FAILURE);
		}
		/* Print the current board */
		print_config(board);

		/* Check to see if the server won 
		   If server hasn't won then the game continues */
		if (winner_found(board) == RED) {
			/* rats, the person beat us! */
			printf("I guess I have your measure!\n");
			exit(EXIT_SUCCESS);
		}

	}

	return 0;

}