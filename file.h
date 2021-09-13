#ifndef FILE_H_
#define FILE_H_

#include <fstream>
#include <cstring>
#include <iostream>
#include <filesystem>
#include <string>

#define DIR "assets/"

int get_file_size(std::string filename) {
    int size = std::filesystem::file_size(DIR + filename);
    return size;
}

int get_header_size(std::string filename) {
    std::ifstream file;
    file.open(DIR + filename, std::ios::binary);

    unsigned char c;
    
    int i = 0;
    int start = 0;
    while (file.read((char*) &c, sizeof(char))) {
        if(i == 10) {
            start = (int) c;
        }
        i++;
    }
    return start;
}

void get_header(std::string filename, unsigned char * header, int size) {
    std::ifstream file;
    file.open(DIR + filename, std::ios::binary);

    unsigned char c;

    int i = 0;
    while (i < size) {
        file.read((char*) &c, sizeof(char));
        header[i] = c;
        i++;
    }
}

void read_file(std::string filename, unsigned char * char_array) {

    std::ifstream file;
    file.open(DIR + filename);
    std::string str;
    
    while (std::getline(file, str)) {
        strcpy((char*) char_array, str.c_str());
    }

}

void read_bmp(std::string filename, unsigned char * char_array) {
    std::ifstream file;
    file.open(DIR + filename, std::ios::binary);

    unsigned char c;
    
    int i = 0;
    int start = 0;
    while (file.read((char*) &c, sizeof(char))) {
        char_array[i] = c;
        // if(i == 10) {
        //     start = (int) c;
        // }
        // if(i >= start && i > 10) {
        //     char_array[i-start] = c;
        // }
        i++;
    }

}

bool file_exists(std::string filename) {
    FILE *file;
    if (file = fopen(filename.c_str(), "r")) {
        fclose(file);
        return 1;
    } else {
        return 0;
    }
}

void write(std::string filename, unsigned char * char_array, int size) {
    if(file_exists(DIR+filename)) {
        remove((DIR+filename).c_str());
    }
    std::ofstream file;
    file.open(DIR + filename);
    std::string str;

    for(int i = 0; i < size; i++) {
        str.push_back(char_array[i]);
    }

    file << str;
    file.close();
}


void write_bmp(std::string filename, unsigned char * char_array, int size, unsigned char * header, int header_size) {

    std::ofstream file;
    file.open(DIR + filename, std::ios::app);
    std::string str;
    
    for(int i = 0; i < header_size; i++) {
        str.push_back(header[i]);
    }
    for(int i = header_size; i < size; i++) {
        str.push_back(char_array[i]);
    }
    file << str;
    file.close();
}

#endif