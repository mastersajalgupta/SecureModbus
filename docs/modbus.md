# Modbus-TCP in SecureModbus

## 1. Overview

Modbus-TCP is an industrial communication protocol used to exchange data between control systems and devices such as PLCs.

SecureModbus uses Modbus-TCP as the communication protocol between:

```text
Modbus Client
      |
      ▼
SecureModbus Gateway
      |
      ▼
PLC Simulator
```

The project uses C++ TCP sockets to implement the communication.

---

## 2. Modbus-TCP Communication

Modbus-TCP uses TCP as its transport protocol.

The standard Modbus-TCP port is:

```text
TCP 502
```

For the SecureModbus demonstration, local ports are used:

```text
Gateway       : TCP 1503
PLC Simulator : TCP 1502
```

This avoids requiring privileged port configuration during development.

---

## 3. Modbus-TCP Packet Structure

A Modbus-TCP request contains an MBAP header followed by the Modbus Protocol Data Unit (PDU).

The simplified structure used by SecureModbus is:

```text
+----------------------+----------------+
| Transaction ID       | 2 bytes        |
+----------------------+----------------+
| Protocol ID          | 2 bytes        |
+----------------------+----------------+
| Length               | 2 bytes        |
+----------------------+----------------+
| Unit ID              | 1 byte         |
+----------------------+----------------+
| Function Code        | 1 byte         |
+----------------------+----------------+
| Starting Address     | 2 bytes        |
+----------------------+----------------+
| Quantity             | 2 bytes        |
+----------------------+----------------+
```

The gateway extracts these fields from the received TCP data.

---

## 4. Example Request

The valid request used by the demonstration client is:

```text
00 01 00 00 00 06 01 03 00 00 00 01
```

The fields can be interpreted as:

| Field            | Value |
| ---------------- | ----: |
| Transaction ID   |     1 |
| Protocol ID      |     0 |
| Length           |     6 |
| Unit ID          |     1 |
| Function Code    |     3 |
| Starting Address |     0 |
| Quantity         |     1 |

---

## 5. Function Code

The current SecureModbus policy allows:

```text
Function Code = 3
```

Function code `3` represents:

**Read Holding Registers**

The gateway checks the function code against the configured security policy.

The current configuration contains:

```text
ALLOW_FUNCTION=3
```

A request using another function code is blocked.

For example, a request using function code `6` produces:

```text
BLOCK: Function Code not allowed
```

---

## 6. Unit ID

The Unit ID identifies the target device or logical unit in the Modbus communication.

SecureModbus currently permits:

```text
Minimum Unit ID = 1
Maximum Unit ID = 247
```

These values are configured in:

```text
configs/rules.conf
```

Example:

```text
MIN_UNIT_ID=1
MAX_UNIT_ID=247
```

A request with Unit ID `255` is blocked:

```text
BLOCK: Unit ID outside policy
```

---

## 7. Protocol ID

For Modbus-TCP, the Protocol Identifier in the MBAP header is:

```text
0
```

SecureModbus checks this value.

A packet with:

```text
Protocol ID = 1
```

is rejected with:

```text
BLOCK: Invalid Protocol ID
```

---

## 8. Quantity

The quantity field specifies the number of registers being requested.

The current SecureModbus policy requires:

```text
Quantity > 0
Quantity <= 125
```

The configured maximum is:

```text
MAX_QUANTITY=125
```

Therefore:

```text
Quantity = 0
```

is rejected as invalid.

Similarly:

```text
Quantity = 126
```

is rejected because it exceeds the configured policy.

---

## 9. Transaction ID

The Transaction Identifier is used to associate a response with its corresponding request.

For example:

```text
Request Transaction ID  = 1
Response Transaction ID = 1
```

SecureModbus compares the response transaction ID with the original request.

If the values do not match, the response is rejected:

```text
BLOCK: Transaction ID mismatch
```

---

## 10. Response Validation

The gateway validates the PLC response before forwarding it to the client.

The current response checks include:

```text
Transaction ID
Protocol ID
Unit ID
Function Code
Byte count
```

For a valid response, the gateway prints:

```text
RESPONSE VALIDATION: PASS
```

The current PLC simulator returns:

```text
00 01 00 00 00 05 01 03 02 00 48
```

The final two bytes contain the demonstration register value:

```text
0x0048 = 72
```

The client displays:

```text
Temperature = 72
```

---

## 11. Modbus-TCP Security Boundary

The gateway creates a security boundary between the client and PLC:

```text
Client
  |
  | Modbus-TCP Request
  ▼
SecureModbus Gateway
  |
  ├── Invalid → BLOCK
  |
  └── Valid
        |
        ▼
      PLC
        |
        ▼
   PLC Response
        |
        ▼
Response Validation
        |
        ▼
      Client
```

This prevents requests that fail the configured validation rules from being forwarded to the simulated PLC.

---

## 12. Current Scope

The current implementation focuses on a limited subset of Modbus-TCP functionality.

It currently demonstrates:

* Modbus-TCP request parsing
* Function Code 3
* Unit ID validation
* Protocol ID validation
* Quantity validation
* Transaction ID validation
* Response validation
* Security policy enforcement

The project does not currently implement every Modbus function code or every possible Modbus exception response.

---

## 13. Summary

SecureModbus treats the Modbus-TCP packet as structured data rather than blindly forwarding TCP traffic.

The gateway:

```text
Receive
   ↓
Parse
   ↓
Validate
   ↓
Apply Policy
   ↓
Allow / Block
   ↓
Forward Allowed Request
   ↓
Validate Response
   ↓
Return Response
```

This provides the foundation for implementing additional industrial protocol security rules in future versions.
