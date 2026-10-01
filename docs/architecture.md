# SecureModbus Architecture

## 1. Overview

SecureModbus is a Linux-based C++ security gateway for Modbus-TCP communication.

The gateway is placed between a control client and a PLC. It receives Modbus-TCP requests, inspects the packet fields, validates them against configured security policies, and decides whether the request should be allowed or blocked.

For the project demonstration, the PLC is implemented as a C++ TCP simulator.

---

## 2. High-Level Architecture

```text
                    SecureModbus Gateway
                           |
                           |
Control Client ───────────►|───────────► PLC Simulator
     TCP :1503             |              TCP :1502
                           |
                    Packet Inspection
                           |
                    Packet Validation
                           |
                     Policy Manager
                      /           \
                  ALLOW           BLOCK
                    |               |
                    ▼               ▼
              PLC Simulator       Drop
                    |
                    ▼
             Response Validation
                    |
                    ▼
              Gateway → Client
```

---

## 3. Main Components

### 3.1 Modbus Client

The Modbus client acts as the industrial control client.

Its responsibilities are:

* Establish a TCP connection with the gateway.
* Construct a Modbus-TCP request.
* Send the request to the gateway.
* Receive the validated PLC response.
* Display the returned value.

The demonstration client connects to:

```text
127.0.0.1:1503
```

---

### 3.2 SecureModbus Gateway

The gateway is the main security component.

Its responsibilities are:

* Listen for incoming TCP connections.
* Receive Modbus-TCP packets.
* Inspect important packet fields.
* Validate the request.
* Apply security policies.
* Allow or block the request.
* Log the security decision.
* Forward allowed requests to the PLC simulator.
* Validate the PLC response.
* Forward valid responses back to the client.

The gateway listens on:

```text
TCP port 1503
```

---

### 3.3 Packet Inspection

The gateway extracts important Modbus-TCP fields from the received packet.

The inspected fields include:

```text
Transaction ID
Protocol ID
Length
Unit ID
Function Code
Quantity
```

The current security implementation uses these fields to perform validation and policy checks.

---

### 3.4 Packet Validation

The validator checks whether the received request satisfies the expected protocol and security requirements.

Current checks include:

```text
Packet length
Protocol ID
Unit ID
Function Code
Quantity
Maximum quantity
```

Invalid packets are blocked before they reach the PLC simulator.

---

### 3.5 Policy Manager

The policy manager provides configurable security rules.

The current policy file is:

```text
configs/rules.conf
```

Example:

```text
ALLOW_FUNCTION=3
MAX_QUANTITY=125
MIN_UNIT_ID=1
MAX_UNIT_ID=247
```

This allows the gateway security rules to be changed without modifying the main gateway source code.

---

### 3.6 PLC Simulator

The PLC simulator represents an industrial PLC for testing.

It:

* Listens for gateway connections.
* Receives allowed Modbus requests.
* Generates a Modbus response.
* Returns the response to the gateway.

The simulator listens on:

```text
TCP port 1502
```

The current demonstration returns the value:

```text
72
```

which the client displays as:

```text
Temperature = 72
```

---

### 3.7 Response Validation

The gateway does not blindly forward the PLC response.

It validates important response fields before sending the response back to the client.

Current checks include:

```text
Transaction ID
Protocol ID
Unit ID
Function Code
Response byte count
```

The transaction ID, unit ID, and function code are compared with the original request.

A valid response produces:

```text
RESPONSE VALIDATION: PASS
```

---

### 3.8 Logger

The gateway records security decisions in:

```text
logs/securemodbus.log
```

Log entries contain information such as:

```text
Timestamp
Decision
Function Code
Unit ID
Quantity
Reason
```

For example:

```text
BLOCK Function=3 Unit=255 Quantity=1 Reason=Unit ID outside policy
```

The log directory is excluded from Git using `.gitignore`.

---

## 4. Request Processing Flow

A normal request follows this sequence:

```text
1. Client connects to Gateway
              |
              ▼
2. Gateway receives packet
              |
              ▼
3. Packet inspection
              |
              ▼
4. Packet validation
              |
              ▼
5. Security policy check
              |
        ┌─────┴─────┐
        ▼           ▼
      ALLOW        BLOCK
        |           |
        ▼           ▼
6. Connect to     Log + Drop
   PLC
        |
        ▼
7. Forward request
        |
        ▼
8. Receive PLC response
        |
        ▼
9. Validate response
        |
        ▼
10. Forward response
    to client
```

---

## 5. Security Decision Flow

The gateway makes the following type of decision:

```text
Incoming Packet
       |
       ▼
Packet Inspection
       |
       ▼
Protocol Validation
       |
       ▼
Policy Validation
       |
       ├───────────────┐
       ▼               ▼
    Valid            Invalid
       |               |
       ▼               ▼
    ALLOW             BLOCK
       |               |
       ▼               ▼
Forward to PLC      Drop Packet
       |               |
       ▼               ▼
Validate Response      Log
       |
       ▼
Forward to Client
```

---

## 6. Network Ports

The project uses separate local TCP ports for the demonstration:

| Component            | Port | Purpose                                |
| -------------------- | ---: | -------------------------------------- |
| SecureModbus Gateway | 1503 | Receives client requests               |
| PLC Simulator        | 1502 | Receives allowed requests from gateway |

The standard Modbus-TCP port is not directly used in the demonstration. Local ports are used to make the project easier to run and test without requiring privileged port configuration.

---

## 7. Linux Networking Architecture

SecureModbus is implemented as a user-space Linux networking application.

The gateway uses POSIX/Linux socket APIs such as:

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

The simplified networking path is:

```text
SecureModbus Application
          |
          ▼
    Linux Socket API
          |
          ▼
 Linux Networking Stack
          |
          ▼
 Network Interface
          |
          ▼
 Network Device Driver
          |
          ▼
 Network Hardware
```

The gateway operates above the Linux device-driver layer.

---

## 8. Linux Device Driver Context

SecureModbus does not currently implement a custom Linux kernel device driver.

The project demonstrates the relationship between a user-space networking application and the Linux networking/device-driver stack.

The gateway communicates through Linux socket APIs, while the lower-level networking stack and network device driver are provided by Linux.

Therefore, the project should be described as:

> A Linux user-space networking and security gateway that operates above the network device-driver layer.

No custom kernel module or device driver is claimed as part of the current implementation.

---

## 9. Deployment Model

The current development setup uses Linux/WSL2.

The complete demonstration runs locally:

```text
Modbus Client
     |
     | localhost:1503
     ▼
SecureModbus Gateway
     |
     | localhost:1502
     ▼
PLC Simulator
```

A future deployment could place the gateway between a real control system and a physical PLC on separate network interfaces.

---

## 10. Architecture Summary

SecureModbus separates the communication path into clear security stages:

```text
Client
  ↓
TCP Connection
  ↓
Packet Inspection
  ↓
Packet Validation
  ↓
Policy Enforcement
  ↓
ALLOW / BLOCK
  ↓
PLC Communication
  ↓
Response Validation
  ↓
Client
```

This architecture provides a simple and demonstrable security boundary for Modbus-TCP communication while keeping the implementation entirely in C++ and Linux user space.
