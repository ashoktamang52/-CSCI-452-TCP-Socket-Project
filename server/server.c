/*

  ECHOSERV.C
  ==========
  (c) Paul Griffiths, 1999
  Email: mail@paulgriffiths.net
  
  Simple TCP/IP echo server.

*/


#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <sys/time.h>
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */

#include "helper.h"           /*  our own helper functions  */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


/*  Global constants  */

#define ECHO_PORT          (2002)
#define MAX_LINE           (1000)


int main(int argc, char *argv[]) {
    int       list_s;                /*  listening socket          */
    int       conn_s;                /*  connection socket         */
    short int port;                  /*  port number               */
    struct    sockaddr_in servaddr;  /*  socket address structure  */
    char      buffer[MAX_LINE];      /*  character buffer          */
    char     *endptr;                /*  for strtol()              */

    /* Waiting for socket to be ready to send data */
    fd_set fds;
    struct timeval timeout;
    int rc, result;


    /*  Get port number from the command line, and
        set to default port if no arguments were supplied  */

    if ( argc == 2 ) {
    	port = strtol(argv[1], &endptr, 0);
    	if ( *endptr ) {
    	    fprintf(stderr, "ECHOSERV: Invalid port number.\n");
    	    exit(EXIT_FAILURE);
	   }
    }
    else if ( argc < 2 ) {
	   port = ECHO_PORT;
    }
    else {
    	fprintf(stderr, "ECHOSERV: Invalid arguments.\n");
    	exit(EXIT_FAILURE);
    }

	
    /*  Create the listening socket  */

    if ((list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
    	perror("ECHOSERV: Error creating listening socket.\n");

    	exit(EXIT_FAILURE);
        }


    /*  Set all bytes in socket address structure to
        zero, and fill in the relevant data members   */

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(port);


    /*  Bind our socket addresss to the 
	listening socket, and call listen()  */

    if ( bind(list_s, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) {
    	fprintf(stderr, "ECHOSERV: Error calling bind()\n");
    	exit(EXIT_FAILURE);
        }

    if ( listen(list_s, LISTENQ) < 0 ) {
    	perror("ECHOSERV: Error calling listen()\n");
    	exit(EXIT_FAILURE);
        }

    
    /*  Enter an infinite loop to respond
        to client requests and echo input  */

    while ( 1 ) {
        fprintf(stderr, "Does it go here?\n");

    	/*  Wait for a connection, then accept() it  */

    	if ( (conn_s = accept(list_s, NULL, NULL) ) < 0 ) {
    	    fprintf(stderr, "ECHOSERV: Error calling accept()\n");
    	    exit(EXIT_FAILURE);
    	}

            /*  Retrieve an input line from the connected socket
            then simply write it back to the  same socket.     */
        char capitalize[4];
        strcpy(capitalize, "CAP\n");

        /* use select() to wait for socket to be ready to send data */
        timeout.tv_sec = 3; timeout.tv_usec = 0;
        FD_ZERO(&fds); FD_SET(conn_s, &fds);
        rc = select(sizeof(fds) * 8, &fds, NULL, NULL, &timeout);
        if (rc == - 1) {
            perror("select failed");
            return -1;
        }
        else if (rc > 0) {
            if (FD_ISSET(conn_s, &fds)) {
                Readline(conn_s, buffer, MAX_LINE-1);
                fprintf(stderr, "Buffered %s\n", buffer);
                Writeline(conn_s, buffer, strlen(buffer));
            }
        }
        
        /* while (strncmp(buffer, capitalize, 4) && strncmp(buffer[strlen(buffer) - 1], "\n", 1)) { */
            
            /* When client wants to capitalize the string */
            
        
                
        
        fprintf(stderr, "Buffered %s\n", buffer);
        /* Writeline(conn_s, buffer, strlen(buffer)); */

    	/*  Close the connected socket  */
    	if ( close(conn_s) < 0 ) {
    	    perror("ECHOSERV: Error calling close()\n");
    	    exit(EXIT_FAILURE);
    	}
        else {
            fprintf(stderr, "Connection closed.\n");
        }
    }
}
