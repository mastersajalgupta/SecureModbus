# SecureModbus Security Design

## 1. Security Objective

SecureModbus is designed to prevent invalid or unauthorized Modbus-TCP requests from being forwarded to the PLC.

The gateway acts as a security boundary between the control client and the PLC:

```text id="u8u5p1"
Control Client
      |
      ▼
SecureModbus Gateway
      |
      ▼
PLC
```

Instead of blindly forwarding every request, the gateway inspects and validates the packet before forwarding it.

---

## 2. Security Processing Pipeline

Every incoming request follows this pipeline:

```text id="f0vh4v"
Incoming TCP Packet
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
        ▼
Security Decision
      /       \
   ALLOW      BLOCK
     |           |
     ▼           ▼
 Forward       Drop
 to PLC        Packet
     |
     ▼
Response Validation
     |
     ▼
Forward to Client
```

---

## 3. Packet Length Validation

The gateway first checks whether the received packet is large enough to contain the required Modbus-TCP request fields.

If the packet is too short, it is rejected.

Example:

```text id="2nqv1v"
Received 5 bytes

BLOCK: Packet too short
Packet dropped by SecureModbus.
```

This prevents the gateway from attempting to interpret incomplete packet data as a valid Modbus request.

---

## 4. Protocol ID Validation

The Modbus-TCP Protocol Identifier is expected to be:

```text id="3f2qzx"
0
```

SecureModbus checks this field before forwarding the request.

Example invalid request:

```text id="ldl8be"
Protocol ID = 1
```

Result:

```text id="v2d7w8"
BLOCK: Invalid Protocol ID
```

---

## 5. Unit ID Policy

The gateway applies a configurable Unit ID range.

Current policy:

```text id="k8yxk1"
MIN_UNIT_ID=1
MAX_UNIT_ID=247
```

The gateway blocks Unit IDs outside this range.

For example:

```text id="b7d6n3"
Unit ID = 255
```

produces:

```text id="qk4x6u"
BLOCK: Unit ID outside policy
```

---

## 6. Function Code Policy

The current policy permits Function Code `3`.

```text id="iyh8f3"
ALLOW_FUNCTION=3
```

Function Code 3 corresponds to Read Holding Registers in the current demonstration.

If another function code is received, the gateway blocks it.

Example:

```text id="b9q5vd"
Function Code = 6
```

Result:

```text id="6d2p9s"
BLOCK: Function Code not allowed
```

This provides a simple allow-list mechanism for Modbus operations.

---

## 7. Quantity Validation

The gateway validates the requested quantity.

The current rules are:

```text id="w5k5ob"
Quantity > 0
Quantity <= 125
```

### Quantity Zero

A request with:

```text id="f6r6gx"
Quantity = 0
```

is blocked:

```text id="m88k7c"
BLOCK: Invalid quantity
```

### Quantity Above Maximum

A request with:

```text id="3d7a9k"
Quantity = 126
```

is blocked:

```text id="g6c7y1"
BLOCK: Quantity exceeds policy
```

---

## 8. Policy Configuration

Security rules are stored in:

```text id="c74m4f"
configs/rules.conf
```

Current configuration:

```text id="qtx9ra"
ALLOW_FUNCTION=3
MAX_QUANTITY=125
MIN_UNIT_ID=1
MAX_UNIT_ID=247
```

The gateway loads these values when it starts.

This separates security policy from the main gateway implementation.

---

## 9. Allow Decision

A packet is allowed only after it passes all current checks.

For a valid request, the gateway prints:

```text id="z3a2kh"
SECURITY DECISION: ALLOW
```

The request is then forwarded to the PLC simulator.

Example valid request:

```text id="0n6y3w"
00 01 00 00 00 06 01 03 00 00 00 01
```

The gateway accepts the request because:

```text
Protocol ID = 0
Unit ID = 1
Function Code = 3
Quantity = 1
```

---

## 10. Block Decision

If any required security check fails, the gateway blocks the request.

The packet is not forwarded to the PLC.

The gateway reports:

```text id="2o9r1f"
BLOCK: <reason>
Packet dropped by SecureModbus.
```

