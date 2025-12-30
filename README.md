Image Steganography using LSB (C Programming)
📌 Description

This project implements image-based steganography using Least Significant Bit (LSB) manipulation in C programming. The goal is to securely hide a secret text file inside an uncompressed BMP image without causing any visible change to the image.

The project demonstrates low-level file handling, bitwise operations, and data encoding/decoding techniques. During encoding, metadata such as a magic string, secret file extension, file size, and actual data are embedded into the image. The decoding process extracts the hidden data and reconstructs the original secret file.

⚙️ Features

LSB-based data hiding technique

Supports uncompressed BMP image format

Embeds:

Magic string (for validation)

Secret file extension

Secret file size

Actual secret data

Lossless image quality (no visible distortion)

Complete encoding and decoding implementation

Modular and well-structured C code

🛠 Technologies Used

Language: C

Concepts:

File handling

Bitwise operations

Pointers & structures

Binary data manipulation

Platform: Linux

Compiler: GCC

📂 Project Structure
PROJECT - 2 Steganography/
 ├── encode.c        # Encoding logic
 ├── encode.h
 ├── decode.c        # Decoding logic
 ├── decode.h
 ├── common.h        # Common macros & utilities
 ├── types.h         # User-defined data types
 ├── test_encode.c   # Test driver
 ├── beautiful.bmp   # Original carrier image
 ├── stego.bmp       # Image with embedded data
 ├── secret.txt      # Secret file to hide
 ├── output.txt      # Extracted secret file
 ├── output.bmp      # Generated stego image
 └── README.md       # Project documentation

▶️ Usage
🔹 Encoding (Hide Secret Data)
./a.out -e beautiful.bmp secret.txt output.bmp

🔹 Decoding (Extract Secret Data)
./a.out -d output.bmp

📚 Learning Outcomes

Understanding steganography and data hiding concepts

Practical implementation of LSB technique

File structure of BMP images

Binary-level data manipulation

Modular C programming

Secure data sharing concepts

🚧 Limitations

Supports only uncompressed BMP images

Designed for text-based secret files

No encryption (steganography only, not cryptography)

Not resistant to image compression or modification

👨‍💻 Author

Shubham Shinde

📜 License

This project is open-source and intended for educational and learning purposes.
