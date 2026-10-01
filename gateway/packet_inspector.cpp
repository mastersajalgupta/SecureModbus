#include <iostream>
#include <cstdint>

bool validatePacket(
    uint16_t protocolId,
    uint8_t unitId,
    uint8_t functionCode,
    uint16_t quantity
) {
    if (protocolId != 0) {
        std::cout << "BLOCK: Invalid Protocol ID\n";
        return false;
    }

    if (unitId == 0 || unitId > 247) {
        std::cout << "BLOCK: Invalid Unit ID\n";
        return false;
    }

    if (functionCode != 3) {
        std::cout << "BLOCK: Unsupported Function Code\n";
        return false;
    }

    if (quantity == 0 || quantity > 125) {
        std::cout << "BLOCK: Invalid Quantity\n";
        return false;
    }

    return true;
}

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

    uint16_t protocolId =
        (packet[2] << 8) | packet[3];

    uint8_t unitId = packet[6];

    uint8_t functionCode = packet[7];

    uint16_t quantity =
        (packet[10] << 8) | packet[11];

    std::cout << "Modbus Packet Inspection\n";
    std::cout << "------------------------\n";

    std::cout << "Protocol ID   : "
              << protocolId << "\n";

    std::cout << "Unit ID       : "
              << static_cast<int>(unitId) << "\n";

    std::cout << "Function Code : "
              << static_cast<int>(functionCode) << "\n";

    std::cout << "Quantity      : "
              << quantity << "\n";

    std::cout << "\nSecurity Decision: ";

    if (validatePacket(
            protocolId,
            unitId,
            functionCode,
            quantity)) {

        std::cout << "ALLOW\n";

    } else {

        std::cout << "BLOCK\n";
    }

    return 0;
}
