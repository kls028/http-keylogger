#include <winsock2.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa); //run Winsock 2.2

    SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET; //ipv4
    addr.sin_port = htons(8080); //port 8080 converted from LE to BE
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    bind(server, (struct sockaddr *)&addr, sizeof(addr)); //bind server socket to 127.0.0.1:8080
    listen(server, 1);

    printf("waiting for connection\n");

    SOCKET client = accept(server, NULL, NULL); // client socket creation when connection received

    char buffer[4096] = {0};
    int received = recv(client, buffer, sizeof(buffer) - 1, 0);

    if (received > 0) {
        char *body = strstr(buffer, "\r\n\r\n"); //get first occurrence of \r\n\r\n indicating end of HTTP headers

        printf("received: %s\n",
               body ? body + 4 : buffer);
    }

    const char response[] =
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: 2\r\n"
        "Connection: close\r\n\r\n"
        "OK";

    send(client, response, sizeof(response) - 1, 0);

    closesocket(client);
    closesocket(server);
    WSACleanup();

    return 0;
}