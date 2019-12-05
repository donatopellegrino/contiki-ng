#include "contiki.h"
#include <stdio.h>
#include <stdlib.h>
#include "net/ipv6/uip-ds6.h"
#include "net/ipv6/uip-udp-packet.h"
#include "aes-128.h"
#include "dev/adc-sensors.h"

#define UDP_CLIENT_PORT 8765
#define UDP_SERVER_PORT 5678
#define MAX_PAYLOAD_LEN	30
#define DEVICE_ID 1
#define ADC_PIN 5


static struct uip_udp_conn *client_conn;
static uip_ipaddr_t server_ipaddr;
static int seq_id = 0;

static void send_packet();
static void set_global_address();

static char* secret;
static uint8_t* key;

static uint8_t* cipher2ascii(char* str){
  static uint8_t tmp[16];	
  for(int i=0; i<16; i++){
    tmp[i]=(uint8_t)str[i];
  }
  return tmp;
}

PROCESS(node, "node process");
AUTOSTART_PROCESSES(&node);

PROCESS_THREAD(node, ev, data)
{
  static struct etimer timer;
  PROCESS_BEGIN();

  NETSTACK_MAC.on();

  etimer_set(&timer, CLOCK_SECOND * 10);

  set_global_address();
  client_conn = udp_new(NULL, UIP_HTONS(UDP_SERVER_PORT), NULL);
  if(client_conn == NULL) {
    printf("No UDP connection available, exiting the process!\n");
    PROCESS_EXIT();
  }
  //set encryption key
  secret="thisisasecretkey";
  //static uint8_t* key;
  //cipher2ascii(secret, key);
  /*for(int i=0;i<16;i++){
    printf("%x", key[i]);
  }
  printf("\n");*/
  //AES_128.set_key(key);


  while(1) {
    printf("VIVO\n");

    send_packet();

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    etimer_reset(&timer);
  }
  PROCESS_END();
}

static void send_packet(){
  key=cipher2ascii(secret);
  AES_128.set_key(key); 
  char buf[MAX_PAYLOAD_LEN];


  //Calcolo temperaura
  adc_sensors.configure(ANALOG_GROVE_LOUDNESS, ADC_PIN);
  uint16_t temperature = adc_sensors.value(ANALOG_GROVE_LOUDNESS);
  //int temperature=10123;

  int lenght=2;
  if(temperature==0) temperature=1;
  int tmp=temperature;
  char plain[17];
  plain[0]='B';
  plain[1]=',';
  sprintf(plain+2, "%d", temperature);
  while(tmp != 0)
  {
    /* Increment digit count */
    lenght++;

    /* Remove last digit of 'num' */
    tmp /= 10;
  }
  for(int i=lenght; i<16; i++){
    plain[i]='f';
  }
  plain[16]='\0';

  uint8_t *cipher;
  cipher=cipher2ascii(plain);
  
  AES_128.encrypt(cipher);
  
  static char cipherSend[17];
  for(int i=0; i<16; i++)
    cipherSend[i]=(char)cipher[i];
  //memcpy(cipherSend, (char *)cipher, 16);
  cipherSend[16]='\0';

  printf("%s, %s, %p, %u%u\n", plain, secret, (void*)cipher, key[0], key[15]);
  for(int i=0;i<16;i++) printf("%02X", cipher[i]);
  printf("\n");

  seq_id++;
  sprintf(buf, "%s\n", cipherSend);
  uip_udp_packet_sendto(client_conn, buf, strlen(buf),
                        &server_ipaddr, UIP_HTONS(UDP_SERVER_PORT));
}

static void set_global_address(){
  uip_ip6addr(&server_ipaddr, 0xfd00, 0, 0, 0x5000, 0, 0, 0, 1);
}
