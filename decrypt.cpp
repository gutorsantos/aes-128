#include <iostream>
#include <cstring>
#include "aes.h"

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
    for(int i = 0; i < KEYSIZE; i += 4) {
        unsigned char row[KEYSIZE/4];
        int mod = 4;
        for(int j = 0; j < KEYSIZE/4; j++) {
            row[j] = state[(j*4)+(i/4)];        // Get the row

        }
        for(int j = 0; j < KEYSIZE/4; j++) {
            int shift_index = (( ((j*4)+(i/4)+(i)) % KEYSIZE) + KEYSIZE) % KEYSIZE;
            state[shift_index] = row[j];
        }

    }

}

void gmix_column(unsigned char *r) {
    // cout << hex << (int) r[0] << endl << (int) r[1] << endl << (int) r[2] << endl << (int) r[3] << endl << endl;
    unsigned char a[4];
    unsigned char b[4];
    unsigned char c;
    unsigned char h;
    /* The array 'a' is simply a copy of the input array 'r'
     * The array 'b' is each element of the array 'a' multiplied by 2
     * in Rijndael's Galois field
     * a[n] ^ b[n] is element n multiplied by 3 in Rijndael's Galois field */ 
    for (c = 0; c < 4; c++) {
        a[c] = r[c];
        /* h is 0xff if the high bit of r[c] is set, 0 otherwise */
        h = (r[c] << 7) & 1; /* arithmetic right shift, thus shifting in either zeros or ones */
        b[c] = r[c] >> 1; /* implicitly removes high bit because b[c] is an 8-bit char, so we xor by 0x1b and not 0x11b in the next line */
        b[c] ^= h / 0x1B; /* Rijndael's Galois field */
    }
    r[0] = b[0] ^ a[3] ^ a[2] ^ b[1] ^ a[1]; /* 2 * a0 + a3 + a2 + 3 * a1 */
    r[1] = b[1] ^ a[0] ^ a[3] ^ b[2] ^ a[2]; /* 2 * a1 + a0 + a3 + 3 * a2 */
    r[2] = b[2] ^ a[1] ^ a[0] ^ b[3] ^ a[3]; /* 2 * a2 + a1 + a0 + 3 * a3 */
    r[3] = b[3] ^ a[2] ^ a[1] ^ b[0] ^ a[0]; /* 2 * a3 + a2 + a1 + 3 * a0 */

    // cout << "MIXED COLUMNS" << endl;
    // cout << hex << (int) r[0] << endl << (int) r[1] << endl << (int) r[2] << endl << (int) r[3] << endl << endl;
}

void inverse_mix_columns(unsigned char * state) {

    for(int i = 0; i < KEYSIZE; i += 4) {
        unsigned char row[KEYSIZE/4];
        int mod = 4;
        for(int j = 0; j < KEYSIZE/4; j++) {
            row[j] = state[j+i];
        }
        gmix_column(row);
        for(int j = 0; j < KEYSIZE/4; j++) {
            state[j+i] = row[j];
        }

    }

}

/**
 * This function will do the XOR between state and
 * key. The inverse operation of XOR is XOR itself. 
 */
void inverse_add_round_key(unsigned char * state, unsigned char * key) {
    for (int i = 0; i < KEYSIZE; i++) {
		state[i] ^= key[i];
	}
}

/**
 * According said on AES_Decrypt function, therefore decrypt initial 
 * round will be the encrypt final round with inversed steps.
 */
void initial_round(unsigned char * state, unsigned char * key) {
    inverse_add_round_key(state, key);
    inverse_shift_rows(state);
    inverse_sub_bytes(state, KEYSIZE);
}

/**
 * The main round continues as main round,
 * just differentiated by having the inverted steps 
 */
void main_round(unsigned char * state, unsigned char * key, int i) {
    inverse_add_round_key(state, key);
    inverse_mix_columns(state);
    inverse_shift_rows(state);
    inverse_sub_bytes(state, KEYSIZE);
}

/**
 * As said on AES_Decrypt function, the decrypt final round 
 * will be the encrypt initial round.
 */
void final_round(unsigned char * state, unsigned char * key) {
    inverse_add_round_key(state, key);

}

/** rot_word
 * This function is responsible to vertically shift the
 * column
 *          example: ab      cd
 *                   cd  =>  ef
 *                   ef      01
 *                   01      ab
 */
void rot_word(unsigned char * w) {
    unsigned char tmp[KEYSIZE/4];
    memcpy(tmp, w, sizeof(w));

    for(int i = 0; i < KEYSIZE/4; i++) {
        w[((i-1) % (KEYSIZE/4) + (KEYSIZE/4))%(KEYSIZE/4)] = tmp[i];
    }
}

void expand_key(unsigned char * key, unsigned char * expanded_key, int size) {
	for(int i = 0; i < KEYSIZE; i++) {
		expanded_key[i] = key[i];
	}

	int bytes_written = KEYSIZE;
	int rcon_count = 1;
    unsigned char w_i_1[KEYSIZE/4];

	while(bytes_written < size) {

        for(int i = 0; i < KEYSIZE/4; i++) {
            w_i_1[i] = expanded_key[i + bytes_written - 4];
        }

		if(bytes_written % KEYSIZE == 0) {
			rot_word(w_i_1);
            inverse_sub_bytes(w_i_1, KEYSIZE/4);
            
            /** 
             * Just the first byte of rcon will do effect on XOR,
             * so we opperates XOR only between the first column element and
             * these first byte of rcon. 
             *                              example: 01 00 00 00 
             *                                           XOR 
             *                                       9f a8 51 3c
             *                                       9e a8 51 3c
             */
            w_i_1[0] ^= rcon[rcon_count];
            for(int i = 0; i < 4; i++) {
            }

            rcon_count++;
		}

        for (unsigned char c = 0; c < 4; c++) {
			expanded_key[bytes_written] = expanded_key[bytes_written - 16] ^ w_i_1[c];
			bytes_written++;
		}
	}
}

/**
 * The decryption is basically realize the inverse steps of
 * encryption. So the functions will operate the inverse mathematical
 * operations.
 */
void AES_Decrypt(unsigned char * message, unsigned char * key, int rounds) {

    unsigned char expanded_key[(rounds+1)*KEYSIZE];

    expand_key(key, expanded_key, (rounds+1)*KEYSIZE);
    
    initial_round(message, &expanded_key[((rounds+1)*KEYSIZE)-16]);
    for(int i = rounds-1; i > 0; i++) {
        main_round(message, &expanded_key[16 *(i+1)], i);
    }
    final_round(message, expanded_key);
    
    cout << endl << endl << "DECRYPTED MESSAGE" << endl;
    // print_array(message);
}