#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_endian.h>
#include <sys/_types/_socklen_t.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 2048
#define PORT 3000

int main(void) {
  int server_fd;
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("Could not start the server socket: %s", strerror(errno));
    exit(EXIT_FAILURE);
  }
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse))) {
    printf("Could not re-use the port: %s", strerror(errno));
    exit(EXIT_FAILURE);
  }
  struct sockaddr_in server_addr = {
      .sin_family = AF_INET,
      .sin_port = htons(PORT),
      .sin_addr = {htonl(INADDR_ANY)},
  };

  if (bind(server_fd, (const struct sockaddr *)&server_addr,
           sizeof(server_addr)) < 0) {
    printf("Bind failed: %s", strerror(errno));
    exit(EXIT_FAILURE);
  }

  int8_t backlogCount = 5;
  if (listen(server_fd, backlogCount) < 0) {
    printf("Listen failed: %s", strerror(errno));
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  uint8_t buffer[BUFFER_SIZE];
  memset(buffer, 0, BUFFER_SIZE);

  printf("Listening on port :%d...\n", PORT);
  int client_fd =
      accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
  if (client_fd < 0) {
    printf("Accept failed: %s", strerror(errno));
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < BUFFER_SIZE; i++) {
    if (buffer[i] != 255) {
      printf("%c", buffer[i]);
    }
  }

  printf("client connected \n");

  char *res = "HTTP/1.1 200 OK\r\n"
              "Content-Length: 13\r\n"
              "Connection: close\r\n"
              "\r\n"
              "Hello, world!";
  send(client_fd, res, strlen(res), 0);

  close(client_fd);
  close(server_fd);
  return EXIT_SUCCESS;
}
