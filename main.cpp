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
    system("clear");
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
    char opt = 'y';
    system("clear");
    cout << "Is file to be encrypted/decrypted a image? (y/n) ";
    cin >> opt;

    return tolower(opt);

}

void basic_aes() {
    int rounds;

    unsigned char message[BLOCKSIZE];
    unsigned char key[KEYSIZE];

    // input rounds
    rounds = input_rounds();

    // Read the files
    read_file("secret.key", key);               // open the secret.key to read the key

    char opt = select_encrypt_decrypt();        // show select menu
    if(opt == 'e') {
        read_file("message.in", message);       // open the message.in to read the message 
        AES_Encrypt(message, key, rounds);
        write("basic-aes-128-encrypt.txt", message, BLOCKSIZE);
    }else if(opt == 'd') {
        read_file("encrypted.in", message);     // open the message.in to read the message 
        AES_Decrypt(message, key, rounds);
        write("basic-aes-128-decrypt.txt", message, BLOCKSIZE);
    }else {
        cout << "Invalid option!" << endl;
    }

}

void ecb_aes() {    
    int rounds;

    // input rounds
    rounds = input_rounds();

    char image = is_image();

    unsigned char key[KEYSIZE];
    read_file("secret.key", key);

    long long int file_size = 0;
    int header_size = 0;
    unsigned char * header;
    string filename;
    if(image == 'y') {
        cout << "Write the filename with extension (OBS: file must be on assests directory): ";
        cin >> filename;
        file_size = get_file_size(filename);
        header_size = get_header_size(filename);
        header = new unsigned char[header_size];
        get_header(filename, header, header_size);
    }else {
        file_size = get_file_size("message.in");
        header_size = 0;
    }
    int data_size = file_size - header_size;
    unsigned char * message = (unsigned char *) malloc(file_size);
    
    if(image == 'y') {
        read_bmp(filename, message);
    }else {
        header = NULL;
        read_file("message.in", message);
    }/**/
    char e_or_d = select_encrypt_decrypt();
    AES_ECB(message, key, rounds, file_size, header, header_size, e_or_d, filename);
    free(message);
}

void ctr_aes() {
    int rounds;

    // input rounds
    rounds = input_rounds();

    char image = is_image();

    unsigned char key[KEYSIZE];
    read_file("secret.key", key);

    int file_size = 0;
    int header_size = 0;
    unsigned char * header;
    string filename;
    if(image == 'y') {
        cout << "Write the filename with extension (OBS: file must be on assests directory): ";
        cin >> filename;
        file_size = get_file_size(filename);
        header_size = get_header_size(filename);
        header = new unsigned char[header_size];
        get_header(filename, header, header_size);
    }else {
        file_size = get_file_size("message.in");
        header_size = 0;
    }
    int data_size = file_size - header_size;
    unsigned char * message = (unsigned char *) malloc(file_size);
    
    if(image == 'y') {
        read_bmp(filename, message);
    }else {
        header = NULL;
        read_file("message.in", message);
    }

    char e_or_d = select_encrypt_decrypt();
    AES_CTR(message, key, rounds, file_size, header, header_size, e_or_d, filename);
    free(message);

}

int main() {
    cout << "+-----------------+" << endl;
    cout << "|     AES-128     |" << endl;
    cout << "+-----------------+" << endl;

    int opt = 0;

    do {
        cout << "Choose an option:" << endl;
        // cout << "0 - Basic Encrypt/Decrypt " << endl;
        cout << "1 - ECB mode" << endl;
        cout << "2 - CTR mode" << endl;        
        cin >> opt;

        switch(opt) {
            case 0:
                return 0;
                // basic_aes();
                // break;
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