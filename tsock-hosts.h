#ifndef TSOCK_HOSTS
#define TSOCK_HOSTS


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



#endif