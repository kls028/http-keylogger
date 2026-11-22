#include <winsock2.h>
#include <stdio.h>
#include <string.h>

/*
 * demo of network processing module client app - opens client connection to a server, formulates HTTP POST request and sends it to server
 */

int main(void) {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa); //run Winsock 2.2

    SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET; //ipv4
    addr.sin_port = htons(8080); //port 8080 converted from LE to BE
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    connect(client, (struct sockaddr *)&addr, sizeof(addr));

    const char req[] =
        "POST /test HTTP/1.1\r\n"
        "Host: 127.0.0.1\r\n"
        "Content-Length: 12\r\n\r\n"
        "test message";

    send(client, req, sizeof(req) - 1, 0);

    char resp[256] = {0};
    int received = recv(client, resp, sizeof(resp) - 1, 0);

    if (received > 0) {
        char *body = strstr(resp, "\r\n\r\n"); //get first occurrence of \r\n\r\n indicating end of HTTP headers
        printf("response:\n%s\n", body ? body + 4 : resp);
    }


    closesocket(client);
    WSACleanup();
    return 0;
}