# AES 

This project has the objective of implement the 128 bit AES algorithm.

The app contains 3 modes:

1. Basic mode 
2. ECB mode
2. CTR mode

## Run

```
g++ -std=c++17 main.cpp -o main
./main
```

## Basic Mode

Encrypts or decrypts a message with exact 16 bytes.

### Encrypt
The message to be encrypted must be on **message.in** and the key must be too on **secret.key**.
The result will be written inside assets folder in file **encrypted.in**.


### Decrypt
The message to be decrypted must be on **encrypted.in** and the key must be too on **secret.key**.
The result will be written inside assets folder in file **decrypted.in**.
