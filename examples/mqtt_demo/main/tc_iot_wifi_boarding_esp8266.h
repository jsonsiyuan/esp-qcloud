#ifndef BOARDING_ESP8266_H
#define BOARDING_ESP8266_H

#define inline __inline



void softAP_task(void *pvParameters);
int start_softAP(const char *ssid, const char *psw);
void stop_softAP(void);

void smartconfig_task(void * parm);
int start_smartconfig(void);
void stop_smartconfig(void);

#endif

