#include<iostream>
#include"generator.h"
#include<chrono>

using namespace std;

int main() {
    auto start = chrono::high_resolution_clock::now();
    Generator g1(6);
    g1.run();
    g1.create_ouput_file();
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(stop - start);
    cout <<"Time taken: " << duration.count() << endl;
    return 0;
}
