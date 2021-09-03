#include "aes.h"
#include "file.h"
#include "encrypt.cpp"
#include "decrypt.cpp"
#include "modes.cpp"
#include <iostream>
#include <cstdlib>

using namespace std;


int input_rounds() {
    int rounds;
    cout << "How many rounds? ";
    cin >> rounds;

    return rounds;
}

char select_encrypt_decrypt() {
    char opt;
    system("clear");
    cout << "Do you want to encrypt (e) or decrypt (d)?" << endl;
    cin >> opt;

    return opt;
}

int is_image() {
    int opt;
    system("clear");
    cout << "Is file to be encrypted/decrypted a image?" << endl;
    cin >> opt;

    return opt;

}

void basic_aes() {
    int rounds;

    unsigned char message[KEYSIZE];
    unsigned char key[KEYSIZE];

    // input rounds
    rounds = input_rounds();

    // Read the files
    read_file("secret.key", key);               // open the secret.key to read the key

    char opt = select_encrypt_decrypt();        // show select menu
    if(opt == 'e') {
        read_file("message.in", message);       // open the message.in to read the message 
        AES_Encrypt(message, key, rounds);
        write("basic-aes-128-encrypt.txt", message, KEYSIZE);
    }else if(opt == 'd') {
        read_file("encrypted.in", message);     // open the message.in to read the message 
        AES_Decrypt(message, key, rounds);
        write("basic-aes-128-decrypt.txt", message, KEYSIZE);
    }else {
        cout << "Invalid option!" << endl;
    }

}

void ecb_aes() {    
    int rounds;

    // input rounds
    rounds = input_rounds();

    int image = is_image();

    unsigned char key[KEYSIZE];
    read_file("secret.key", key);

    int file_size = 0;
    int header_size = 0;
    unsigned char * header;
    if(image) {
        string filename;
        cout << "Write the filename with extension (OBS: file must be on assests directory): ";
        cin >> filename;
        file_size = get_file_size("tux.bmp");
        header_size = get_header_size("tux.bmp");
        header = new unsigned char[header_size];
        get_header("tux.bmp", header, header_size);
    }else {
        file_size = get_file_size("message.in");
        header_size = 0;
    }
    int data_size = file_size - header_size;
    unsigned char message[data_size];
    
    if(image) {
        read_bmp("tux.bmp", message);
    }else {
        header = NULL;
        read_file("message.in", message);
    }/**/

    AES_ECB(message, key, rounds, data_size, header, header_size);
}

void ctr_aes() {
    int rounds;

    // input rounds
    rounds = input_rounds();

    int image = is_image();

    unsigned char key[KEYSIZE];
    read_file("secret.key", key);

    int file_size = 0;
    int header_size = 0;
    unsigned char * header;
    if(image) {
        string filename;
        cout << "Write the filename with extension (OBS: file must be on assests directory): ";
        cin >> filename;
        file_size = get_file_size("tux.bmp");
        header_size = get_header_size("tux.bmp");
        header = new unsigned char[header_size];
        get_header("tux.bmp", header, header_size);
    }else {
        file_size = get_file_size("message.in");
        header_size = 0;
    }
    int data_size = file_size - header_size;
    unsigned char message[data_size];
    
    if(image) {
        read_bmp("tux.bmp", message);
    }else {
        header = NULL;
        read_file("message.in", message);
    }

    AES_CTR(message, key, rounds, data_size, header, header_size);

}

int main() {
    cout << "+-----------------+" << endl;
    cout << "|     AES-128     |" << endl;
    cout << "+-----------------+" << endl;

    int opt = 0;

    do {
        cout << "Choose an option:" << endl;
        cout << "0 - Basic Encrypt/Decrypt " << endl;
        cout << "1 - ECB mode" << endl;
        cout << "2 - CTR mode" << endl;        
        cin >> opt;

        switch(opt) {
            case 0:
                basic_aes();
                break;
            case 1:
                ecb_aes();
                break;
            case 2:
                ctr_aes();
                break;
            default: 
                cout << "Invalid option!" << endl;
                break;
        }
    } while(opt > 2 || opt < 0);
    
    return 0;
}