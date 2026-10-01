cmake_minimum_required(VERSION 3.16)

project(SecureModbus
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(securemodbus_gateway
    gateway/gateway.cpp
)

add_executable(plc_simulator
    simulator/plc_simulator.cpp
)

add_executable(modbus_client
    simulator/modbus_client.cpp
)

add_executable(packet_inspector
    gateway/packet_inspector.cpp
)

add_executable(modbus_parser
    gateway/modbus_parser.cpp
)

add_executable(validator
    gateway/validator.cpp
)

add_executable(policy_manager
    gateway/policy_manager.cpp
)

add_executable(logger
    gateway/logger.cpp
)

add_executable(invalid_client
    tests/invalid_client.cpp
)

add_executable(invalid_unit
    tests/invalid_unit.cpp
)

add_executable(invalid_quantity
    tests/invalid_quantity.cpp
)

add_executable(large_quantity
    tests/large_quantity.cpp
)

add_executable(invalid_protocol
    tests/invalid_protocol.cpp
)

add_executable(malformed_client
    tests/malformed_client.cpp)
