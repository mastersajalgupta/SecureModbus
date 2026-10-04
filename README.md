# SecureModbus

## Industrial Modbus-TCP Packet Sanitizer & Security Gateway

SecureModbus is a Linux-based C++ security gateway that sits between a Modbus client and a PLC. It inspects incoming Modbus-TCP packets, validates their fields against security policies, and decides whether the request should be allowed or blocked.

For demonstration and testing, the PLC is simulated using a C++ TCP server.

The project demonstrates how a security gateway can protect industrial communication by validating protocol fields before forwarding traffic to the PLC.

---

## Project Objective

The objective of SecureModbus is to provide a lightweight security layer for Modbus-TCP communication.

The gateway:

1. Receives a Modbus-TCP request from a client.
2. Parses important Modbus fields.
3. Validates the packet structure and protocol fields.
4. Applies configurable security policies.
5. Allows valid requests.
6. Blocks invalid or unauthorized requests.
7. Logs security decisions.
8. Forwards allowed requests to the PLC simulator.
9. Validates the PLC response before forwarding it back to the client.

---

## System Architecture

```text
                 SecureModbus Gateway
                        |
                        |
Control Client ───────► Gateway ───────► PLC Simulator
     :1503                |                  :1502
                          |
                   Packet Inspection
                          |
                    Packet Validation
                          |
                    Policy Manager
                     /          \
                 ALLOW          BLOCK
                   |              |
                   ▼              ▼
             PLC Simulator      Drop
                   |
                   ▼
             Response Validation
                   |
                   ▼
                Client
```

### Communication Flow

```text
Modbus Client
     |
     | TCP :1503
     ▼
SecureModbus Gateway
     |
     | Validate request
     |
     ├── BLOCK ──► Log + Drop
     |
     └── ALLOW
          |
          | TCP :1502
          ▼
     PLC Simulator
          |
          | Response
          ▼
     Gateway Response Validation
          |
          ▼
     Modbus Client
```

---

## Security Checks

The gateway currently performs the following checks:

| Check                  | Policy / Validation             |
| ---------------------- | ------------------------------- |
| Packet length          | Minimum Modbus-TCP request size |
| Protocol ID            | Must be `0`                     |
| Unit ID                | Must be between `1` and `247`   |
| Function Code          | Configurable allowed function   |
| Quantity               | Must be greater than `0`        |
| Maximum quantity       | Configurable maximum of `125`   |
| Transaction ID         | Response must match request     |
| Response Protocol ID   | Must be `0`                     |
| Response Unit ID       | Must match request              |
| Response Function Code | Must match request              |
| Response byte count    | Must match expected value       |

---

## Security Policy

Security rules are stored in:

```text
configs/rules.conf
```

Current configuration:

```text
ALLOW_FUNCTION=3
MAX_QUANTITY=125
MIN_UNIT_ID=1
MAX_UNIT_ID=247
```

Function code `3` represents **Read Holding Registers** in the current demonstration.

The policy configuration allows security rules to be changed without modifying the main gateway source code.

---

## Technology Stack

* **Language:** C++
* **Operating System:** Linux
* **Protocol:** Modbus-TCP
* **Transport:** TCP/IP
* **Build System:** CMake
* **Networking:** Linux/POSIX socket APIs
* **Configuration:** Text-based policy configuration
* **Version Control:** Git/GitHub

---

## Project Structure

```text
SecureModbus/
│
├── CMakeLists.txt
├── README.md
├── .gitignore
│
├── configs/
│   └── rules.conf
│
├── gateway/
│   ├── gateway.cpp
│   ├── logger.cpp
│   ├── modbus_parser.cpp
│   ├── packet_inspector.cpp
│   ├── policy_manager.cpp
│   └── validator.cpp
│
├── simulator/
│   ├── modbus_client.cpp
│   └── plc_simulator.cpp
│
├── tests/
│   ├── invalid_client.cpp
│   ├── invalid_protocol.cpp
│   ├── invalid_quantity.cpp
│   ├── invalid_unit.cpp
│   ├── large_quantity.cpp
│   └── malformed_client.cpp
│
└── docs/
    ├── architecture.md
    ├── modbus.md
    ├── security.md
    ├── testing.md
    └── setup.md
```

---

## Build Requirements

The project requires:

* Linux
* GCC/G++
* CMake 3.16 or newer
* Git

Check the installed versions:

```bash
g++ --version
cmake --version
git --version
```

---

## Build Instructions

Clone the repository:

```bash
git clone <repository-url>
cd SecureModbus
```

Configure the project:

```bash
cmake -S . -B build
```

Build all components:

```bash
cmake --build build -j2
```

The compiled programs are placed inside:

```text
build/
```

---

## Running the System

SecureModbus uses two local TCP ports for the demonstration:

```text
PLC Simulator : 1502
Gateway       : 1503
```

