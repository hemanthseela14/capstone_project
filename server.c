#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

typedef struct {
    int temperature;
    int humidity;
    int wind_speed;
} WeatherData;

void generate_weather_data(WeatherData* data) {
    // Simulate weather data generation
    srand(time(NULL));
    data->temperature = rand() % 40;   
    data->humidity = rand() % 100;     
    data->wind_speed = rand() % 150;   
}

void* handle_client(void* arg) {
    int client_sock = *(int*)arg;
    char buffer[BUFFER_SIZE];
    int read_size;
    WeatherData data;

    // Generate weather data
    generate_weather_data(&data);
    snprintf(buffer, BUFFER_SIZE, "Temperature: %d C, Humidity: %d %%, Wind Speed: %d km/h",
             data.temperature, data.humidity, data.wind_speed);

    // Send weather data to the client
    send(client_sock, buffer, strlen(buffer), 0);

    // Receive confirmation from the client
    if ((read_size = recv(client_sock, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[read_size] = '\0';
        printf("Client: %s\n", buffer);
    }

    if (read_size == 0) {
        printf("Client disconnected\n");
    } else if (read_size == -1) {
        perror("recv failed");
    }

    close(client_sock);
    free(arg);
    return NULL;
}

int main() {
    int server_sock, client_sock, *new_sock;
    struct sockaddr_in server, client;
    socklen_t client_len = sizeof(client);
    pthread_t thread_id;

    // Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Could not create socket");
        exit(EXIT_FAILURE);
    }
    printf("Socket created\n");

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Bind
    if (bind(server_sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Bind failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }
    printf("Bind done\n");

    // Listen
    listen(server_sock, MAX_CLIENTS);
    printf("Waiting for incoming connections...\n");

    // Accept incoming connections
    while ((client_sock = accept(server_sock, (struct sockaddr*)&client, &client_len))) {
        printf("Connection accepted\n");

        new_sock = malloc(1);
        *new_sock = client_sock;

        if (pthread_create(&thread_id, NULL, handle_client, (void*)new_sock) < 0) {
            perror("Could not create thread");
            free(new_sock);
            close(client_sock);
        } else {
            printf("Handler assigned\n");
        }
    }

    if (client_sock < 0) {
        perror("Accept failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    close(server_sock);
    return 0;
}
