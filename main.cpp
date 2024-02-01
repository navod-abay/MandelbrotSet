#include<iostream>
#include"generator.h"

int main() {
    Generator g1(0);
    g1.run_first_iter(1);
    g1.create_ouput_file();
    return 0;
}
