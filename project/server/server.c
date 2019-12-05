#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(){
  struct sockaddr_in6 i6sock;
  i6sock.sin6_family = AF_INET6;
  i6sock.sin6_addr   = in6addr_any;
  i6sock.sin6_port   = htons(5678);

  char buffer[100];

  int sock = socket(AF_INET6, SOCK_DGRAM, 0);

  bind(sock, (struct sockaddr *)&i6sock, sizeof(i6sock));

  while(1){
    fflush(stdout);
    int len = recvfrom(sock, buffer, sizeof(char)*100, 0, NULL, NULL);

    buffer[len] = '\0';

    printf("%s", buffer);
  }
}
