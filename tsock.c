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
/* structures retournées par les fonctions de gestion de la base de données du réseau */
#include <netdb.h>
/* pour les entrées/sorties */
#include <stdio.h>
/* pour la gestion des erreurs */
#include <errno.h>
#include <string.h>

#include "tools.h"
#include "config.h"

#include "BAL-mgmt.h"
#include "BAL-hosts.h"
#include "tsock-hosts.h"






int main (int argc, char **argv) {
	int c;
	extern char *optarg;
	extern int optind;
	int nb_message = -1;
    int lg_message = -1;
    int source = -1;
    int udp = 0;
    int port;
    char* hostname;
    struct message_init Mi;
    int bal = 0;
    int mode_bal = 0;
    int recepteur;

/*---------------------------------------------------------------------------------*/
/* ----------------------- Option/Flag Management -------------------------------- */
/*---------------------------------------------------------------------------------*/
	while ((c = getopt(argc, argv, "psun:l:e:r:b")) != -1) {
		switch (c) {
		case 'p':
			if (source == 1) {
				printf("usage: cmd [-p|-s][-n ##][-l ##][-u]\n");
				exit(1);
			}
			source = 0;
			break;

		case 's':
			if (source == 0) {
				printf("usage: cmd [-p|-s][-n ##][-l ##][-u]\n");
				exit(1) ;
			}
			source = 1;
			break;

		case 'n':
			nb_message = atoi(optarg);
			break;

		case 'l':
			lg_message = atoi(optarg);
			break;         

    case 'u':
      udp = 1;
      break;

    case 'e':
      Mi.emetteur = 1;
      mode_bal = 1;
      recepteur = atoi(optarg);
      break;

    case 'r':
      Mi.emetteur = 0;
      mode_bal = 1;
      recepteur = atoi(optarg);
      break;

    case 'b':
      bal = 1;
      mode_bal = 1;
      break;
      
		default:
			printf("usage: cmd [-p|-s][-n ##][-l ##][-u]\n");
			break;
		}
	}

  if (mode_bal == 0) {

	if (source == -1) {
		printf("usage: cmd [-p|-s][-n ##][-l ##][-u]\n");    
		exit(1) ;
	}

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

   if (udp == 1) {
     printf("We're using UDP\n");
     if (source == 1) {
       printf("We're on the source\n");

       if (lg_message == -1) {lg_message = 30;}
       if (nb_message == -1) {nb_message = 10;}
       
       port = atoi(argv[argc-1]);
       hostname = argv[argc-2];

       client_udp(port, hostname, nb_message, lg_message);

     } else { 
       printf("We're on the reciever\n");

       port = atoi(argv[argc-1]);

       server_udp(port, nb_message); 
     }
            
   } else {
     printf("We're using TCP\n");
     if (source == 1) {
       printf("We're on the source\n");

       if (lg_message == -1) {lg_message = 30;}
       if (nb_message == -1) {nb_message = 10;}
       
       port = atoi(argv[argc-1]);
       hostname = argv[argc-2];

       client_tcp(port, hostname, nb_message, lg_message);

     } else { 
       printf("We're on the reciever\n");
       
       port = atoi(argv[argc-1]);

       server_tcp(port, nb_message); 
      }
  }

  } else {
    /* BAL mode */
    if (bal == 0) {
        if (Mi.emetteur == 1) {
          printf("Emitter\n");
          if (lg_message == -1) {lg_message = 30;}
          if (nb_message == -1) {nb_message = 10;}

          port = atoi(argv[argc-1]);
          hostname = argv[argc-2];
          
          bal_e(port, hostname, nb_message, lg_message, Mi, recepteur);
      
        } else {
          printf("Receptor\n");
          port = atoi(argv[argc-1]);
          hostname = argv[argc-2];
          bal_r(port, hostname, Mi, recepteur); 
        }
    } else {
      printf("BAL created\n");
      port = atoi(argv[argc-1]);
      create_bal(port);
    }
  }

   return 0;
}

