#include<new>
#include<iostream>

const int START = -2;
const int END = 2;

class Complex
{
private:
    long real;
    long imag;
public:
    Complex(long real, long imag);
    Complex();
    Complex operator*(Complex const & other) const;
    Complex operator+(Complex const & other) const;
};


class Generator
{
private:

    class Pallete
    {
    private:
        Complex **plane;
        bool **include;
        int size;

    public:
        Pallete(long step);
        ~Pallete();
    };
    

    
public:
    Generator();
    ~Generator();
};

Generator::Generator()
{
}

Generator::~Generator()
{
}
