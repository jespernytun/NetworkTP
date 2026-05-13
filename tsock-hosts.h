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

void client_udp(int port, char* hostname, int nbmsg, int lgmsg)

void server_udp(int port, int nbmsg)

void client_tcp(int port, char* hostname, int nbmsg, int lgmsg)

void server_tcp(int port, int nbmsg)



#endif
