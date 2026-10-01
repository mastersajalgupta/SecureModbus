# SecureModbus Testing Documentation

## 1. Overview

SecureModbus is tested using both positive and negative test cases.

The testing verifies that:

- Valid Modbus-TCP requests are allowed.
- Invalid packets are blocked.
- Security policies are enforced.
- Valid packets are forwarded to the PLC simulator.
- PLC responses are validated before being returned to the client.
- Security decisions are logged.

---

## 2. Test Environment

The project is tested on Linux using:

- Ubuntu/WSL2
- C++17
- GCC 13.3.0
- CMake 3.28.3
- TCP sockets
- Localhost networking

The demonstration uses:

```text
SecureModbus Gateway : TCP 1503
PLC Simulator        : TCP 1502

