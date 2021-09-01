#ifndef FILE_H_
#define FILE_H_

#include <fstream>
#include <cstring>
#include <iostream>

#define DIR "assets/"

void read_file(std::string filename, unsigned char * char_array) {

    std::ifstream file;
    file.open(DIR + filename);
    std::string str;
    
    // unsigned char message[KEYSIZE];
    while (std::getline(file, str)) {
        strcpy((char*) char_array, str.c_str());
    }

}

#endif