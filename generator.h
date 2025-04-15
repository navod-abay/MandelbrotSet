#include<new>
#include<iostream>
#include<fstream>
#include<atomic>

#ifndef GENERATOR_H
#define GENERATOR_H

const int START = -2;
const int END = 2;
const int ITER_LIMIT  = 500;
const int INIT_STEP_SIZE = 64;
const int INIT_CONVO_BLOCK_SIZE = 3;
const int x_start = -2;
const int x_end = 1;
const int y_start = -1;
const int y_end = 1;

class Complex
{
private:
    double real;
    double imag;
public:
    Complex(double x, double y);
    Complex();
    Complex operator*(Complex const & other) const;
    Complex operator+(Complex const & other) const;
    Complex operator*=(Complex const & other) const;
    Complex & operator+=(Complex const & other);
    friend long mod_square(Complex const & num);
    double Real() const;
    double Imaginary() const;
    friend std::ostream & operator<<(std:: ostream & stream, const Complex & value);
};

class ThreadLimits
{
    friend class Generator;
private:
    int x_start, x_end, y_start, y_end;
public:
    ThreadLimits(int x_start, int x_end, int y_start, int y_end) : x_start(x_start), x_end(x_end), y_start(y_start), y_end(y_end){};\
    ThreadLimits(int c, int d, int threadXsize, int threadYSize, int blockSize){
        x_start = c * threadYSize * blockSize;
        x_end = (c + 1) * threadYSize * blockSize - 1;
        y_start = d * threadXsize * blockSize;
        y_end = (d + 1) * threadXsize * blockSize - 1;
    }
};


class Generator
{
private:
    std::atomic<int> num_skips;
    int num_rec;
    Complex **plane;
    short **inclusion_set;
    unsigned int x_size;
    unsigned int y_size;
    unsigned int num_elem;
    std::ofstream otp;
    std::ofstream convolution_file;
    char * colours[];

    void convolutionIter(int block_size, int cur_rec, ThreadLimits thread_limits);
    short check_inclusion(Complex const & com_num, short prob);
    short convolutionSingleCell(int x, int y, int cur_rec, ThreadLimits thread_limits, int block_size) const;
    void iterationsWithConvolutions(int start, int cur_rec, int blocksize, ThreadLimits thread_limits);
    
public:
void printSet();
    void runIter(int block_size, int start, ThreadLimits thread_limits);
    void runIter(int block_size);
    void create_ouput_file();
    Generator(int num_rec);
    ~Generator();
    void mapColours();
    void run();
};



#endif