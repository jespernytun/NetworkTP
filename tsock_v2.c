/* librairie standard ... */
#include <stdlib.h>
/* pour getopt */
#include <unistd.h>
/* déclaration des types de base */
#include <sys/types.h>
/* constantes relatives aux domaines, types et protocoles */
#include <sys/socket.h>
/* constantes et structures propres au domaine UNIX */
#include <sys/un.h>
/* constantes et structures propres au domaine INTERNET */
#include <netinet/in.h>
/* structures retournées par les fonctions de gestion de la base de
données du réseau */
#include <netdb.h>
/* pour les entrées/sorties */
#include <stdio.h>
/* pour la gestion des erreurs */
#include <errno.h>
#include <string.h>

#define MAX_MSG_LEN 1000

/* Prototypes */
void client_udp(int port, char* hostname, int nbmsg, int lgmsg); /* creates a udp client */
void client_tcp(int port, char* hostname, int nbmsg, int lgmsg); /* creates a tcp client */
void server_udp(int port, int nbmsg); /* creates udp server */
void server_tcp(int port, int nbmsg); /* creates tcp server */


int main (int argc, char **argv) {
	int c;
	extern char *optarg;
	extern int optind;
	int nb_message = -1; /* number of messages to send, 10 by default */
   int lg_message = -1; /* length of messages, 30 by default */
	int source = -1 ; /* 0=reciever, 1=source */
   
   int udp = 0; /* We define a variable udp to handle the flag -u */
   int port;
   char* hostname;
   
   /* getopt to check for flags when tsock is called upon*/
	while ((c = getopt(argc, argv, "psun:l:")) != -1) {
		switch (c) {
		case 'p': /* if reciever flag */
			if (source == 1) {
				printf("usage: cmd [-p|-s][-n ##][-l ##][-u]\n");
				exit(1);
			}
			source = 0; /* then not source */
			break;

		case 's': /* if source flag */
			if (source == 0) {
				printf("usage: cmd [-p|-s][-n ##][-l ##][-u]\n");
				exit(1) ;
			}
			source = 1; /* then source  */
			break;

		case 'n': /* flag to check for nb of messages */
			nb_message = atoi(optarg);
			break;

		case 'l': /* flag to check for msg length */ 
			lg_message = atoi(optarg);
			break;         

      case 'u': /* check if UDP */
        udp = 1;
        break;
        
      /* if command not compliant, print instruction to user */
		default:
			printf("usage: cmd [-p|-s][-n ##][-l ##][-u]\n");
			break;
		}
	}

   /* if not decided, error in function call */
	if (source == -1) {
		printf("usage: cmd [-p|-s][-n ##][-l ##][-u]\n");    
		exit(1) ;
	}

   /* if nb_messages define, print how many to send/recieve (10/inf by default)*/
   if (nb_message != -1) {
     if (source == 1)
       printf("nb of messages to send : %d\n", nb_message);
     else
			printf("nb of messages to recieve : %d\n", nb_message);
   } else {
     if (source == 1) {
       printf("nb of messages to send = 10 by default\n");
     } else
       printf("nb of messages to recieve = inf\n");
   }

   /* if UDP */
   if (udp == 1) {
     printf("We're using UDP\n");
     if (source == 1) {
       printf("We're on the source\n");

       if (lg_message == -1) {lg_message = 30;}
       if (nb_message == -1) {nb_message = 10;}
       
       port = atoi(argv[argc-1]); /* We assign last argument as nb port*/
       hostname = argv[argc-2]; /* We assign second to last argument as hostname*/

       /* we call on our client function */
       client_udp(port, hostname, nb_message, lg_message);

     } else { 
       printf("We're on the reciever\n");

       port = atoi(argv[argc-1]); /* We assign last argument as nb port */

       server_udp(port, nb_message); 
     }
            
   } else { /* else TCP */
     printf("We're using TCP\n");
     if (source == 1) {
       printf("We're on the source\n");

       if (lg_message == -1) {lg_message = 30;}
       if (nb_message == -1) {nb_message = 10;}
       
       port = atoi(argv[argc-1]); /* We assign last argument as nb port*/
       hostname = argv[argc-2]; /* We assign second to last argument as hostname*/

       /* we call on our client function */
       client_tcp(port, hostname, nb_message, lg_message);

     } else { 
       printf("We're on the reciever\n");
       
       port = atoi(argv[argc-1]); /* We assign last argument as nb port */

       server_tcp(port, nb_message); 
      }
   }

   return 0;
}
/*---------------------------------------------------------------------------------*/
/* -----------------------UDP client and server----------------------------------- */
/*---------------------------------------------------------------------------------*/
void client_udp(int port, char* hostname, int nbmsg, int lgmsg){

  /* definition of constants and variables*/
  int sock;
  struct sockaddr_in servaddr;
  socklen_t lg_adr_dist = sizeof(servaddr);
  char M[lgmsg];
  char charmsg = 'a'; /* character that will be sent */
  struct hostent* server;
  int i = 0; /* increment */
  
  /* ---------------------creating the socket of designated specs---------------*/
  if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }
  
  /* -----------------------------create host address --------------------------*/
  /* using gethostbyname to find server hostent */
  if ((server = gethostbyname(hostname)) == NULL) {
    fprintf(stderr, "host not found\n");
    exit(1);
  }
  /* assigning values found finalizing socket */
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);
  memcpy(&servaddr.sin_addr, server->h_addr, server->h_length);

  /*---------------------sending nbmsg of messages------------------------------*/
  for (i=0; i<nbmsg; i++) {
    memset(&M, charmsg, lgmsg);
    if (sendto(sock, M, lgmsg, 0, (struct sockaddr *)&servaddr, lg_adr_dist) < 0) {
      perror("sendto");
    }
    charmsg++;
    if (charmsg > 'z') charmsg = 'a'; /* Make sure we print characters and not whatever*/
  }
  close(sock);
}

