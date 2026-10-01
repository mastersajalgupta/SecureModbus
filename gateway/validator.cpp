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

    std::cout << "ALLOW: Packet is valid\n";
    return true;
}

int main() {

    uint16_t protocolId = 0;
    uint8_t unitId = 1;
    uint8_t functionCode = 3;
    uint16_t quantity = 1;

    validatePacket(
        protocolId,
        unitId,
        functionCode,
        quantity
    );

    return 0;
}
