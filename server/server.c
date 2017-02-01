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
#include <ctype.h>
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
    char      buffer_send[MAX_LINE];
    char     *endptr;                /*  for strtol()              */


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

        while (1) {
            /*Readline(conn_s, buffer, MAX_LINE-1);*/
            read(conn_s, buffer, MAX_LINE-1);
            fprintf(stderr, "Buffered %s\n", buffer);

            fprintf(stderr, "Buffer length %d\n", strlen(buffer));

            if (strncmp(buffer, "CAP", 3) == 0) {
                /* number of relevant bytes of message 
                   = buffer length - 'CAP' length - length of two line breaks - end of string 
                */

                char to_capitalize[strlen(buffer) - 6];
                memcpy(to_capitalize, buffer + 4, strlen(buffer) - 6);
                fprintf(stderr, "Real message please?: %s\n", to_capitalize);

                /* Capitalize the messsage */
                int index = 0;
                while (to_capitalize[index] != '\0') {
                    if (islower(to_capitalize[index])) {
                        to_capitalize[index] = toupper(to_capitalize[index]);
                    }
                    else {
                        to_capitalize[index] = to_capitalize[index];
                    }
                    index++;
                }
                fprintf(stderr, "Captialled: %s\n", to_capitalize);

                /* parse the capitalized message to send to the client */
                
                sprintf(buffer_send, "%d", strlen(to_capitalize));
                strcat(buffer_send, "\n");
                strcat(buffer_send, to_capitalize);
                fprintf(stderr, "to be sent: %s\n", buffer_send);

                /* send the formatted message to the client */
                write(conn_s, buffer_send, strlen(buffer_send));
            }

            if (strncmp(buffer, "FILE", 4) == 0) {
                char file_name[strlen(buffer) - 7];
                memcpy(file_name, buffer + 5, strlen(buffer) - 7);
                fprintf(stderr, "File Name should be: %s\n", file_name);
                fprintf(stderr, "Length %d\n", strlen(file_name));

                /* Find file name and read that file */
                FILE *fp; /* file pointer */
                fp = fopen(file_name, "rb");
                if (fp) {
                    long lSize;
                    char* large_buffer;
                    size_t result;

                    /* obtain file size */
                    fseek(fp, 0, SEEK_END);
                    lSize = ftell(fp);
                    rewind(fp); /* Put the postion of pointer back to the start of the file */

                    /* allocate memory to hold the whole file */
                    large_buffer = (void* ) malloc(sizeof(void*) * lSize);
                    if (large_buffer == NULL) {
                        perror("Error in allocating required memory: ");
                    }

                    /* copy the file into the buffer */
                    result = fread(large_buffer, 1, lSize, fp);
                    if (result != lSize) {
                        printf("Error reading whole file.\n");
                    }

                    /* close the file and later free the large_buffer */
                    fclose(fp);

                    fprintf(stderr, "The read large bufffer: %s\n", large_buffer);

                    /* send the buffer to the client */
                    write(conn_s, large_buffer, lSize);



                } else {
                    /* No such file */
                    fprintf(stderr, "Not Found\n");
                    strcpy(buffer, "NOT FOUND");
                    sprintf(buffer_send, "%d", strlen(buffer));
                    strcat(buffer_send, buffer);

                    /* Inform client that file is not in the server. */
                    write(conn_s, buffer_send, strlen(buffer_send));
                }

            }
        }
            
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