/*-------------------function that creates udp server---------------------------*/
void server_udp(int port, int nbmsg){
  
  int sock, n;
  int i = 0; /* increment */
  struct sockaddr_in servaddr, cliaddr;
  char pmsg[MAX_MSG_LEN];
  int infmessages = 0;
  socklen_t lg_adr_dist = sizeof(cliaddr);
  
  if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
    perror("socket");
    exit(1);
  }

  /* -----------------------------create host address --------------------------*/
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_port = htons(port);
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  /* using bind to assign info to our socket created */
  if (bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("bind");
    exit(1);
  }

  /*-------------------------------reception----------------------------------- */

  if (nbmsg == -1) {infmessages = 1;}

  while (infmessages || i < nbmsg) {
    
    if ((n = recvfrom(sock, pmsg, sizeof(pmsg), 0, (struct sockaddr *)&cliaddr, &lg_adr_dist)) < 0) {
      perror("recvfrom");
      exit(1);
    }

    printf("received: [%d %.*s]\n", (i+1), n, pmsg);
    i++;
  }
  
  close(sock);

}

/*---------------------------------------------------------------------------------*/
/* -----------------------TCP client and server----------------------------------- */
/*---------------------------------------------------------------------------------*/

void client_tcp(int port, char* hostname, int nbmsg, int lgmsg){

  int sock;
  struct sockaddr_in servaddr;
  socklen_t lg_adr_dist = sizeof(servaddr);
  char M[lgmsg];
  char charmsg = 'a'; /* character that will be sent */
  struct hostent* server;
  int i = 0; /* increment */
  
  /* creating the socket of designated specs */
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }
  
  /* create host address */
  if ((server = gethostbyname(hostname)) == NULL) {
    fprintf(stderr, "host not found\n");
    exit(1);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);
  memcpy(&servaddr.sin_addr, server->h_addr, server->h_length);

  /* sending a connection request */
  if (connect(sock, (struct sockaddr *)&servaddr, lg_adr_dist) < 0){
    perror("connect");
    exit(1);
  }

  /* sending nbmsg of messages*/
  for (i=0; i<nbmsg; i++) {
    memset(&M, charmsg, lgmsg);
    if (send(sock, M, lgmsg, 0) < 0) {
      perror("send");
    }
    charmsg++;
    if (charmsg > 'z') charmsg = 'a'; /* Make sure we print characters and not whatever*/
  }
  close(sock);
}

void server_tcp(int port, int nbmsg){

  /* definition of vaiables */
  int sock, n;
  int i = 0; /* increment */
  struct sockaddr_in servaddr, cliaddr;
  char pmsg[MAX_MSG_LEN];
  int infmessages = 0;
  socklen_t lg_adr_dist = sizeof(cliaddr);

  /* creating socket*/
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    perror("socket");
    exit(1);
  }

  /*---------------------creating local address----------------------------*/
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_port = htons(port);
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  /* binding address to socket */
  if (bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("bind");
    exit(1);
  }

  /* accept connection request */
  if (accept(sock, (struct sockaddr *)&cliaddr, lg_adr_dist) < 0){
    perror("accept");
    exit(1);
  }

  
  /* reception */
  if (nbmsg == -1) {infmessages = 1;}
  while (infmessages || i < nbmsg) {
    
    if ((n = recv(sock, pmsg, sizeof(pmsg), 0) < 0)) {
      perror("recv");
      exit(1);
    }

    printf("received: [%d %.*s]\n", (i+1), n, pmsg);
    i++;
  }
  
  close(sock);

}

