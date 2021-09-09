#ifndef ENCRYT_H_
#define ENCRYT_H_

#include <iostream>
#include "aes.h"
#include "file.h"

using namespace std;

/**
 * This function will do substituition following the table of
 * values. In this way, it will be used the s-box. 
 */
void sub_bytes(unsigned char * state, int len) {
    for (int i = 0; i < len; i++) {
		state[i] = s_box[state[i]];
	}

}

/**
 * This function will do the left shift on rows.
 * The row 0 does not suffer shift. 
 * The row 1 suffers one shift. 
 * The row 2 suffers two shifts. 
 * The row 3 suffers three shifts. 
 */
void shift_rows(unsigned char * state) {
    for(int i = 0; i < KEYSIZE; i += 4) {
        unsigned char row[KEYSIZE/4];
        int mod = 4;
        for(int j = 0; j < KEYSIZE/4; j++) {
            row[j] = state[(j*4)+(i/4)];

        }
        for(int j = 0; j < KEYSIZE/4; j++) {
            int shift_index = (( ((j*4)+(i/4)-(i)) % KEYSIZE) + KEYSIZE) % KEYSIZE;
            state[shift_index] = row[j];
        }
    }

}

/**
 * It realizes the sum (which is now XOR) of the products 
 * (which is now the entire operation described on galois_multiplication(),
 * read below) of matrices multiplication between the column and the follow matrix:
 * 
 *          02 03 01 01
 *          01 02 03 01
 *          01 01 02 03
 *          03 01 01 02 
 */
void galois_mix_column(unsigned char * col) {
    unsigned char a[4];
    unsigned char b[4];
	unsigned char h;

    for (int i = 0; i < 4; i++) {
        a[i] = col[i];
        /**
         * If looked at carefully, galois_multiplication(n,2) is simply a single shift and a conditional exclusive or, and
         * galois_multiplication(n,2) can be simplified with a galois_multiplication(n,2) with an exclusive or, so
         * we can avoid the for loop, saving computacional resources. In order to that, we don't call the 
         * Galois multiplication on encryption.
         *
         * b[i] = galois_multiplication(col[i], 0x02); is the same that lines below
         */
        h = col[i] & 0x80;
		b[i] = col[i] << 1;
		if(h == 0x80) {
			b[i] ^= 0x1b;
        }
    }
    col[0] = b[0] ^ a[3] ^ a[2] ^ b[1] ^ a[1];
    col[1] = b[1] ^ a[0] ^ a[3] ^ b[2] ^ a[2];
    col[2] = b[2] ^ a[1] ^ a[0] ^ b[3] ^ a[3];
    col[3] = b[3] ^ a[2] ^ a[1] ^ b[0] ^ a[0];
}

/**
 * This function realizes the preparation of the mix columns calculations
 * under the Galois Field (GF). It takes the columns separately and 
 * it passes as argument to the galois_mix_columns where they will be the 
 * matrices multiplication using GF addition and multiplication.
 */
void mix_columns(unsigned char * state) {

    for(int i = 0; i < KEYSIZE; i += 4) { // Starts on 4 because there's no need to shift the first row
        unsigned char col[KEYSIZE/4];
        int mod = 4;
        for(int j = 0; j < KEYSIZE/4; j++) {
            col[j] = state[j+i];
        }
        galois_mix_column(col);
        for(int j = 0; j < KEYSIZE/4; j++) {
            state[j+i] = col[j];
        }

    }

}

/**
 * This function will do the XOR between state and
 * key. 
 */
void add_round_key(unsigned char * state, unsigned char * roundKey) {
    for (int i = 0; i < KEYSIZE; i++) {
		state[i] ^= roundKey[i];
	}
}

/**
 * This round only realizes the add round key
 */
void initial_round(unsigned char * state, unsigned char * key) {
    add_round_key(state, key);
}

/**
 * This round realizes sub bytes, shift rows,
 * mix columns add round key.
 */
void main_round(unsigned char * state, unsigned char * key, int i) {
    sub_bytes(state, KEYSIZE);
    shift_rows(state);
    mix_columns(state);
    add_round_key(state, key);
}

/**
 * This round realizes sub bytes, shift rows
 * and add round key.
 */
void final_round(unsigned char * state, unsigned char * key) {
    sub_bytes(state, KEYSIZE);
    shift_rows(state);
    add_round_key(state, key);
}

/**
 * The encryption realizes one initial round,
 * n-1 main rounds and one final round
 */
void AES_Encrypt(unsigned char * message, unsigned char * key, int rounds) {
    // for(int i = 0; i < 16; i++) {
    //     cout << hex << (int) message[i] << " ";
    // }
    unsigned char expanded_key[(rounds+1)*KEYSIZE];

    // Expand key
    expand_key(key, expanded_key, (rounds+1)*KEYSIZE);
    
    initial_round(message, expanded_key);
    for(int i = 0; i < rounds-1; i++) {
        main_round(message, &expanded_key[16 *(i+1)], i);
    }
    final_round(message, &expanded_key[((rounds+1)*KEYSIZE)-16]);


    // cout << endl << endl << "FINAL MESSAGE" << endl;
    // print_array(message);
}
#endif