#include<new>
#include<iostream>
#include<fstream>

#ifndef GENERATOR_H
#define GENERATOR_H

const int START = -2;
const int END = 2;
const int ITER_LIMIT  = 500;
const int INIT_STEP_SIZE = 64;
const int INIT_CONVO_BLOCK_SIZE = 4;

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


class Generator
{
private:
    int num_skips;
    int start;
    int num_rec;
    int cur_rec;
    Complex **plane;
    short **inclusion_set;
    unsigned int tot_size;
    float ** probablities;
    std::ofstream otp;
    std::ofstream convolution_file;

    void convolutionIter(int block_size);
    short check_inclusion(Complex const & com_num, short prob);
    short convolutionSingleCell(int x, int y, int block_size) const;
    
public:
    void runIter(int block_size, int start);
    void runIter(int block_size);
    void create_ouput_file();
    Generator(int num_rec);
    ~Generator();
    void run();
};

#endif