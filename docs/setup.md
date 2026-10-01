# SecureModbus Setup and Usage Guide

## 1. Overview

This document explains how to build, run, test, and demonstrate the SecureModbus project on Linux.

SecureModbus is a C++17 Linux-based Modbus-TCP security gateway.

The demonstration consists of:

```text
Modbus Client
     |
     v
SecureModbus Gateway :1503
     |
     v
PLC Simulator :1502
