

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server;
    char message[BUFFER_SIZE], server_reply[BUFFER_SIZE];

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Could not create socket");
        exit(EXIT_FAILURE);
    }
    printf("Socket created\n");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("172.20.0.3");
    server.sin_port = htons(PORT);

    // Connect to remote server
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Connect failed");
        close(sock);
        exit(EXIT_FAILURE);
    }
    printf("Connected\n");

    // Receive weather data from the server
    if (recv(sock, server_reply, BUFFER_SIZE, 0) < 0) {
        perror("recv failed");
        close(sock);
        exit(EXIT_FAILURE);
    }
    printf("Weather Data: %s\n", server_reply);

    // Send confirmation to server
    snprintf(message, BUFFER_SIZE, "Weather data received successfully.");
    send(sock, message, strlen(message), 0);

    close(sock);
    return 0;
}
