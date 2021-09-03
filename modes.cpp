
#include <cstring>
#include "aes.h"
#include "encrypt.cpp"

using namespace std;

void AES_ECB(unsigned char * message, unsigned char * key, int rounds, int size, unsigned char * header, int header_size) {
    int message_len = size;
    int padded_message_len = message_len;

    /**
     * If message length is not a 16 multiple, extend the message len 
     * and fill it up with 0
     */
    if(padded_message_len % KEYSIZE != 0) {
        padded_message_len = ((padded_message_len / KEYSIZE) + 1) * KEYSIZE;
    }
    cout << padded_message_len << endl;
    // 
    unsigned char * encrypted = new unsigned char[padded_message_len];      // creates a new char array with 16 multiple length
	memcpy(encrypted, message, message_len);                                // copies the message to the expanded char array

    for(int i = message_len; i < padded_message_len; i++) {
			encrypted[i] = 0x00;                                               // padding the extra positions with 0x00
    }
    
    // Encrypt the blocks
    unsigned char final[message_len];
    for(int i = 0; i < padded_message_len; i += 16) {
        AES_Encrypt(encrypted + i, key, rounds);
        memcpy(&final[(i)], &encrypted[i], 16);
    }
    for(int i = 0; i < padded_message_len; i += 16) {
        for(int j = 0; j < 16; j++) {
            cout << hex << (((int) final[i+j] < 16) ? "0" : "") << (int) final[i+j] << " ";
        } cout << endl;}
        
    if(header == NULL) {
        write("aes-128-ecb-encrypted.in", final, size);
    }else {
        write_bmp("tux-2-ecb.bmp", final, size, header, header_size);
    }

}

void AES_CTR(unsigned char * message, unsigned char * key, int rounds, int size, unsigned char * header, int header_size) {
    int message_len = size;
    int padded_message_len = message_len;
    /**
     * If message length is not a 16 multiple, extend the message len 
     * and fill it up with 0
     */
    if(padded_message_len % KEYSIZE != 0) {
        padded_message_len = ((padded_message_len / KEYSIZE) + 1) * KEYSIZE;
    }

    unsigned char * padded = new unsigned char[padded_message_len];      // creates a new char array with 16 multiple length
	for (int i = 0; i < padded_message_len; i++) {
		if (i >= message_len) {
			padded[i] = 0x00;               // padding the extra positions with 0x00
		}
		else {
			padded[i] = message[i];      // copies the message to the expanded char array
		}
	}
    unsigned char nonce[16] = { 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70};
    unsigned char c[16];

    unsigned char final[padded_message_len];

    // Instead encrypt the message, it will encrypt the counter summed with a nonce
    for(int i = 0; i < padded_message_len/KEYSIZE; i++) {
        if(i == 0) {
            cout << endl;
        }
        memcpy(c, nonce, KEYSIZE);
        c[15] += i;
        AES_Encrypt(c, key, rounds);
        
        for(int j = 0; j < 16; j++) {
            c[j] ^= message[(16*i)+j];      // it make XOR between the encrypted counter and message byte a byte
            final[(16*i)+j] = c[j];         // saves the resultant block in char array that it will hold all resultants blocks
        }
    }
    if(header == NULL) {
        write("aes-128-ctr-encrypted.in", final, size);
    }else {
        write_bmp("tux-2-ctr.bmp", final, size, header, header_size);
    }
    
    // for(int i = 0; i < message_len; i++) {
    //     cout << hex << (((int) final[i] < 16) ? "0" : "") << (int) final[i];
    // }

}