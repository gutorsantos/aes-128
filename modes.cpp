
#include <cstring>
#include "aes.h"
#include "encrypt.cpp"

using namespace std;

void AES_ECB(unsigned char * message, unsigned char * key, int rounds, int size, unsigned char * header, int header_size, char opt, string filename) {
    int message_len = size;
    int padded_message_len = message_len;

    /**
     * If message length is not a 16 multiple, extend the message len 
     * and fill it up with 0, but if it is multiple, extend the message one more block
     */
    
    //if(padded_message_len % BLOCKSIZE != 0) 
    padded_message_len = ((padded_message_len / BLOCKSIZE) + 1) * BLOCKSIZE;

    unsigned char * encrypted = (unsigned char*) malloc(padded_message_len);    // creates a new char array with 16 multiple length
	memcpy(encrypted, message, message_len);                                    // copies the message to the expanded char array

    for(int i = message_len; i < padded_message_len; i++) {
			encrypted[i] = padded_message_len-message_len;                                               // padding the extra positions with 0x00
    }

    unsigned char * final = (unsigned char*) malloc(padded_message_len);

    // If the file is not a image
    if(header != NULL) {
        filename.pop_back(); filename.pop_back(); filename.pop_back(); filename.pop_back(); // remove the extension
    }
    if(opt == 'e') {
        // Encrypt the blocks
        for(int i = 0; i < padded_message_len; i += 16) {
            AES_Encrypt(encrypted + i, key, rounds);
            memcpy(&final[(i)], &encrypted[i], 16);
        }
            
        if(header == NULL) {
            write("aes-128-ecb-encrypted.txt", final, padded_message_len);
        }else {
            write_bmp(filename+"-ecb-encrypted.bmp", final, padded_message_len, header, header_size);
        }
    } else if(opt == 'd') {
        // Decrypt the blocks
        for(int i = 0; i < padded_message_len; i += 16) {
            AES_Decrypt(encrypted + i, key, rounds);
            memcpy(&final[(i)], &encrypted[i], 16);
        }
            
        if(header == NULL) {
            write("aes-128-ecb-decrypted.txt", final, padded_message_len-message_len);   // 
        }else {
            write_bmp(filename+"-ecb-decrypted.bmp", final, padded_message_len, header, header_size);
        }
    }
    free(encrypted);
    free(final);

}


void AES_CTR(unsigned char * message, unsigned char * key, int rounds, int size, unsigned char * header, int header_size, char opt, string filename) {
    int message_len = size;
    int padded_message_len = message_len;
    /**
     * If message length is not a 16 multiple, extend the message len 
     * and fill it up with 0
     */
    if(padded_message_len % BLOCKSIZE != 0) {
        padded_message_len = ((padded_message_len / BLOCKSIZE) + 1) * BLOCKSIZE;
    }

    unsigned char * padded = (unsigned char*) malloc(padded_message_len);//new unsigned char[padded_message_len];      // creates a new char array with 16 multiple length
	for (int i = 0; i < padded_message_len; i++) {
		if (i >= message_len) {
			padded[i] = 0x00;               // padding the extra positions with 0x00
		}
		else {
			padded[i] = message[i];      // copies the message to the expanded char array
		}
	}
    unsigned char nonce[16] = { 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70};
    // unsigned char nonce[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    unsigned char c[16];

    unsigned char * final = (unsigned char*) malloc(padded_message_len);

    // Instead encrypt the message, it will encrypt the counter summed with a nonce
    for(int i = 0; i < padded_message_len/BLOCKSIZE; i++) {
        if(i == 0) {
            cout << endl;
        }
        memcpy(c, nonce, BLOCKSIZE);
        byte_array_sum(c, i, 15);
        // c[15] += i;
        AES_Encrypt(c, key, rounds);
        
        for(int j = 0; j < 16; j++) {
            c[j] ^= message[(16*i)+j];      // it make XOR between the encrypted counter and message byte a byte
            final[(16*i)+j] = c[j];         // saves the resultant block in char array that it will hold all resultants blocks
        }
    }

    // If the file is not a image
    if(header != NULL) {
        filename.pop_back(); filename.pop_back(); filename.pop_back(); filename.pop_back(); // remove the extension
    }
    if(opt == 'e') {
        if(header == NULL) {
            write("aes-128-ctr-encrypted.txt", final, size);
        }else {
            write_bmp(filename+"-ctr-encrypted.bmp", final, size, header, header_size);
        }
    }else if( opt == 'd') {
        if(header == NULL) {
            write("aes-128-ctr-decrypted.txt", final, size);
        }else {
            write_bmp(filename+"-ctr-decrypted.bmp", final, size, header, header_size);
        }

    }
    free(padded);
    free(final);


}