# AV Evasion Techniques Research

This repository contains a collection of research-based Proof-of-Concept (PoC) code snippets demonstrating various techniques used to bypass security controls like Antivirus (AV) and Endpoint Detection and Response (EDR) systems.

## Disclaimer
**IMPORTANT:** This project is for **educational and authorized security testing purposes only**. The techniques shown here should only be used in environments where you have explicit permission to test. Using these techniques against systems you do not own is illegal and unethical. The author takes no responsibility for any misuse.

## Features
This repository includes implementations of:
- **Memory Execution:** Techniques for running code directly in memory.
- **Obfuscation:** XOR-based encryption/decryption for payloads.
- **API Hashing:** Hiding function names to avoid static detection.
- **Direct Syscalls:** Bypassing standard Windows API hooks.
- **Memory Patching:** Patching AMSI/ETW in-memory.
- **Process Manipulation:** PPID spoofing, Thread Hijacking, and Stack Spoofing.

## Structure
The project is organized by technique. Each folder contains the source code for a specific method.
- `techniques/`: Contains C and Assembly files for each evasion method.

## Usage
1. Clone the repository.
2. Compile the C code using a compiler like MinGW or Visual Studio.
3. Use the provided tools to obfuscate your payloads before testing.

## Contributing
This is an open research project. If you have improvements or new techniques, feel free to open a Pull Request.

## License
This project is licensed under the MIT License. See the LICENSE file for details.
