#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstdint>
#include <fstream>
#include <ctime>
int allowFunction = 0;
int maxQuantity = 0;
int minUnitId = 0;
int maxUnitId = 0;

bool loadPolicy() {

    std::ifstream file("configs/rules.conf");

    if (!file) {
        std::cout << "Could not open policy file\n";
        return false;
    }

    std::string line;

    while (std::getline(file, line)) {

        if (line.find("ALLOW_FUNCTION=") == 0) {
            allowFunction = std::stoi(line.substr(15));
        }

        else if (line.find("MAX_QUANTITY=") == 0) {
            maxQuantity = std::stoi(line.substr(13));
        }

        else if (line.find("MIN_UNIT_ID=") == 0) {
            minUnitId = std::stoi(line.substr(12));
        }

        else if (line.find("MAX_UNIT_ID=") == 0) {
            maxUnitId = std::stoi(line.substr(12));
        }
    }

    file.close();

    return true;
}
void writeLog(std::string decision,
              int functionCode,
              int unitId,
              int quantity,
              std::string reason) {

    std::ofstream file("logs/securemodbus.log", std::ios::app);

    if (!file) {
        std::cout << "Could not open log file\n";
        return;
    }

    time_t now = time(nullptr);

    std::string timestamp = ctime(&now);

    if (!timestamp.empty() && timestamp.back() == '\n') {
        timestamp.pop_back();
    }

    file << "[" << timestamp << "] "
         << decision
         << " Function=" << functionCode
         << " Unit=" << unitId
         << " Quantity=" << quantity
         << " Reason=" << reason
         << "\n";

    file.close();
}
bool validatePacket(unsigned char packet[], int size) {

    if (size < 12) {

        std::cout << "BLOCK: Packet too short\n";

        writeLog(
            "BLOCK",
            0,
            0,
            0,
            "Packet too short"
        );

        return false;
    }

    uint16_t protocolId =
        (packet[2] << 8) | packet[3];

    uint8_t unitId = packet[6];

    uint8_t functionCode = packet[7];

    uint16_t quantity =
        (packet[10] << 8) | packet[11];

    std::cout << "\nPacket Inspection\n";

    std::cout << "Protocol ID   : "
              << protocolId << "\n";

    std::cout << "Unit ID       : "
              << static_cast<int>(unitId) << "\n";

    std::cout << "Function Code : "
              << static_cast<int>(functionCode) << "\n";

    std::cout << "Quantity      : "
              << quantity << "\n";


    if (protocolId != 0) {

        std::cout
            << "BLOCK: Invalid Protocol ID\n";

        writeLog(
            "BLOCK",
            functionCode,
            unitId,
            quantity,
            "Invalid Protocol ID"
        );

        return false;
    }


    if (unitId < minUnitId ||
        unitId > maxUnitId) {

        std::cout
            << "BLOCK: Unit ID outside policy\n";

        writeLog(
            "BLOCK",
            functionCode,
            unitId,
            quantity,
            "Unit ID outside policy"
        );

        return false;
    }


    if (functionCode != allowFunction) {

        std::cout
            << "BLOCK: Function Code not allowed\n";

        writeLog(
            "BLOCK",
            functionCode,
            unitId,
            quantity,
            "Function Code not allowed"
        );

        return false;
    }


    if (quantity == 0) {

        std::cout
            << "BLOCK: Invalid quantity\n";

        writeLog(
            "BLOCK",
            functionCode,
            unitId,
            quantity,
            "Quantity below minimum"
        );

        return false;
    }


    if (quantity > maxQuantity) {

        std::cout
            << "BLOCK: Quantity exceeds policy\n";

        writeLog(
            "BLOCK",
            functionCode,
            unitId,
            quantity,
            "Quantity exceeds policy"
        );

        return false;
    }


    std::cout
        << "SECURITY DECISION: ALLOW\n";

    writeLog(
        "ALLOW",
        functionCode,
        unitId,
        quantity,
        "Valid request"
    );

    return true;
}
bool validateResponse(
    unsigned char response[],
    int responseBytes,
    unsigned char request[]
) {

    if (responseBytes < 9) {
        std::cout << "BLOCK: Response too short\n";
        return false;
    }

    uint16_t responseTransactionId =
        (response[0] << 8) | response[1];

    uint16_t requestTransactionId =
        (request[0] << 8) | request[1];

    uint16_t protocolId =
        (response[2] << 8) | response[3];

    uint8_t unitId = response[6];

    uint8_t functionCode = response[7];

    std::cout << "\nResponse Validation\n";
    std::cout << "Transaction ID : "
              << responseTransactionId << "\n";
    std::cout << "Protocol ID    : "
              << protocolId << "\n";
    std::cout << "Unit ID        : "
              << static_cast<int>(unitId) << "\n";
    std::cout << "Function Code  : "
              << static_cast<int>(functionCode) << "\n";

    if (responseTransactionId != requestTransactionId) {
        std::cout << "BLOCK: Transaction ID mismatch\n";
        return false;
    }

    if (protocolId != 0) {
        std::cout << "BLOCK: Invalid response Protocol ID\n";
        return false;
    }

    if (unitId != request[6]) {
        std::cout << "BLOCK: Response Unit ID mismatch\n";
        return false;
    }

    if (functionCode != request[7]) {
        std::cout << "BLOCK: Response Function Code mismatch\n";
        return false;
    }

    if (response[8] != 2) {
        std::cout << "BLOCK: Invalid response byte count\n";
        return false;
    }

    std::cout << "RESPONSE VALIDATION: PASS\n";

    return true;
}
int main() {

    if (!loadPolicy()) {
        return 1;
    }

    std::cout << "SecureModbus Policy Loaded\n";
    std::cout << "Allowed Function : "
              << allowFunction << "\n";
    std::cout << "Maximum Quantity : "
              << maxQuantity << "\n";
    std::cout << "Unit ID Range    : "
              << minUnitId << " - "
              << maxUnitId << "\n\n";

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket < 0) {
        std::cerr << "Socket creation failed\n";
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

    sockaddr_in gatewayAddress{};

    gatewayAddress.sin_family = AF_INET;
    gatewayAddress.sin_addr.s_addr = INADDR_ANY;
    gatewayAddress.sin_port = htons(1503);

    if (bind(
        serverSocket,
        (struct sockaddr*)&gatewayAddress,
        sizeof(gatewayAddress)
    ) < 0) {

        std::cerr << "Gateway bind failed\n";
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 5) < 0) {

        std::cerr << "Gateway listen failed\n";
        close(serverSocket);
        return 1;
    }

    std::cout << "SecureModbus Gateway running on TCP port 1503...\n";

    while (true) {

        std::cout << "\nWaiting for client...\n";

        sockaddr_in clientAddress{};
        socklen_t clientLength = sizeof(clientAddress);

        int clientSocket = accept(
            serverSocket,
            (struct sockaddr*)&clientAddress,
            &clientLength
        );

        if (clientSocket < 0) {
            std::cerr << "Client connection failed\n";
            continue;
        }

        std::cout << "Client connected to gateway.\n";

        unsigned char buffer[256];

        int bytesReceived =
            recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived <= 0) {

            std::cerr << "No packet received\n";

            close(clientSocket);
            continue;
        }

        std::cout << "Received "
                  << bytesReceived
                  << " bytes\n";

        std::cout << "Client Packet: ";

        for (int i = 0; i < bytesReceived; i++) {
            printf("%02X ", buffer[i]);
        }

        std::cout << "\n";

        bool allowed =
            validatePacket(buffer, bytesReceived);

        if (!allowed) {

            std::cout
                << "Packet dropped by SecureModbus.\n";

            close(clientSocket);
            continue;
        }

        int plcSocket =
            socket(AF_INET, SOCK_STREAM, 0);

        if (plcSocket < 0) {

            std::cerr
                << "PLC socket creation failed\n";

            close(clientSocket);
            continue;
        }

        sockaddr_in plcAddress{};

        plcAddress.sin_family = AF_INET;
        plcAddress.sin_port = htons(1502);

        inet_pton(
            AF_INET,
            "127.0.0.1",
            &plcAddress.sin_addr
        );

        if (connect(
            plcSocket,
            (struct sockaddr*)&plcAddress,
            sizeof(plcAddress)
        ) < 0) {

            std::cerr
                << "Could not connect to PLC\n";

            close(plcSocket);
            close(clientSocket);
            continue;
        }

        std::cout
            << "Connected to PLC Simulator.\n";

        send(
            plcSocket,
            buffer,
            bytesReceived,
            0
        );

        std::cout
            << "Packet forwarded to PLC.\n";

        unsigned char response[256];

        int responseBytes =
            recv(
                plcSocket,
                response,
                sizeof(response),
                0
            );
            if (responseBytes > 0) {

    std::cout
        << "PLC Response: ";

    for (int i = 0; i < responseBytes; i++) {
        printf("%02X ", response[i]);
    }

    std::cout << "\n";

    bool responseValid =
        validateResponse(
            response,
            responseBytes,
            buffer
        );

    if (responseValid) {

        send(
            clientSocket,
            response,
            responseBytes,
            0
        );

        std::cout
            << "Response forwarded to client.\n";

    } else {

        std::cout
            << "Response blocked by SecureModbus.\n";
    }
}

        close(plcSocket);
        close(clientSocket);

        std::cout << "Client session completed.\n";
    }

    close(serverSocket);

    return 0;
}
