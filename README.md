
**Ananya Batra**

In this project, I wrote programs to encrypt and decrypt a file using Schmidth Samoa cryptography, which includes generating a public and private key for a user. A file can be encrypted by using this user's public key and can only be decrypted correctly through the user's private key (which in the real world, would only be available to this user, meaning that the encrypted file can only be decrypted by its intended user). Before a user can encrypt or decrypt a file, they need to generate their public and private keys. keygen.c generates a user's public and private keys, calling upon ss.c functions, which in turn calls upon numtheory.c functions and randstate.c to do this. ss.c contains all the functions necessary to perform schmidt samoa cryptography(making public and private keys, reading and writing these keys, and encrypting and decrypting files), utilizing numtheory.c which contains the mathematical functions required to generate the necessary components for SS cryptography. Once the keys are generated, we can encrypt a file (pretending to be a new user) using the original user's public key with encrypt.c. This program takes in a file, encrypts that file with the user's public key, and outputs the encrypted message in another file. The orginal user can then take this outputted file, and decrypt it using decrypt.c which uses the user's private key to decrypt the encrypted text. Both decrypt.c and encrypt.c also make calls to ss.c. Makefile compiles all the c program files and builds the keygen, encrypt, and decrypt executables.

Files Needed to build the program:
- universe.h: interface that contains the function declarations for universe
- universe.c: defines the universe struct and the functions declared in universe.h
- gap.h: contains the gaps array used in shell.c
- sorting.c: contains main() and creates a universe to run the Game of Life on
- Makefile: Helps build the program by compiling all the c program files, formatting (make format) files, producing the life executable, and removing (make clean) all compiler generated files. The commands make all, make, make life build the life executable.

To build the program:
1. In your terminal, set the file path to wherever all the files listed above are stored
2. Call make or make all to compile all programs. Call make keygen to build the keygen executable, call make encrypt to build the encrypt executable, and call make the decrypt executable
3. If running keygen: type ./keygen -[-b:i:n:d:s:vh] [-b bits] [-i iterations] [-n public key file] [-d private key file] [-s seed] (choosing the command-line options you want). For example, calling ./keygen -b 300 -n ss2.pub -d ss2.priv will generate a public key with at least 300 bits, and will write the username and public key to ss2.pub and write pq and d to ss2.priv.
4. If running encrypt: type ./encrypt -[-i:o:n:vh] [-i infile] [-o outfile] [-n public key file] (choosing the command-line options you want). For example, calling ./encrypt -o encrypt.txt will require you to type the message you want to encrypt on the terminal (terminating the message with CONTROL + d) and will print the encrypted text in excrypt.txt.
5. If running encrypt: type ./decrypt -[-i:o:n:vh] [-i infile] [-o outfile] [-n private key file] (choosing the command-line options you want). For example, calling ./decrypt -i encrypt.txt will read in the encrypted text from encrypt.txt and print the decryption to the terminal.

Command-line options for keygen: 
-   -h              Display program help and usage.
-   -v              Display verbose program output.
-   -b bits         Minimum bits needed for public key n (default: 256).
-   -i iterations   Miller-Rabin iterations for testing primes (default: 50).
-   -n pbfile       Public key file (default: ss.pub).
-   -d pvfile       Private key file (default: ss.priv).
-   -s seed         Random seed for testing.

Command-line options for encrypt:
-   -h              Display program help and usage.
-   -v              Display verbose program output.
-   -i infile       Input file of data to encrypt (default: stdin).
-   -o outfile      Output file for encrypted data (default: stdout).
-   -n pbfile       Public key file (default: ss.pub).

Command-line options for decrypt:
-   -h              Display program help and usage.
-   -v              Display verbose program output.
-   -i infile       Input file of data to decrypt (default: stdin).
-   -o outfile      Output file for decrypted data (default: stdout).
-   -n pvfile       Private key file (default: ss.priv).


