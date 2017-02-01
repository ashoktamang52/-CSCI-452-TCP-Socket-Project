/*

  ECHOCLNT.C
  ==========
  (c) Paul Griffiths, 1999
  Email: mail@paulgriffiths.net
  
  Simple TCP/IP echo client.

*/


#include <sys/socket.h>       /*  socket definitions        */
#include <netinet/in.h>
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */

#include "helper.h"           /*  Our own helper functions  */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


/*  Global constants  */

#define MAX_LINE           (1000)


/*  Function declarations  */

int ParseCmdLine(int argc, char *argv[], char **szAddress, char **szPort);


/*  main()  */

int main(int argc, char *argv[]) {

    int       conn_s;                     /*  connection socket                  */
    short int port;                       /*  port number                        */
    struct    sockaddr_in servaddr;       /*  socket address structure           */
    char      buffer[MAX_LINE];           /*  character buffer                   */
    char      buffer_send[MAX_LINE];      /*  Holds message to be send to server */
    char      buffer_received[MAX_LINE];  /*  Holds message send by server       */
    char     *szAddress;                  /*  Holds remote IP address            */
    char     *szPort;                     /*  Holds remote port                  */
    char     *endptr;                     /*  for strtol()                       */


    /*  Get command line arguments  */

    ParseCmdLine(argc, argv, &szAddress, &szPort);


    /*  Set the remote port  */

    port = strtol(szPort, &endptr, 0);
    if ( *endptr ) {
    	printf("ECHOCLNT: Invalid port supplied.\n");
    	exit(EXIT_FAILURE);
    }
	

    /*  Create the listening socket  */

    if ( (conn_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
    	fprintf(stderr, "ECHOCLNT: Error creating listening socket.\n");
    	exit(EXIT_FAILURE);
    } else {
        fprintf(stderr, "Listening socket created.\n");
    }


    /*  Set all bytes in socket address structure to
        zero, and fill in the relevant data members   */

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons(port);


    /*  Set the remote IP address  */

    if ( inet_aton(szAddress, &servaddr.sin_addr) <= 0 ) {
    	printf("ECHOCLNT: Invalid remote IP address.\n");
    	exit(EXIT_FAILURE);
    } else {
        fprintf(stderr, "IP address set.\n");
    }

    
    /*  connect() to the remote echo server  */

    if ( connect(conn_s, (struct sockaddr *) &servaddr, sizeof(servaddr) ) < 0 ) {
    	printf("ECHOCLNT: Error calling connect()\n");
    	exit(EXIT_FAILURE);
    } else {
        fprintf(stderr, "Connected to the server.\n");
    }


    /*  Get string to follow user commands */
    do {
        printf("Insert your command: ");
        fgets(buffer, MAX_LINE, stdin);
        printf("%s", buffer);

        if (strncmp(buffer, "s", 1) == 0) {
            printf("\nPlease Enter a string: ");
            fgets(buffer, MAX_LINE, stdin);
            fprintf(stderr, "Buffer %s\n", buffer);

            /* Format the input string */

            strcpy(buffer_send, "CAP\n");
            strcat(buffer_send, buffer);
            strcat(buffer_send, "\n");
            fprintf(stderr, "%s\n", buffer_send);

            write(conn_s, buffer_send, strlen(buffer_send));
            read(conn_s, buffer_received, MAX_LINE-1);

            memcpy(buffer, buffer_received + 2, strlen(buffer_received) - 2);
            printf("Server responded: %s", buffer);
        }
        else if (strncmp(buffer, "t", 1) == 0) {
            printf("\nPlease Enter a string: ");
            fgets(buffer, MAX_LINE, stdin);


            FILE *fp; /*file pointer*/
            fprintf(stderr, "Real file name?: %d\n", strlen(buffer));
            char temp[strlen(buffer) - 1];
            memcpy(temp, buffer, strlen(buffer) - 1);

            

            /* Format the input string */
            strcpy(buffer_send, "FILE\n");
            strcat(buffer_send, buffer);
            fprintf(stderr, "Length %d\n", strlen(buffer_send));
            fprintf(stderr, "%s\n", buffer_send);

            /* Send message to server. */
            write(conn_s, buffer_send, strlen(buffer_send));

            /* Read message from server. */
            read(conn_s, buffer_received, MAX_LINE-1);
            fprintf(stderr, "Server responded: %s\n", buffer_received);
            fprintf(stderr, "Buffer size: %d\n", strlen(buffer_received));

            /* write the data to the file. */
            if (strncmp(buffer_received + 2, "NOT FOUND", 9) == 0) {
                printf("%s\n", buffer_received + 2);
            }
            else {
                fp = fopen(temp, "wb");
                fwrite(buffer_received, 1, strlen(buffer_received), fp);
                fclose(fp);
            }
            

        }
        else if (strncmp(buffer, "q", 1) == 0) {
            fprintf(stderr, "Now should exit.\n");
            if (close(conn_s) < 0 ) {
                fprintf(stderr, "ECHOSERV: Error calling close()\n");
                exit(EXIT_FAILURE);
            }
            return EXIT_SUCCESS;
        }
        else
            printf("\nInvalid Command: Press 's' for echo, 't' for file storage and 'q' for exit.\n");
    } while (strncmp(buffer, "q", 1) != 0);
    

    /*  Send string to echo server, and retrieve response  */
    /*
    Readline(conn_s, buffer_received, MAX_LINE-1);
    printf("Server responded: %s", buffer_received);
    */

    


    /*  Output echoed string  */

    printf("Echo response: %s\n", buffer_received);

    return EXIT_SUCCESS;
}


int ParseCmdLine(int argc, char *argv[], char **szAddress, char **szPort) {

    int n = 1;

    while ( n < argc ) {
	if ( !strncmp(argv[n], "-a", 2) || !strncmp(argv[n], "-A", 2) ) {
	    *szAddress = argv[++n];
	}
	else if ( !strncmp(argv[n], "-p", 2) || !strncmp(argv[n], "-P", 2) ) {
	    *szPort = argv[++n];
	}
	else if ( !strncmp(argv[n], "-h", 2) || !strncmp(argv[n], "-H", 2) ) {
	    printf("Usage:\n\n");
	    printf("    timeclnt -a (remote IP) -p (remote port)\n\n");
	    exit(EXIT_SUCCESS);
	}
	++n;
    }

    return 0;
}

