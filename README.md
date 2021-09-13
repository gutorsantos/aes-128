# AES 

This project has the objective of implement the 128 bit AES algorithm.

The app contains 3 modes:

0. Basic mode (not listed on menu)
1. ECB mode
2. CTR mode

## Run

```
g++ -std=c++17 main.cpp -o main
./main
```
openssl enc -aes-128-ctr -in tux.bmp -out tux-compare.bmp -K 6162636465666768696a6b6c6d6e6f70 -iv 6162636465666768696a6b6c6d6e6f70 && dd if=tux.bmp of=tux-compare.bmp bs=54 count=1 conv=notrunc

openssl enc -aes-128-ecb -in selfie.bmp -out self-compare-ecb.bmp -K 6162636465666768696a6b6c6d6e6f70 && dd if=selfie.bmp of=self-compare-ecb.bmp bs=54 count=1 conv=notrunc

openssl enc -aes-128-ecb -in message.in -out message-compare-ecb.bmp -K 6162636465666768696a6b6c6d6e6f70

openssl enc -aes-128-ctr -in message.in -out message-compare-ctr.in -K 6162636465666768696a6b6c6d6e6f70 -iv 6162636465666768696a6b6c6d6e6f70 


## Basic Mode

Encrypts or decrypts a message with exact 16 bytes.

### Encrypt
The message to be encrypted must be on **message.in** and the key must be too on **secret.key**.
The result will be written inside assets folder in file **encrypted.in**.


### Decrypt
The message to be decrypted must be on **encrypted.in** and the key must be too on **secret.key**.
The result will be written inside assets folder in file **decrypted.in**.


## ECB Mode

Encrypts or decrypts a message using ecb mode.

### Encrypt
The program will ask for a filename that must be contained on assets directory.

The result will be written inside assets folder: 
- {filename}-ecb-encrypted => if the message is a image
- aes-128-ecb-encrypted => if the message is not a image

### Decrypt
The program will ask for a filename that must be contained on assets directory.

The result will be written inside assets folder: 
- {filename}-ecb-decrypted => if the message is a image
- aes-128-ecb-decrypted => if the message is not a image
## CTR Mode

Encrypts or decrypts a message using ctr mode.

### Encrypt
The program will ask for a filename that must be contained on assets directory.

The result will be written inside assets folder: 
- {filename}-ecb-encrypted => if the message is a image
- aes-128-ecb-encrypted => if the message is not a image

### Decrypt
The program will ask for a filename that must be contained on assets directory.

The result will be written inside assets folder: 
- {filename}-ctr-decrypted => if the message is a image
- aes-128-ctr-decrypted => if the message is not a image