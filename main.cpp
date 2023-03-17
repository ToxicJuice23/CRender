#include "/home/jujur/coding/c/socket.h"
#include <QApplication>
#include <QInputDialog>
#include <QTextEdit>
#include <QLabel>
#include <QMainWindow>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    char input_t[100];
    printf("Enter a url: ");
    scanf("%s", input_t);
    printf("Url entered: %s\n", input_t);
    if (strstr(input_t, "..")) {
        printf("Illegal character found in url");
        exit(1);
    }
    // get protocol
    if (argc < 2) {
        printf("Usage: ./client2 {protocol, ex:http}\n");
        exit(1);
    }
    char* def_port;
    if (argv[1] == "https")
        def_port = "443";
    else
        def_port = "80";
    // parse url
    char* hostname = input_t;
    char* end = hostname + strlen(hostname);
    *end = 0;
    char* port = strstr(hostname, ":");
    char* path = strstr(hostname, "/");
    if (port) {
        *port = (char)0;
        port += 1;
    } else {
        printf("Port not found in url, setting port to %s\n", def_port);
        port = def_port;
    }
    if (path) {
        *path = (char)0;
        path = path + 1;
    } else {
        path = "";
    }

    printf("Variables port: %s path: /%s hostname: %s protocol: %s\n", port, path, hostname, argv[1]);
    // send request
    // create socket connection
    char request[1024];
    sprintf(request, "GET /%s HTTP/1.1\r\nHost: www.%s\r\nConnection: close\r\n\r\n", path, hostname);

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *serveraddr;
    int res = getaddrinfo(hostname, port, &hints, &serveraddr);
    if (res) {
        printf("Getaddrinfo() failed errno %d\n", errno);
        exit(1);
    }

    int server_socket = socket(serveraddr->ai_family, serveraddr->ai_socktype, serveraddr->ai_protocol);
    printf("Socket has been created\n");
    connect(server_socket, serveraddr->ai_addr, serveraddr->ai_addrlen);
    printf("Connected to server\n");

    // send request
    int bs = send(server_socket, request, strlen(request), 0);
    printf("Sent %d bytes\n", bs);

    char read[1000000];
    int total_br = 0;
    int br = recv(server_socket, read+total_br, 8000, 0);
    total_br += br;
    while (br >= 1) {
        if (total_br < read - 8001) {
            br = recv(server_socket, read+total_br, 8000, 0);
            total_br += br;
        } else {
            printf("Ran out of memory buckaroo!\n");
        }
    }
    close(server_socket);

    // find body and display it
    sprintf(read, "%.*s", total_br, read);
    char* body = strstr(read, "\r\n\r\n");
    if (!body) {
        printf("No body was found in response. terminating program.\n");
        exit(0);
    }
    body += 4; // set body to start at html req
    printf("\n\nBody found: \n\n\t%s\n", body);
    //QTextEdit* display = new QTextEdit(body);
    QLabel* display = new QLabel(body);
    QMainWindow w;
    w.setStyleSheet("background-color: white; color: black;");
    w.setCentralWidget(display);
    w.show();
    return a.exec();
}
