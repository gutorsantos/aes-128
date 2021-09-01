
#include <iostream>

int main() {
    unsigned char a = 'm';
    unsigned char b = 'a';
    unsigned char t = (a & 0x0F);
    unsigned char tk = (a & 0xF0) >> 4;
    std::cout << std::hex << (int) t << std::endl;
    std::cout << std::hex << (int) tk << std::endl;
    return 0;
}