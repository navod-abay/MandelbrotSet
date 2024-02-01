#include"generator.h"
using namespace std;

Complex:: Complex(long x, long y)
{
    real = x;
    imag = y;
}

Complex::Complex(): real(0), imag(0)
{
}

Complex Complex::operator*(Complex const & other) const
{
    long x = real * other.real - imag * other.imag;
    long y = real * other.imag + imag * other.real;
    return Complex{x, y};
}

Complex Complex::operator+(Complex const & other) const
{
    return Complex{real + other.real, imag + other.imag};
}




Generator::Pallete::Pallete(long step)
{
    size = ((START - END) / step ) + 1;
    plane = new Complex*[size];
    include = new bool*[size];
    long x_s = START;
    long y_s = START;
    for(int i = 0; i < size; i++) {
        plane[i] = new Complex[size];
        include[i] = new bool[size];
        for(int j = 0; j < size; j++) {
            plane[i][j] = Complex(x_s, y_s);
            include[i][j] = false;
            y_s += step;
        }
        x_s += step;
    }
}

Generator::Pallete::~Pallete()
{
    for(int i = 0; i < size; i++) {
        delete [] plane[i];
        delete [] include[i];
    }
    delete [] plane;
    delete [] include;
}