#include "aes.h"
#include "file.h"
#include "encrypt.cpp"
#include <iostream>

using namespace std;

int input_rounds() {
    int rounds;
    cout << "How many rounds? ";
    cin >> rounds;

    return rounds;
}

void normal_aes() {
    int rounds;

    // input rounds
    rounds = input_rounds();

    // open the message.key to read the message
    unsigned char message[KEYSIZE];
    read_file("message.in", message);

    // open the secret.key to read the key
    unsigned char key[KEYSIZE];
    read_file("secret.key", key);

    AES_Encrypt(message, key, rounds);

}

void ecb_aes() {

}

void ctr_aes() {

}

int main() {
    cout << "+-----------------+" << endl;
    cout << "|     AES-128     |" << endl;
    cout << "+-----------------+" << endl;

    int opt = 0;

    do {
        cout << "Choose an option:" << endl;
        cout << "0 - Encrypt/Decrypt " << endl;
        cout << "1 - ECB mode" << endl;
        cout << "2 - CTR mode" << endl;        
        cin >> opt;

        switch(opt) {
            case 0:
                normal_aes();
                break;
            
            case 1:
                break;

            case 2:
                break;
            default: 
                cout << "Invalid option!" << endl;
                break;
        }
    } while(opt > 2 || opt < 0);
    
    return 0;
}