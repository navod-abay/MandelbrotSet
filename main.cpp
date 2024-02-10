#include<iostream>
#include"generator.h"

int main() {
    Generator g1(3);
    g1.run();
    g1.create_ouput_file();
    return 0;
}
