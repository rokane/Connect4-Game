/* 
   Game Server for the classic game Connect-4
   Written by Ryan O'Kane: rokane - 587723
   COMP20023 - Computer Systems: Project 2A
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <pthread.h>

#include "connect4.h"
#include "server.h"

/* Codes indicating how game ended */
#define STATUS_UNFINISHED   1
#define STATUS_ILLEGAL_MOVE 2
#define STATUS_USER_WON     3
#define STATUS_AI_WON       4
#define STATUS_DRAW         5

/* Lock for ensuring conncurency with sychronized thread processing */
pthread_mutex_t lock;

int main(int argc, char **argv)
{

	int sockfd, portno, clilen;

	/* Client & Server address information block */
	struct sockaddr_in serv_addr, cli_addr;

	/* Struct to pass in connection info into thread */
	connection_t *connection;

	/* Logfile which Server writes to during execution */
	FILE *logfile;

	/* Thread ID */
	pthread_t tid;
	

	/* Initialize mutex lock */
	if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        exit(1);
    }	
	
	/* Open logfile */
	logfile = fopen("log.txt", "w");

	/* Make sure it opened */
	if(logfile == NULL){

		printf("Error opening the logfile \n");
		exit(0);
	}

	/* Need 2 input arguments: execuatble, port_number */
	if (argc < 2) 
	{
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}

	/* Create TCP Socket - Using IPv4 */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	/* Returns positive int if successful */
	if (sockfd < 0) 
	{
		perror("ERROR opening socket");
		exit(1);
	}

	/* Zero all bytes in server address */
	bzero((char *) &serv_addr, sizeof(serv_addr));

	/* Establish the port number */
	portno = atoi(argv[1]);

	/* Create address we're going to listen on (given port number)
	 - converted to network byte order & any IP address for 
	 this machine */
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	/* Store Port in machine-neutral format */
	serv_addr.sin_port = htons(portno);  

	/* Bind address to the socket */
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))<0) 
	{
		perror("ERROR on binding");
		exit(1);
	}

	/* Listen on socket for incoming connections from clients */
	listen(sockfd,5);

	clilen = sizeof(struct sockaddr_in);


	/* Accept  connections - Create a new thread and play game */
	while(1)
	{
		/* Create connection data structure to pass to thread */
		connection = (connection_t *)malloc(sizeof(connection_t));

		/* Assign new socket to connection */
		connection->socket = accept(sockfd, (struct sockaddr *)&cli_addr, 
										&clilen);

		/* Keep track of Client and Server addresses for printing */
		connection->client_address = cli_addr;
		connection->serv_address = serv_addr;

		/* Check to see if socket is there */
		if (connection->socket <= 0)
	    {
	        free(connection);
        }
        else
	    {
	    	/* Make a log entry - New connection */
	    	fopen("log.txt","a+");
			log_connection(logfile, connection->client_address, 
								connection->socket);

	        /* start a new thread to handle game, but continue running */
	        pthread_create(&tid, 0, thread_game_handler, (void *)connection);
	        pthread_detach(tid);
	    }
	    
	    fclose(logfile);

	}

	/* Destroy mutex lock once finished */
	pthread_mutex_destroy(&lock);
	
	


	return 0;

}

/* Function logs a client connection in logfile */
void log_connection(FILE* logfile, struct sockaddr_in client, int socket)
{
	/* Lock to ensure no other threads are writing to logfile */
	pthread_mutex_lock(&lock);

	/* Convert address into readable form */
	char address[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(client.sin_addr), address, INET_ADDRSTRLEN);

	fprintf(logfile, "(%s)(%s)(soc_id %d) client connected \n", 
		current_time(), address, socket);

	fflush(logfile);

	/* Unlock when finished writing */
	pthread_mutex_unlock(&lock);

}

/* Returns the current time */
char* current_time()
{
	time_t current_time;
	char* time_string;

	/* Obtain current time as seconds elapsed since the Epoch. */
    current_time = time(NULL);

    /* Convert to local time format. */
    time_string = ctime(&current_time);
    
    /* Take off \n at end of the string */
    size_t index = strlen(time_string);
    time_string[index - 1] = '\0';


	return time_string;
}

