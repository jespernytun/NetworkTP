#ifndef NETWORK_CONFIG_TP
#define NETWORK_CONFIG_TP

#define MAX_MSG_LEN 1000
#define MAX_WAITLIST 5
#define LG_MAX_MSG 100

// Message to initialize conversation with mailbox
struct message_init {
  int emetteur; // Defines if you wish to send or recieve messages
  int lg_msg;   // Lenght of messages to send to mailbox
  int nb_msg;	// Nb of messages to send to mailbox
  int id_recept; // ID of the recieving mailbox
};

#endif
