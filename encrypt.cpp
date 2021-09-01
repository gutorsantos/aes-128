#include <iostream>
#include "aes.h"

using namespace std;

void print_array_key(unsigned char * k, int size) {
    // for (int i = 0; i < KEYSIZE; i++) {
	// 	cout << hex << (int) m[i] << " ";
	// }
    // cout << endl;

    for(int i = 0; i < size; i += 4) {
        for(int j = 0; j < 4; j++) {
            
		    cout << hex << (int) k[(j*4)+(i/4)] << "\t";
        }  

        cout << endl;

    }
}

void print_array(unsigned char * m) {
    // for (int i = 0; i < KEYSIZE; i++) {
	// 	cout << hex << (int) m[i] << " ";
	// }
    // cout << endl;

    for(int i = 0; i < KEYSIZE; i += 4) {
        unsigned char row[KEYSIZE/4];
        int mod = 4;
        for(int j = 0; j < KEYSIZE/4; j++) {
            row[j] = m[(j*4)+(i/4)];
            
		    cout << hex << (int) row[j] << "\t";
        }  

        cout << endl;

    }
}

void sub_bytes(unsigned char * state, int len) {
    for (int i = 0; i < len; i++) {
		state[i] = s_box[state[i]];
	}

}

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
        h = (r[c] >> 7) & 1; /* arithmetic right shift, thus shifting in either zeros or ones */
        b[c] = r[c] << 1; /* implicitly removes high bit because b[c] is an 8-bit char, so we xor by 0x1b and not 0x11b in the next line */
        b[c] ^= h * 0x1B; /* Rijndael's Galois field */
    }
    r[0] = b[0] ^ a[3] ^ a[2] ^ b[1] ^ a[1]; /* 2 * a0 + a3 + a2 + 3 * a1 */
    r[1] = b[1] ^ a[0] ^ a[3] ^ b[2] ^ a[2]; /* 2 * a1 + a0 + a3 + 3 * a2 */
    r[2] = b[2] ^ a[1] ^ a[0] ^ b[3] ^ a[3]; /* 2 * a2 + a1 + a0 + 3 * a3 */
    r[3] = b[3] ^ a[2] ^ a[1] ^ b[0] ^ a[0]; /* 2 * a3 + a2 + a1 + 3 * a0 */

    // cout << "MIXED COLUMNS" << endl;
    // cout << hex << (int) r[0] << endl << (int) r[1] << endl << (int) r[2] << endl << (int) r[3] << endl << endl;
}

void mix_columns(unsigned char * state) {

    for(int i = 0; i < KEYSIZE; i += 4) { // Starts on 4 because there's no need to shift the first row
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

void add_round_key(unsigned char * state, unsigned char * roundKey) {
    for (int i = 0; i < KEYSIZE; i++) {
		state[i] ^= roundKey[i];
	}
}

void initial_round(unsigned char * state, unsigned char * key) {
    add_round_key(state, key);
}

void main_round(unsigned char * state, unsigned char * key, int i) {
    sub_bytes(state, KEYSIZE);
    shift_rows(state);
    mix_columns(state);
    add_round_key(state, key);

}

void final_round(unsigned char * state, unsigned char * key) {
    sub_bytes(state, KEYSIZE);
    shift_rows(state);
    add_round_key(state, key);
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
            sub_bytes(w_i_1, KEYSIZE/4);
            
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

void AES_Encrypt(unsigned char * message, unsigned char * key, int rounds) {

    unsigned char expanded_key[(rounds+1)*KEYSIZE];
    
    unsigned char message2[16] = {
        0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d, 0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34,
    };

    unsigned char key2[16] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
    };

    expand_key(key2, expanded_key, (rounds+1)*KEYSIZE);
    
    initial_round(message2, expanded_key);
    for(int i = 0; i < rounds-1; i++) {
        main_round(message2, &expanded_key[16 *(i+1)], i);
    }
    final_round(message2, &expanded_key[((rounds+1)*KEYSIZE)-16]);
    
    cout << endl << endl << "FINAL MESSAGE" << endl;
    cout << message2[0] << message2[1] << message2[2] << message2[3] << message2[4] << message2[5] << message2[6] << message2[7] << message2[8] << message2[9] << message2[10] << message2[11] << message2[12] << message2[13] << message2[14] << message2[15] << endl;
    print_array(message2);
}