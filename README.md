# SecureModbus

## Industrial Modbus-TCP Packet Sanitizer & Security Gateway

SecureModbus is a Linux-based C++ security gateway designed to inspect,
validate, filter, and log Modbus-TCP communication between an industrial
control client and a PLC.

## Technology

- C++
- Linux
- TCP/IP
- Modbus-TCP
- CMake
- Linux networking APIs

## Architecture

Control Client
      |
      | Modbus-TCP
      v
SecureModbus Gateway
      |
      | Validated Traffic
      v
PLC Simulator

## Main Features

- Modbus-TCP packet parsing
- Packet validation
- Security policy enforcement
- Allow/block decisions
- Invalid packet detection
- Security logging
- Linux networking integration

## Project Status

Under development.
