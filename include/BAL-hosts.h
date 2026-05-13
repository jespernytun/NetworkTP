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

#include "config.h"   
#include "BAL-mgmt.h" 
#include "tools.h" 

/*---------------------------------------------------------------------------------*/
/* -----------------------BAL mode: emitter----------------------------------------*/
/*---------------------------------------------------------------------------------*/
// Takes a port number, a hostname, the number of messages, their length, an initialization message, and a BOX ID as inputs
// Opens communication with BAL, sends an initialzation message that defines sizes for the correspondance and informs the BAL
// which BAL it seeks to engage with.

void bal_e(int port, char* hostname, int nbmsg, int lgmsg, struct message_init Mi, int id_recept);


/*---------------------------------------------------------------------------------*/
/* -----------------------BAL mode: receptor---------------------------------------*/
/*---------------------------------------------------------------------------------*/
// Takes a port number, a hostname, an initialization message, and a BOX ID as inputs
// Opens communication with BAL, sends initialization meesage that informs BAL that it wants to recieve message, and from which
// Letterbox it wants to communicate with.

void bal_r(int port, char* hostname, struct message_init Mi, int id_recept);


/*---------------------------------------------------------------------------------*/
/* -----------------------BAL mode: BAL creation-----------------------------------*/
/*---------------------------------------------------------------------------------*/
// Creates a BAL at a portnumber
// Will engage with bal_e and bal_r
// Initializes datastructre once called upon
// If it doesn't find a box on reception, it will create it

void create_bal(int port);

#endif