### Terminal 1 — Start PLC Simulator

```bash
./build/plc_simulator
```

Expected:

```text
PLC Simulator running on TCP port 1502...
Waiting for gateway...
```

### Terminal 2 — Start Gateway

```bash
./build/securemodbus_gateway
```

Expected:

```text
SecureModbus Policy Loaded
Allowed Function : 3
Maximum Quantity : 125
Unit ID Range    : 1 - 247

SecureModbus Gateway running on TCP port 1503...

Waiting for client...
```

### Terminal 3 — Start Modbus Client

```bash
./build/modbus_client
```

Expected result:

```text
Connected to SecureModbus Gateway
Modbus request sent
Response received: 00 01 00 00 00 05 01 03 02 00 48
Temperature = 72
```

---

## Security Testing

The project contains separate clients for testing invalid requests.

### Invalid Function Code

```bash
./build/invalid_client
```

Expected:

```text
BLOCK: Function Code not allowed
Packet dropped by SecureModbus.
```

### Invalid Unit ID

```bash
./build/invalid_unit
```

Expected:

```text
BLOCK: Unit ID outside policy
Packet dropped by SecureModbus.
```

### Invalid Quantity

```bash
./build/invalid_quantity
```

Expected:

```text
BLOCK: Invalid quantity
Packet dropped by SecureModbus.
```

### Excessive Quantity

```bash
./build/large_quantity
```

Expected:

```text
BLOCK: Quantity exceeds policy
Packet dropped by SecureModbus.
```

### Invalid Protocol ID

```bash
./build/invalid_protocol
```

Expected:

```text
BLOCK: Invalid Protocol ID
Packet dropped by SecureModbus.
```

### Malformed Packet

```bash
./build/malformed_client
```

Expected:

```text
BLOCK: Packet too short
Packet dropped by SecureModbus.
```

---

## End-to-End Validation

A valid request follows this path:

```text
Client
  |
  | Valid Modbus-TCP request
  ▼
Gateway
  |
  | SECURITY DECISION: ALLOW
  ▼
PLC Simulator
  |
  | Valid response
  ▼
Gateway
  |
  | RESPONSE VALIDATION: PASS
  ▼
Client
```

The current demonstration successfully produces:

```text
Temperature = 72
```

---

## Logging

Security decisions are written to:

```text
logs/securemodbus.log
```

Example:

```text
[Thu Oct  1 06:15:59 2026] BLOCK Function=3 Unit=255 Quantity=1 Reason=Unit ID outside policy
```

The log directory is excluded from Git using `.gitignore`.

---

## Linux Networking

SecureModbus uses Linux/POSIX socket APIs for TCP communication.

The gateway uses operations such as:

```text
socket()
bind()
listen()
accept()
connect()
send()
recv()
close()
```

The project therefore demonstrates interaction with the Linux networking stack at the application level.

The system is designed to run on Linux. Development and testing were performed in a Linux environment using WSL2.

---

## Linux Device Driver Context

SecureModbus is primarily a **user-space networking and security application**. It does not currently implement a custom Linux kernel device driver.

The project relates to Linux device-driver concepts through the networking path:

```text
Application
     |
     | Linux socket API
     ▼
Linux networking stack
     |
     ▼
Network interface
     |
     ▼
Network device driver
     |
     ▼
Network hardware
```

The gateway operates above the device-driver layer and uses Linux networking interfaces rather than implementing a new kernel driver.

This distinction is intentional: the project does not claim to contain a custom kernel module or device driver that has not been implemented.

---

## Limitations

* The PLC is simulated rather than connected to physical industrial hardware.
* The current demonstration uses local TCP ports `1502` and `1503` instead of directly exposing TCP port `502`.
* The current policy focuses on selected Modbus-TCP fields and Function Code 3.
* The project currently operates as a user-space gateway.
* Testing was performed in a Linux/WSL2 environment.

---

## Future Improvements

Possible future extensions include:

* Support for additional Modbus function codes.
* More detailed Modbus exception-response handling.
* Stronger request/response length validation.
* Automated CTest-based security tests.
* Connection and rate-control policies.
* More detailed security event logging.
* Deployment on a native Linux machine with a physical network interface.
* Integration with Linux firewall/network filtering mechanisms.
* Optional kernel-level networking experiments where supported by the deployment environment.

---

## Project Status

**Functional prototype completed**

The current implementation provides:

* Modbus-TCP client
* PLC simulator
* Security gateway
* Packet inspection
* Packet validation
* Configurable security policy
* Allow/block decisions
* Security logging
* Response validation
* Invalid packet test clients
* CMake build system
* Linux TCP socket communication
* End-to-end client-to-PLC demonstration

---

## Author

**Sajal Gupta**

B.Tech Computer Science Engineering

Institute of Technical Education & Research (ITER), SOA University

---

## License

This project is intended for academic and educational purposes.
