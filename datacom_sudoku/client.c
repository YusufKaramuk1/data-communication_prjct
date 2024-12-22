#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 8080


void initializeWinsock() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed\n");
        exit(1);
    }
}


SOCKET createSocket() {
    SOCKET s;
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        exit(1);
    }
    return s;
}


void connectToServer(SOCKET s, struct sockaddr_in server) {
    if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Connection failed\n");
        exit(1);
    }
    printf("Connected to the server\n");
}


void displayBoard(char *message) {
    int num;
    for (int i = 0; i < 81; i++) {
        sscanf(message + i * 2, "%d", &num);
        if (num == 0) {
            printf("\033[1;34m%d \033[0m", num);
        } else {
            printf("\033[1;32m%d \033[0m", num);
        }

        if ((i + 1) % 9 == 0) {
            printf("\n");
            if (i == 26 || i == 53) {
                printf("----------------------------\n");
            }
        }
    }
}

int main() {
    SOCKET s;
    struct sockaddr_in server;
    char message[500];
    int row, col, num;


    initializeWinsock();


    s = createSocket();


    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(PORT);


    connectToServer(s, server);


    while (1) {
        int recvSize;


        recvSize = recv(s, message, sizeof(message), 0);
        message[recvSize] = '\0';
        displayBoard(message);


        while (1) {
            printf("Enter row (0-8), column (0-8), and number (1-9): ");
            if (scanf("%d %d %d", &row, &col, &num) != 3) {
                printf("Invalid input. Please enter valid integers for row, column, and number.\n");
                while(getchar() != '\n');
                continue;
            }
            if (row < 0 || row >= 9 || col < 0 || col >= 9 || num < 1 || num > 9) {
                printf("Invalid values. Ensure row and column are between 0-8, and number is between 1-9.\n");
                continue;
            }
            break;
        }


        sprintf(message, "%d %d %d", row, col, num);
        send(s, message, strlen(message), 0);


        recvSize = recv(s, message, sizeof(message), 0);
        message[recvSize] = '\0';
        printf("%s\n", message);
    }


    closesocket(s);
    WSACleanup();

    return 0;
}
