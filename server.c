#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_CLIENTES 30

// Global variable for port (can be passed as program argument)
int PUERTO = 8181;

// Signal handler for SIGINT (Ctrl+C)
void sigint_handler(int sig) {
    printf("\nReceived SIGINT. Exiting...\n");
    exit(sig);
}

// Function declarations
void handle_client(int client_socket);
void get_info(char *response);
void get_num_partitions(char *response);
void get_kernel_version(char *response);
void sshd_running(char *response);

int main(int argc, char *argv[]) {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Register SIGINT handler
    signal(SIGINT, sigint_handler);

    // Check for port argument
    if (argc > 1) {
        PUERTO = atoi(argv[1]);
    }

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PUERTO);

    // Bind socket to port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Failed to bind socket");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, MAX_CLIENTES) < 0) {
        perror("Failed to listen");
        exit(EXIT_FAILURE);
    }

    printf("Servidor escuchando en el puerto %d...\n", PUERTO);

    // Accept incoming connections and handle them in separate threads or forks
    while (1) {
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Failed to accept connection");
            exit(EXIT_FAILURE);
        }

        printf("Nueva conexión aceptada\n");

        // Fork a new process to handle each client
        int pid = fork();
        if (pid == -1) {
            perror("Failed to create child process");
            close(client_socket);
            continue;
        } else if (pid == 0) {
            // Child process
            close(server_fd);
            handle_client(client_socket);
            exit(EXIT_SUCCESS);
        } else {
            // Parent process
            close(client_socket);
        }
    }

    return 0;
}

void handle_client(int client_socket) {
    while (1) {
        char buffer[1024] = {0};
        read(client_socket, buffer, sizeof(buffer));
        printf("Recibido: %s\n", buffer);

        // Parse client request
        if (strstr(buffer, "getInfo") != NULL) {
            char response[256];
            get_info(response);
            write(client_socket, response, strlen(response));
        } else if (strstr(buffer, "getNumberOfPartitions") != NULL) {
            char response[256];
            get_num_partitions(response);
            write(client_socket, response, strlen(response));
        } else if (strstr(buffer, "getCurrentKernelVersion") != NULL) {
            char response[256];
            get_kernel_version(response);
            write(client_socket, response, strlen(response));
        } else if (strstr(buffer, "sshdRunning") != NULL) {
            char response[256];
            sshd_running(response);
            write(client_socket, response, strlen(response));
        } else {
            char *not_found_response = "HTTP/1.0 404 Not Found\nContent-Type: text/html\n\n<html><body><h1>404 Not Found</h1></body></html>";
            write(client_socket, not_found_response, strlen(not_found_response));
            printf("Respuesta de solicitud no encontrada enviada\n");
        }

        // Break the loop if client closes the connection
        if (strncmp(buffer, "quit", 4) == 0)
            break;
    }

    close(client_socket);
    printf("Socket de cliente cerrado\n");
}


void get_info(char *response) {
    snprintf(response, 256, "HTTP/1.0 200 OK\nContent-Type: text/plain\n\nServicio de Salet Gutierrez, v0.1");
    printf("Respuesta de getInfo generada: %s\n", response);
}

void get_num_partitions(char *response) {
    FILE *lsblk = popen("lsblk -l | grep part | wc -l", "r");
    if (lsblk == NULL) {
        snprintf(response, 256, "HTTP/1.0 500 Internal Server Error\nContent-Type: text/html\n\n<html><body><h1>500 Internal Server Error</h1></body></html>");
        printf("Error al ejecutar lsblk\n");
        return;
    }

    char buffer[16];
    fgets(buffer, sizeof(buffer), lsblk);
    pclose(lsblk);

    // Construct response
    snprintf(response, 256, "HTTP/1.0 200 OK\nContent-Type: text/plain\n\nNumber of partitions: %s", buffer);
    printf("Respuesta de getNumberOfPartitions generada: %s\n", response);
}

void get_kernel_version(char *response) {
    FILE *uname_r = popen("uname -r", "r");
    if (uname_r == NULL) {
        snprintf(response, 256, "HTTP/1.0 500 Internal Server Error\nContent-Type: text/html\n\n<html><body><h1>500 Internal Server Error</h1></body></html>");
        printf("Error al ejecutar uname -r\n");
        return;
    }

    char buffer[64];
    fgets(buffer, sizeof(buffer), uname_r);
    pclose(uname_r);

    // Trim newline characters
    buffer[strcspn(buffer, "\n")] = 0;

    // Construct response with full kernel version
    snprintf(response, 256, "HTTP/1.0 200 OK\nContent-Type: text/plain\n\nKernel version: %s", buffer);
    printf("Respuesta de getCurrentKernelVersion generada: %s\n", response);
}


void sshd_running(char *response) {
    FILE *sshd_check = popen("ps aux | grep sshd | grep -v grep", "r");
    if (sshd_check == NULL) {
        snprintf(response, 256, "HTTP/1.0 500 Internal Server Error\nContent-Type: text/html\n\n<html><body><h1>500 Internal Server Error</h1></body></html>");
        printf("Error al ejecutar ps aux | grep sshd\n");
        return;
    }

    char buffer[1024];
    if (fgets(buffer, sizeof(buffer), sshd_check) != NULL) {
        // SSHD process is running
        snprintf(response, 256, "HTTP/1.0 200 OK\nContent-Type: text/plain\n\nsshd is running: true");
    } else {
        // No SSHD process found
        snprintf(response, 256, "HTTP/1.0 200 OK\nContent-Type: text/plain\n\nsshd is running: false");
    }

    pclose(sshd_check);
    printf("Respuesta de sshdRunning generada: %s\n", response);
}
