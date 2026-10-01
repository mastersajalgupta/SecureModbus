#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

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

    file << "[" << ctime(&now)
         << "] "
         << decision
         << " Function=" << functionCode
         << " Unit=" << unitId
         << " Quantity=" << quantity
         << " Reason=" << reason
         << "\n";

    file.close();
}

int main() {

    writeLog("ALLOW", 3, 1, 1, "Valid request");

    writeLog("BLOCK", 6, 1, 1,
             "Function Code not allowed");

    std::cout << "Log entries created successfully\n";

    return 0;
}
