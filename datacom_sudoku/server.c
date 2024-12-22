#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 8080
#define SIZE 9


int board[SIZE][SIZE] = {
    {0, 0, 5, 2, 6, 8, 0, 0, 0},
    {6, 0, 9, 0, 0, 0, 0, 0, 0},
    {0, 8, 7, 5, 9, 4, 0, 0, 0},
    {0, 0, 0, 0, 2, 7, 0, 0, 9},
    {5, 0, 0, 4, 8, 9, 6, 0, 3},
    {9, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 5, 3, 8, 1, 0, 9, 0, 7},
    {4, 6, 0, 0, 7, 2, 0, 3, 5},
    {7, 9, 1, 0, 0, 0, 0, 0, 6}
};


int isValidMove(int row, int col, int num) {
    for (int i = 0; i < SIZE; i++) {
        if (board[row][i] == num || board[i][col] == num) {
            return 0;
        }
    }

    int startRow = row - row % 3, startCol = col - col % 3;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i + startRow][j + startCol] == num) {
                return 0;
            }
        }
    }

    return 1;
}


void sendBoard(SOCKET clientSocket) {
    char message[500];
    int msgLength = 0;


    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            msgLength += sprintf(message + msgLength, "%d ", board[i][j]);
        }
        if (i == 2 || i == 5) {
            msgLength += sprintf(message + msgLength, "\n----------------------\n");
        } else {
            msgLength += sprintf(message + msgLength, "\n");
        }
    }

    send(clientSocket, message, strlen(message), 0);
}

int main() {
    WSADATA wsa;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in server, client;
    int c;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }


    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);


    if (bind(serverSocket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed\n");
        return 1;
    }


    listen(serverSocket, 3);
    printf("Waiting for incoming connections...\n");


    c = sizeof(struct sockaddr_in);
    clientSocket = accept(serverSocket, (struct sockaddr *)&client, &c);
    if (clientSocket == INVALID_SOCKET) {
        printf("Accept failed\n");
        return 1;
    }

    printf("Client connected\n");


    sendBoard(clientSocket);


    while (1) {
        int row, col, num;
        char response[500];


        int recvSize = recv(clientSocket, response, sizeof(response), 0);
        if (recvSize == SOCKET_ERROR) {
            printf("Receive failed\n");
            break;
        }
        response[recvSize] = '\0';


        if (sscanf(response, "%d %d %d", &row, &col, &num) != 3) {
            send(clientSocket, "Invalid input format. Please enter row, column, and number.", 62, 0);
            continue;
        }


        if (isValidMove(row, col, num)) {
            board[row][col] = num;
            send(clientSocket, "Valid Move!", 12, 0);
        } else {
            send(clientSocket, "Invalid Move. Try Again.", 24, 0);
        }


        sendBoard(clientSocket);
    }


    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
