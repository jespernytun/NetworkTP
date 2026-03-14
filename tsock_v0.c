
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

/* Prototypes */
void client_udp(int port, char* hostname); /* creates a udp client */
void server_udp(int port);


int main (int argc, char **argv) {
	int c;
	extern char *optarg;
	extern int optind;
	int nb_message = -1; /* number of messages to send, 10 by default */
	int source = -1 ; /* 0=reciever, 1=source */
   
   int udp = 0; /* We define a variable udp to handle the flag -u */
   int port;
   char* hostname;
   
   
	while ((c = getopt(argc, argv, "psun:")) != -1) {
		switch (c) {
		case 'p':
			if (source == 1) {
				printf("usage: cmd [-p|-s][-n ##][-u]\n");
				exit(1);
			}
			source = 0;
			break;

		case 's':
			if (source == 0) {
				printf("usage: cmd [-p|-s][-n ##][-u]\n");
				exit(1) ;
			}
			source = 1;
			break;

		case 'n':
			nb_message = atoi(optarg);
			break;

      case 'u':
        udp = 1;
        break;
        

		default:
			printf("usage: cmd [-p|-s][-n ##][-u]\n");
			break;
		}
	}

	if (source == -1) {
		printf("usage: cmd [-p|-s][-n ##][-u]\n");     
		exit(1) ;
	}

   
   if (nb_message != -1) {
     if (source == 1)
       printf("nb of messages to send : %d\n", nb_message);
     else
			printf("nb of messages to recieve : %d\n", nb_message);
   } else {
     if (source == 1) {
       nb_message = 10 ;
       printf("nb of messages to send = 10 by default\n");
     } else
       printf("nb of messages to recieve = inf\n");
   }
   
   if (udp == 1) {
     if (source == 1) {
       printf("We're on the source\n");
       
       port = atoi(argv[argc-1]); /* We assign last argument as nb port*/
       hostname = argv[argc-2]; /* We assign second to last argument as hostname*/
       
       client_udp(port, hostname);

     } else { 
       printf("We're on the reciever\n");

       port = atoi(argv[argc-1]); /* We assign last argument as nb port */

       server_udp(port); 
     }
            
   } else {
     printf("Sorry, TCP function not yet implemented\n");
   }

   return 0;
}


void client_udp(int port, char* hostname){

  int sock;
  struct sockaddr_in servaddr;
  socklen_t lg_adr_dist = sizeof(servaddr);
  char M[30];
  memset(M, 'a', 30);
  struct hostent* server; 
  
  /* creating the socket of designated specs */
  if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
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

  /* sending */
if (sendto(sock, M, 30, 0, (struct sockaddr *)&servaddr, lg_adr_dist) < 0) {
    perror("sendto");
}
    
  close(sock);
}


void server_udp(int port){
  
  int sock, n;
  struct sockaddr_in servaddr, cliaddr;
  char pmsg[30];
  socklen_t lg_adr_dist = sizeof(cliaddr);
  
  if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
    perror("socket");
    exit(1);
  }
  
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_port = htons(port);
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  
  if (bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("bind");
    exit(1);
  }

  /* receptiom */
  if ((n = recvfrom(sock, pmsg, sizeof(pmsg), 0, (struct sockaddr *)&cliaddr, &lg_adr_dist)) < 0) {
    perror("recvfrom");
    exit(1);
  }
  
  printf("received: [%.*s]\n", n, pmsg);
  
  close(sock);

}
