#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket < 0) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    sockaddr_in gatewayAddress{};

    gatewayAddress.sin_family = AF_INET;
    gatewayAddress.sin_port = htons(1503);

    inet_pton(AF_INET, "127.0.0.1", &gatewayAddress.sin_addr);

    if (connect(clientSocket,
                (struct sockaddr*)&gatewayAddress,
                sizeof(gatewayAddress)) < 0) {

        std::cerr << "Connection failed\n";
        close(clientSocket);
        return 1;
    }

    std::cout << "Connected to SecureModbus Gateway\n";

    unsigned char packet[] = {
        0x00, 0x02,
        0x00, 0x00,
        0x00, 0x06,
        0xFF,
        0x03,
        0x00, 0x00,
        0x00, 0x01
    };

    std::cout << "Sending invalid Unit ID request\n";

    send(clientSocket, packet, sizeof(packet), 0);

    close(clientSocket);

    return 0;
}