/* Function logs a client move in logfile */
void log_client_move(FILE* logfile, struct sockaddr_in client, int socket, 
						int move)
{
	/* Lock to ensure no other threads are writing to logfile */
	pthread_mutex_lock(&lock);

	/* Convert address into readable form */
	char address[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(client.sin_addr), address, INET_ADDRSTRLEN);

	fprintf(logfile, "(%s)(%s)(soc_id %d) client's move = %d \n",
		current_time(), address, socket, move);

	fflush(logfile);

	/* Unlock when finished writing */
	pthread_mutex_unlock(&lock);
}

/* Function logs a servers move to logfile */
void log_server_move(FILE* logfile, struct sockaddr_in server, int socket, 
						int move)
{
	/* Lock to ensure no other threads are writing to logfile */
	pthread_mutex_lock(&lock);

	/* Convert address into readable form */
	char address[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(server.sin_addr), address, INET_ADDRSTRLEN);

	fprintf(logfile, "(%s)(%s)(soc_id %d) server's move = %d \n",
		current_time(), address, socket, move);

	fflush(logfile);

	/* Unlock when finished writing */
	pthread_mutex_unlock(&lock);
}

/* Function logs the end of a game - Status indicates result of game */
void log_gameover(FILE* logfile, struct sockaddr_in client, int socket, 
						int status)
{
	/* Lock to ensure no other threads are writing to logfile */
	pthread_mutex_lock(&lock);

	/* Convert address into readable form */
	char address[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(client.sin_addr), address, INET_ADDRSTRLEN);

	fprintf(logfile, "(%s)(%s)(soc_id %d) game over, code = %d \n",
		current_time(), address, socket, status);

	fflush(logfile);

	/* Unlock when finished writing */
	pthread_mutex_unlock(&lock);
}

/* Connect 4 game - Thread will run when created */
void* thread_game_handler(void* connection)
{
	/* Int values corresponding to column of move in game */
	int server_move, client_move;

	/* Game Board */
	c4_t board;

	FILE* logfile = fopen("log.txt", "a+");

	connection_t *client_conn;

	/* Check connection parameter was passed in */
	if(!connection){
		pthread_exit(0);
	}
	else{
		/* Cast void pointer to a struct we need */
		client_conn = (connection_t *)connection;
	}

	/* Get ready to play the game - Initialize board */
	srand(RSEED);
	init_empty(board);

	/* Continue playing until the game has finished */
	while(1)
	{
		/* Wait until client tells us their move */
		if(recv(client_conn->socket, &client_move, sizeof(client_move),0)==0){
			
			/* Log entry - Unfinished Game */
			log_gameover(logfile, client_conn->client_address, 
				client_conn->socket, STATUS_UNFINISHED);
			break;
		}

		/* process the client's move */
		if (do_move(board, client_move, YELLOW)!=1) {
			
			/* Log entry - Illegal Move */
			log_gameover(logfile, client_conn->client_address, 
				client_conn->socket, STATUS_ILLEGAL_MOVE);
			break;
		}

		/* Log entry - Clients move */
		log_client_move(logfile, client_conn->client_address,
		 	client_conn->socket, client_move);

		/* Check to see if the client has won */
		if (winner_found(board) == YELLOW) {

			/* Log entry - User won */
			log_gameover(logfile, client_conn->client_address, 
				client_conn->socket, STATUS_USER_WON);
			break;
		}

		/* was that the last possible move? */
		if (!move_possible(board)) {

			/* Log entry - Drawn Game */
			log_gameover(logfile, client_conn->client_address,
				 client_conn->socket, STATUS_DRAW);
			break;
		}

		/* Perform the server move */
		server_move = suggest_move(board, RED);

		/* process the server's move */
		if (do_move(board, server_move, RED)!=1) {
			
			/* Log entry - Illegal Move */
			log_gameover(logfile, client_conn->client_address, 
				client_conn->socket, STATUS_ILLEGAL_MOVE);
			break;
		}

		/* Log entry - Server move */
		log_server_move(logfile, client_conn->serv_address, 
			client_conn->socket, server_move);

		/* Send server move to the client */
		send(client_conn->socket, &server_move, sizeof(server_move), 0);

		/* Check to see if server has won
		   If not the game continues */
		if (winner_found(board) == RED) {

			/* Log entry - Server won */
			log_gameover(logfile, client_conn->client_address,
				 client_conn->socket, STATUS_AI_WON);
			break;
		}


	}
	/* Close log file and Connection */
	fclose(logfile);
	close(client_conn->socket);
	free(client_conn);
	
	/* Exit thread once loop is broken and game is finsihed */
	pthread_exit(0);


}














