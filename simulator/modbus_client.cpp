#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket < 0) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    sockaddr_in serverAddress{};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(1503);
    inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

    if (connect(clientSocket,
                (struct sockaddr*)&serverAddress,
                sizeof(serverAddress)) < 0) {
        std::cerr << "Connection failed\n";
        close(clientSocket);
        return 1;
    }

    std::cout << "Connected to SecureModbus Gateway\n";

    unsigned char request[] = {
        0x00, 0x01,
        0x00, 0x00,
        0x00, 0x06,
        0x01,
        0x03,
        0x00, 0x00,
        0x00, 0x01
    };

    send(clientSocket, request, sizeof(request), 0);

    std::cout << "Modbus request sent\n";

    unsigned char response[256];

    int bytesReceived = recv(
        clientSocket,
        response,
        sizeof(response),
        0
    );

    if (bytesReceived > 0) {
        std::cout << "Response received: ";

        for (int i = 0; i < bytesReceived; i++) {
            printf("%02X ", response[i]);
        }

        std::cout << "\n";

        if (bytesReceived >= 11) {
            int temperature =
                (response[9] << 8) | response[10];

            std::cout << "Temperature = "
                      << temperature
                      << "\n";
        }
    }

    close(clientSocket);

    return 0;
}
