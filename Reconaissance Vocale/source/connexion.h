#ifndef CONNEXION_H
#define CONNEXION_H

#include "mbed.h"
#include "wifi_helper.h"

void init_my_connexion(bool verbose);
void send_led_state (int switch_state, bool verbose);


#endif