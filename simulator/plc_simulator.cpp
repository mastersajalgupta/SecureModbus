#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket < 0) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    int option = 1;

    setsockopt(
        serverSocket,
        SOL_SOCKET,
        SO_REUSEADDR,
        &option,
        sizeof(option)
    );

    sockaddr_in serverAddress{};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(1502);

    if (bind(
        serverSocket,
        (struct sockaddr*)&serverAddress,
        sizeof(serverAddress)
    ) < 0) {

        std::cerr << "Bind failed\n";
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 5) < 0) {

        std::cerr << "Listen failed\n";
        close(serverSocket);
        return 1;
    }

    std::cout
        << "PLC Simulator running on TCP port 1502...\n";

    while (true) {

        std::cout << "Waiting for gateway...\n";

        sockaddr_in clientAddress{};
        socklen_t clientLength = sizeof(clientAddress);

        int clientSocket = accept(
            serverSocket,
            (struct sockaddr*)&clientAddress,
            &clientLength
        );

        if (clientSocket < 0) {
            std::cerr << "Accept failed\n";
            continue;
        }

        std::cout << "Gateway connected.\n";

        unsigned char buffer[256];

        int bytesReceived = recv(
            clientSocket,
            buffer,
            sizeof(buffer),
            0
        );

        if (bytesReceived > 0) {

            std::cout
                << "Received "
                << bytesReceived
                << " bytes\n";

            std::cout << "Request: ";

            for (int i = 0; i < bytesReceived; i++) {
                printf("%02X ", buffer[i]);
            }

            std::cout << "\n";

            if (bytesReceived >= 12) {

                unsigned char response[11];

                response[0] = buffer[0];
                response[1] = buffer[1];

                response[2] = 0x00;
                response[3] = 0x00;

                response[4] = 0x00;
                response[5] = 0x05;

                response[6] = buffer[6];

                response[7] = 0x03;
                response[8] = 0x02;

                response[9] = 0x00;
                response[10] = 0x48;

                send(
                    clientSocket,
                    response,
                    sizeof(response),
                    0
                );

                std::cout
                    << "Response sent: Temperature = 72\n";
            }
        }

        close(clientSocket);

        std::cout << "Gateway session completed.\n";
    }

    close(serverSocket);

    return 0;
}
