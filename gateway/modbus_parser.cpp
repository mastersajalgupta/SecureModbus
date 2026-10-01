#include <iostream>
#include <iomanip>
#include <cstdint>

int main() {

    unsigned char packet[] = {
        0x00, 0x01,
        0x00, 0x00,
        0x00, 0x06,
        0x01,
        0x03,
        0x00, 0x00,
        0x00, 0x01
    };

    uint16_t transactionId =
        (packet[0] << 8) | packet[1];

    uint16_t protocolId =
        (packet[2] << 8) | packet[3];

    uint16_t length =
        (packet[4] << 8) | packet[5];

    uint8_t unitId = packet[6];

    uint8_t functionCode = packet[7];

    uint16_t startAddress =
        (packet[8] << 8) | packet[9];

    uint16_t quantity =
        (packet[10] << 8) | packet[11];

    std::cout << "Modbus-TCP Packet\n";
    std::cout << "-----------------\n";

    std::cout << "Transaction ID : "
              << transactionId << "\n";

    std::cout << "Protocol ID    : "
              << protocolId << "\n";

    std::cout << "Length         : "
              << length << "\n";

    std::cout << "Unit ID        : "
              << static_cast<int>(unitId) << "\n";

    std::cout << "Function Code  : "
              << static_cast<int>(functionCode) << "\n";

    std::cout << "Start Address  : "
              << startAddress << "\n";

    std::cout << "Quantity       : "
              << quantity << "\n";

    return 0;
}
