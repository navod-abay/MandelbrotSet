#include<new>
#include<iostream>

#ifndef GENERATOR_H
#define GENERATOR_H

const int START = -2;
const int END = 2;
const int ITER_LIMIT  = 10;
const float PROB_BIAS = 1.7;
const int INIT_STEP_SIZE = 64;

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
    int start;
    int num_rec;
    int cur_rec;
    Complex **plane;
    bool **inclusion_set;
    unsigned int tot_size;

    static void convolution();
    static bool check_inclusion(Complex const & com_num, float prob);
    
public:
    void runIter(int block_size, int start);
    void create_ouput_file();
    Generator(int num_rec);
    ~Generator();
    void run();
};

#endif