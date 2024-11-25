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

#define PORT 3000
#define BUFFER_SIZE 2048
#define BACKLOG 5

void check(int e, const char *context) {
  if (e < 0) {
    printf("%s failed: %s", context, strerror(errno));
    exit(EXIT_FAILURE);
  }
}

int main(void) {
  int server_fd, client_fd;
  struct sockaddr_in client_addr;
  struct sockaddr_in server_addr = {
      .sin_family = AF_INET,
      .sin_port = htons(PORT),
      .sin_addr = {htonl(INADDR_ANY)},
  };
  socklen_t client_addr_len = sizeof(client_addr);

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  check(server_fd, "socket creation");

  int reuse = 1;
  check(setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)),
        "setting up socket option");

  check(bind(server_fd, (const struct sockaddr *)&server_addr,
             sizeof(server_addr)),
        "server bind");

  check(listen(server_fd, BACKLOG), "listen server");
  client_fd =
      accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
  check(client_fd, "accept");

  uint8_t buffer[BUFFER_SIZE];
  memset(buffer, 0, BUFFER_SIZE);
  check(recv(client_fd, buffer, BUFFER_SIZE, 0), "recive stuff");

  for (int i = 0; i < BUFFER_SIZE; i++) {
    printf("%c", buffer[i]);
  }

  char *res = "HTTP/1.1 200 OK\r\n"
              "Content-Length: 13\r\n"
              "Connection: keep-alive\r\n"
              "\r\n"
              "Hello, world!";

  send(client_fd, res, strlen(res), 0);

  close(client_fd);
  close(server_fd);
  exit(EXIT_SUCCESS);
}
