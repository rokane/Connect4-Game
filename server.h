/* 
   Header File for server.c
   Written by Ryan O'Kane: rokane - 587723
   COMP20023 - Computer Systems: Project 2A
*/

/* Data Structure containing connection info */
typedef struct
 {
 	int socket;							/* Socket File Descriptor */
 	struct sockaddr_in client_address;  /* Client Address info */
 	struct sockaddr_in serv_address;    /* Server Address info */

 } connection_t;

/* Function Declarations */
void log_connection(FILE* logfile, struct sockaddr_in client, int socket);
char* current_time();
void log_client_move(FILE* logfile, struct sockaddr_in client, int socket, 
	int move);
void log_server_move(FILE* logfile, struct sockaddr_in server, int socket, 
	int move);
void log_gameover(FILE* logfile, struct sockaddr_in client, int socket, 
	int status);
void* thread_game_handler(void* connection);