This prevents invalid requests from reaching the PLC simulator.

---

## 11. Security Logging

Security decisions are recorded by the logging component.

The log file is:

```text id="c6v8cr"
logs/securemodbus.log
```

A log entry contains:

```text id="0h4w72"
Timestamp
Decision
Function Code
Unit ID
Quantity
Reason
```

Example:

```text id="k8w1qf"
[Thu Oct  1 06:15:59 2026] BLOCK Function=3 Unit=255 Quantity=1 Reason=Unit ID outside policy
```

Logging provides a record of blocked requests and their reasons.

---

## 12. Response Security Validation

Security checks are also applied to the response received from the PLC simulator.

The gateway validates:

```text id="2f1xq9"
Transaction ID
Protocol ID
Unit ID
Function Code
Response byte count
```

The transaction ID, Unit ID, and Function Code are compared with the original request.

A valid response produces:

```text id="u1j4qx"
RESPONSE VALIDATION: PASS
```

Only after successful validation is the response forwarded to the client.

---

## 13. Transaction Matching

The gateway uses the Modbus transaction ID to associate a response with the corresponding request.

Example:

```text id="x9t4c8"
Request Transaction ID  = 1
Response Transaction ID = 1
```

If they differ:

```text id="s8y3m2"
BLOCK: Transaction ID mismatch
```

This prevents a response with an unexpected transaction identifier from being accepted as the response to the current request.

---

## 14. Defense-in-Depth Flow

The current design provides multiple validation stages:

```text id="r7j8e0"
Layer 1
Packet Length
     ↓
Layer 2
Protocol Validation
     ↓
Layer 3
Unit ID Validation
     ↓
Layer 4
Function Code Policy
     ↓
Layer 5
Quantity Validation
     ↓
Layer 6
PLC Forwarding
     ↓
Layer 7
Response Validation
```

The purpose is to avoid relying on a single validation check.

---

## 15. Tested Security Cases

The current project includes test clients for:

| Test                    | Expected Decision |
| ----------------------- | ----------------- |
| Valid Function Code 3   | ALLOW             |
| Function Code 6         | BLOCK             |
| Unit ID 255             | BLOCK             |
| Quantity 0              | BLOCK             |
| Quantity 126            | BLOCK             |
| Protocol ID 1           | BLOCK             |
| 5-byte malformed packet | BLOCK             |
| Valid PLC response      | PASS              |

These tests demonstrate that the gateway can distinguish between valid and invalid requests.

---

## 16. Current Security Scope

The current implementation focuses on protocol-level validation and policy enforcement.

It does not currently implement:

* Encryption
* Authentication
* TLS
* Intrusion detection
* Deep packet inspection of every Modbus function
* Kernel-level security modules
* Custom Linux kernel device drivers

These are outside the current implementation scope.

---

## 17. Linux Security Context

SecureModbus operates as a Linux user-space application.

The security boundary is implemented using C++ application logic and Linux TCP socket APIs.

The simplified stack is:

```text id="e7h2w4"
SecureModbus Gateway
        |
        ▼
Linux Socket API
        |
        ▼
Linux Networking Stack
        |
        ▼
Network Device Driver
        |
        ▼
Network Interface
```

The current project does not claim to implement a custom Linux kernel driver.

---

## 18. Future Security Improvements

Possible future improvements include:

* Support for additional Modbus function codes.
* More detailed Modbus exception validation.
* Stronger MBAP length validation.
* Connection rate limiting.
* Request rate monitoring.
* Security event statistics.
* Integration with Linux firewall mechanisms.
* Deployment between separate physical network interfaces.
* Additional Linux security mechanisms where appropriate.

---

## 19. Security Summary

SecureModbus implements a simple security gateway model:

```text id="w6b3q9"
Receive
  ↓
Inspect
  ↓
Validate
  ↓
Apply Policy
  ↓
ALLOW / BLOCK
  ↓
Forward Allowed Traffic
  ↓
Validate Response
  ↓
Return Valid Response
```

The main security principle is:

> Validate before forwarding.

This ensures that requests failing the configured security rules are stopped at the gateway instead of being forwarded to the PLC.
