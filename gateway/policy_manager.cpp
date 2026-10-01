#include <iostream>
#include <fstream>
#include <string>

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

bool checkPolicy(int functionCode, int quantity, int unitId) {

    if (functionCode != allowFunction) {
        std::cout << "BLOCK: Function Code not allowed\n";
        return false;
    }

    if (quantity <= 0 || quantity > maxQuantity) {
        std::cout << "BLOCK: Quantity exceeds policy\n";
        return false;
    }

    if (unitId < minUnitId || unitId > maxUnitId) {
        std::cout << "BLOCK: Unit ID outside policy\n";
        return false;
    }

    std::cout << "POLICY: ALLOW\n";

    return true;
}

int main() {

    if (!loadPolicy()) {
        return 1;
    }

    std::cout << "Policy loaded successfully\n";

    std::cout << "Allowed Function : "
              << allowFunction << "\n";

    std::cout << "Maximum Quantity : "
              << maxQuantity << "\n";

    std::cout << "Unit ID Range    : "
              << minUnitId << " - "
              << maxUnitId << "\n";

    std::cout << "\nTesting valid request...\n";

    checkPolicy(3, 1, 1);

    std::cout << "\nTesting invalid function...\n";

    checkPolicy(6, 1, 1);

    return 0;
}
