#include <iostream>
#include <cstring>
#include "aes.h"
#include "file.h"

using namespace std;

/**
 * This function will do substituition following the table of
 * values. In this way, it will be used the inverse s-box. 
 */
void inverse_sub_bytes(unsigned char * state, int len) {
    for (int i = 0; i < len; i++) {
		state[i] = inverse_s_box[state[i]];
	}

}

/**
 * This function will do the right shift on rows in order
 * to do the inverse operation, revert the left shift realized 
 * on encryption.
 */
void inverse_shift_rows(unsigned char * state) {
    for(int i = 0; i < BLOCKSIZE; i += 4) {
        unsigned char row[BLOCKSIZE/4];
        int mod = 4;
        for(int j = 0; j < BLOCKSIZE/4; j++) {
            row[j] = state[(j*4)+(i/4)];        // Get the row

        }
        for(int j = 0; j < BLOCKSIZE/4; j++) {
            int shift_index = (( ((j*4)+(i/4)+(i)) % BLOCKSIZE) + BLOCKSIZE) % BLOCKSIZE;
            state[shift_index] = row[j];
        }

    }

}


/**
 * To perform the inverse operation of galois_mix_columns,
 * it must be done the matrices multiplication between the column and
 * the follow matrix:
 *          
 *          14 11 13 09
 *          09 14 11 13
 *          13 09 14 11
 *          11 13 09 14
 */
void inverse_galois_mix_columns(unsigned char * col) {
    unsigned char a[4];
    for (int i = 0; i < 4; i++) {
        a[i] = col[i];
    }
    col[0] = galois_multiplication(a[0],14) ^ galois_multiplication(a[3],9) ^ galois_multiplication(a[2],13) ^ galois_multiplication(a[1],11);
    col[1] = galois_multiplication(a[1],14) ^ galois_multiplication(a[0],9) ^ galois_multiplication(a[3],13) ^ galois_multiplication(a[2],11);
    col[2] = galois_multiplication(a[2],14) ^ galois_multiplication(a[1],9) ^ galois_multiplication(a[0],13) ^ galois_multiplication(a[3],11);
    col[3] = galois_multiplication(a[3],14) ^ galois_multiplication(a[2],9) ^ galois_multiplication(a[1],13) ^ galois_multiplication(a[0],11);
}

/**
 * This function realizes the preparation of the inverse mix columns calculations
 * under the Galois Field (GF). It takes the columns separately and 
 * it passes as argument to the inverse_galois_mix_columns where they will be the 
 * matrices multiplication using GF addition and multiplication.
 */
void inverse_mix_columns(unsigned char * state) {

    for(int i = 0; i < BLOCKSIZE; i += 4) {
        unsigned char col[BLOCKSIZE/4];
        int mod = 4;
        for(int j = 0; j < BLOCKSIZE/4; j++) {
            col[j] = state[j+i];
        }
        inverse_galois_mix_columns(col);
        for(int j = 0; j < BLOCKSIZE/4; j++) {
            state[j+i] = col[j];
        }

    }
}

/**
 * This function will do the XOR between state and
 * key. The inverse operation of XOR is XOR itself. 
 */
void inverse_add_round_key(unsigned char * state, unsigned char * key) {
    for (int i = 0; i < BLOCKSIZE; i++) {
		state[i] ^= key[i];
	}
}

/**
 * According said on AES_Decrypt function, therefore decrypt initial 
 * round will be the encrypt final round with inversed steps.
 */
void inverse_initial_round(unsigned char * state, unsigned char * key) {
    inverse_add_round_key(state, key);
    inverse_shift_rows(state);
    inverse_sub_bytes(state, BLOCKSIZE);
}

/**
 * The main round continues as main round,
 * just differentiated by having the inverted steps 
 */
void inverse_main_round(unsigned char * state, unsigned char * key, int i) {
    inverse_add_round_key(state, key);
    inverse_mix_columns(state);
    inverse_shift_rows(state);
    inverse_sub_bytes(state, BLOCKSIZE);
}

/**
 * As said on AES_Decrypt function, the decrypt final round 
 * will be the encrypt initial round.
 */
void inverse_final_round(unsigned char * state, unsigned char * key) {
    inverse_add_round_key(state, key);

}

/**
 * The decryption is basically realize the inverse steps of
 * encryption. So the functions will operate the inverse mathematical
 * operations.
 * 
 * Please, primarily check the encrypt.cpp file for better
 * understanding 
 */
void AES_Decrypt(unsigned char * message, unsigned char * key, int rounds) {

    unsigned char expanded_key[(rounds+1)*BLOCKSIZE];

    expand_key(key, expanded_key, (rounds+1)*BLOCKSIZE);
    
    inverse_initial_round(message, &expanded_key[((rounds+1)*BLOCKSIZE)-16]);
    for(int i = rounds-1; i > 0; i--) {
        inverse_main_round(message, &expanded_key[16 *(i)], i);
    }
    inverse_final_round(message, expanded_key);
    
    // cout << endl << endl << "DECRYPTED MESSAGE" << endl;
    // print_array(message);
}