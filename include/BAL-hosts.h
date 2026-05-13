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

void bal_e(int port, char* hostname, int nbmsg, int lgmsg, struct message_init Mi, int id_recept)

/*---------------------------------------------------------------------------------*/
/* -----------------------BAL mode: receptor---------------------------------------*/
/*---------------------------------------------------------------------------------*/

void bal_r(int port, char* hostname, struct message_init Mi, int id_recept)

/*---------------------------------------------------------------------------------*/
/* -----------------------BAL mode: BAL creation-----------------------------------*/
/*---------------------------------------------------------------------------------*/

void create_bal(int port)

#endif
