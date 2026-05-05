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
#include "BAL-corr.c"



#define MAX_MSG_LEN 1000
#define MAX_WAITLIST 5




struct message_init {
  int emetteur;
  int lg_msg;
  int nb_msg;
  int id_recept;
};

/* Prototypes */
void client_udp(int port, char* hostname, int nbmsg, int lgmsg);
void client_tcp(int port, char* hostname, int nbmsg, int lgmsg);
void server_udp(int port, int nbmsg);
void server_tcp(int port, int nbmsg);
void construire_message (char *message, char motif, int lg);
void afficher_message (char *message, int lg);
void bal_e(int port, char* hostname, int nbmsg, int lgmsg, struct message_init Mi, int id_recept);
void bal_r(int port, char* hostname, struct message_init Mi, int id_recept);
void create_bal(int port);

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

/*---------------------------------------------------------------------------------*/
/* -----------------------UDP client and server----------------------------------- */
/*---------------------------------------------------------------------------------*/
void client_udp(int port, char* hostname, int nbmsg, int lgmsg){

  int sock;
  struct sockaddr_in servaddr;
  socklen_t lg_adr_dist = sizeof(servaddr);
  char M[lgmsg];
  char charmsg = 'a';
  struct hostent* server;
  int i = 0;
  
  if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }
  
  if ((server = gethostbyname(hostname)) == NULL) {
    fprintf(stderr, "host not found\n");
    exit(1);
  }
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);
  memcpy(&servaddr.sin_addr, server->h_addr_list[0], server->h_length);

  for (i=0; i<nbmsg; i++) {
    memset(&M, charmsg, lgmsg);
    if (sendto(sock, M, lgmsg, 0, (struct sockaddr *)&servaddr, lg_adr_dist) < 0) {
      perror("sendto");
    }
    printf("sent: [%d %.*s]\n", (i+1), lgmsg, M);
    charmsg++;
    if (charmsg > 'z') charmsg = 'a';
  }
  close(sock);
}


void server_udp(int port, int nbmsg){
  
  int sock, n;
  int i = 0;
  struct sockaddr_in servaddr, cliaddr;
  char pmsg[MAX_MSG_LEN];
  int infmessages = 0;
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
  char charmsg = 'a';
  struct hostent* server;
  int i = 0;
  
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }
  
  if ((server = gethostbyname(hostname)) == NULL) {
    fprintf(stderr, "host not found\n");
    exit(1);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);
  memcpy(&servaddr.sin_addr, server->h_addr_list[0], server->h_length);

  if (connect(sock, (struct sockaddr *)&servaddr, lg_adr_dist) < 0){
    perror("connect");
    exit(1);
  }

  for (i=0; i<nbmsg; i++) {
    memset(&M, charmsg, lgmsg);
    if (send(sock, M, lgmsg, 0) < 0) {
      perror("send");
    }
    printf("sent: [%d %.*s]\n", (i+1), lgmsg, M);
    charmsg++;
    if (charmsg > 'z') charmsg = 'a';
  }
  close(sock);
}

void server_tcp(int port, int nbmsg){

  int sock, n;
  int connfd;
  int i = 0;
  struct sockaddr_in servaddr, cliaddr;
  char pmsg[MAX_MSG_LEN];
  int infmessages = 0;
  socklen_t lg_adr_dist = sizeof(cliaddr);

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
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

  listen(sock, MAX_WAITLIST);
         
  if ((connfd = accept(sock, (struct sockaddr *)&cliaddr, &lg_adr_dist)) < 0){
    perror("accept");
    exit(1);
  }

  if (nbmsg == -1) {infmessages = 1;}
  while (infmessages || i < nbmsg) {
    
    if (((n = recv(connfd, pmsg, sizeof(pmsg), 0)) < 0)) {
      perror("recv");
      exit(1);
    }

    if (n==0) {
      printf("client disconnected\n");
      break;
    }

    printf("received: [%d %.*s]\n", (i+1), n, pmsg);
    i++;
  }

  close(connfd);
  close(sock);
}

/*---------------------------------------------------------------------------------*/
/* -----------------------BAL mode: emitter----------------------------------------*/
/*---------------------------------------------------------------------------------*/

void bal_e(int port, char* hostname, int nbmsg, int lgmsg, struct message_init Mi, int id_recept){

  int sock;
  struct sockaddr_in servaddr;
  socklen_t lg_adr_dist = sizeof(servaddr);
  char M[lgmsg];
  char charmsg = 'a';
  struct hostent* server;
  int i = 0;
  int lg_mi = sizeof(Mi);
  
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }
  
  if ((server = gethostbyname(hostname)) == NULL) {
    fprintf(stderr, "host not found\n");
    exit(1);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);
  memcpy(&servaddr.sin_addr, server->h_addr_list[0], server->h_length);

  if (connect(sock, (struct sockaddr *)&servaddr, lg_adr_dist) < 0){
    perror("connect");
    exit(1);
  }

  /* Build and send identification message first */
  Mi.lg_msg = lgmsg;
  Mi.nb_msg = nbmsg;
  Mi.id_recept = id_recept;

  if (send(sock, &Mi, lg_mi, 0) < 0) {
    perror("send Mi");
    exit(1);
  }

  /* FIX: sleep takes seconds, not milliseconds — use sleep(1) not sleep(2000) */
  sleep(1);


  /*Construction du message à envoyer*/
  construire_message(M,charmsg,lgmsg);

  /* Send the messages */
  for (i=0; i<nbmsg; i++){
    memset(M, charmsg, lgmsg);
    if (send(sock, M, lgmsg, 0) < 0) {
      perror("send");
    }
    printf("SOURCE : Envoi lettre n°%d a destination du recepteur %d (%d) [%.*s]\n",
           (i+1), id_recept, lgmsg, lgmsg, M);
    charmsg++;
    if (charmsg > 'z') charmsg = 'a';
  }
  printf("SOURCE : fin\n");
  close(sock);
}

