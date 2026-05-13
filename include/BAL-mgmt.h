#ifndef BAL_MGMT
#define BAL_MGMT

// LINKED LISTS FOR BAL LETTER MANAGEMENT

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#include "tools.h"
#include "config.h"


/* linked list data type*/
// Used to store incoming messages, and to look up messages before sending
typedef struct bal_head {
  int nb_boites;
  struct boite_lettre* pfirstbox;
} bal_head;

typedef struct boite_lettre {
  int box_ID;
  int nb_msg;
  struct une_msg* pfirstmsg;     /* POINTS TOWARDS THE MESSAGES   */
  struct boite_lettre* pnextbox; /* POINTS TOWARDS NEXT LETTERBOX */
} boite_lettre;

typedef struct une_msg {
  char msg[LG_MAX_MSG]; 
  int lg_msg;
  struct une_msg* pnextmsg;   /* NEXT MESSAGE */
} une_msg;


// ADD_LETTER FUNCTION
// Adds letter to the top of a letterbox
// Message becomes new head of linked list of messages
void add_letter(bal_head* BAL, int box_ID_dest, char* msg, int lg_msg);

// AFFICHE_LETTER FUNCTION
// Sends all letters of a box over the socket, then frees the messages
// Does not delete a box when it's empty
void affiche_letter(bal_head* BAL, int box_ID_dest, int sock);

#endif
