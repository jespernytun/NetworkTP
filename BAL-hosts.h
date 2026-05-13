#ifndef BAL_HOSTS
#define BAL_HOSTS

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
  
  sleep(1);

  /*Building the message to send*/
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

        add_letter(BAL, recepteur, pmsg, n);
        
      }
      /* E closes the connection — we just close our side */
      close(connfd);

    } else {
      /* === RECEPTOR side === */
      printf("PUITS : Envoi des lettres au recepteur %d\n", recepteur);
      affiche_letter(BAL, recepteur, connfd);

      
      close(connfd);
    }
  }

  free(BAL);
  close(sock);
}
#endif