/*---------------------------------------------------------------------------------*/
/* -----------------------BAL mode: receptor---------------------------------------*/
/*---------------------------------------------------------------------------------*/

void bal_r(int port, char* hostname, struct message_init Mi, int id_recept){
  int sock, n;
  char pmsg[MAX_MSG_LEN];
  struct sockaddr_in servaddr;
  socklen_t lg_adr_dist = sizeof(servaddr);
  int lgmsg;
  struct hostent* server;
  int i = 0;
  int lg_mi = sizeof(Mi);

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }
  
  if ((server = gethostbyname(hostname)) == NULL) {
    fprintf(stderr, "host not found\n");
    exit(1);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(port);
  memcpy(&servaddr.sin_addr, server->h_addr_list[0], server->h_length);

  /* FIX: connect() does not return a new fd — just check < 0 */
  if (connect(sock, (struct sockaddr *)&servaddr, lg_adr_dist) < 0){
    perror("connect");
    exit(1);
  }

  /* Send identification message to BAL */
  Mi.id_recept = id_recept;
  Mi.lg_msg = 0;
  Mi.nb_msg = 0;

  if (send(sock, &Mi, lg_mi, 0) < 0) {
    perror("send Mi");
    exit(1);
  }

  /* FIX: sleep(1) not sleep(2000) */
  sleep(1);

  printf("RECEPTION : Demande de recuperation des lettres par le recepteur %d port=%d, TP=tcp, dest=%s\n",
         id_recept, port, hostname);

  /* Receive letters until BAL closes the connection (n==0) */
  while ((n = recv(sock, pmsg, sizeof(pmsg), 0)) > 0) {
    lgmsg = n;
    printf("RECEPTION : Recuperation lettre par le recepteur %d (%d) [%.*s]\n",
           id_recept, lgmsg, lgmsg, pmsg);
    i++;
  }
  
  if (n < 0) {
    perror("recv");
    exit(1);
  }

  printf("RECEPTION : fin\n");
  close(sock);
}

/*---------------------------------------------------------------------------------*/
/* -----------------------BAL mode: BAL creation-----------------------------------*/
/*---------------------------------------------------------------------------------*/

void create_bal(int port){
  int sock, n;
  int connfd;
  int i ;
  struct sockaddr_in servaddr, cliaddr;
  char pmsg[MAX_MSG_LEN];
  socklen_t lg_adr_dist = sizeof(cliaddr);
  struct message_init pMi;
  int recepteur;
  int lgmsg;
  int nbmsg;

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
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

  listen(sock, MAX_WAITLIST);

  /* Allocate and initialise the BAL head */
  bal_head* BAL = (bal_head*)malloc(sizeof(bal_head));
  BAL->nb_boites = 0;
  BAL->pfirstbox = NULL;

  printf("PUITS : port=%d, TP=tcp\n", port);

  /* Main loop: accept connections indefinitely */
  while (1) {

    if ((connfd = accept(sock, (struct sockaddr *)&cliaddr, &lg_adr_dist)) < 0){
      perror("accept");
      exit(1);
    }

    /* FIX: recv on connfd, not on sock (listening socket) */
    if ((n = recv(connfd, &pMi, sizeof(pMi), 0)) < 0) {
      perror("recv Mi");
      close(connfd);
      continue;
    }

    recepteur = pMi.id_recept;

    if (pMi.emetteur == 1) {
      /* === EMITTER side === */
      lgmsg = pMi.lg_msg;
      nbmsg = pMi.nb_msg;

     

      for (i=0;i < nbmsg;i++){
        n = recv(connfd, pmsg, lgmsg, 0);
        if (n <= 0) {
          if (n == 0) printf("PUITS : emetteur a ferme la connexion\n");
          else perror("recv lettre");
          break;
        }

        printf("PUITS : Reception et stockage lettre n°%d pour le recepteur n°%d [%.*s]\n",
               (i+1), recepteur, n, pmsg);

        /* FIX: pass n (actual received size) to add_letter */
        add_letter(BAL, recepteur, pmsg, n);
        
      }
      /* E closes the connection — we just close our side */
      close(connfd);

    } else {
      /* === RECEPTOR side === */
      printf("PUITS : Envoi des lettres au recepteur %d\n", recepteur);
      affiche_letter(BAL, recepteur, connfd);

      /* FIX: BAL closes the connection after sending all letters */
      close(connfd);
    }
  }

  free(BAL);
  close(sock);
}

/*---------------------------------------------------------------------------------*/
/* -----------------------Message building-----------------------------------------*/
/*---------------------------------------------------------------------------------*/

void construire_message (char *message, char motif, int lg) {
  int i;
  for (i=0; i<lg; i++) message[i] = motif;
}

