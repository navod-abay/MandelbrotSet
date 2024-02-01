#include <math.h>
#include<fstream>
#include "generator.h"
using namespace std;

Complex::Complex(double x, double y)
{
    real = x;
    imag = y;
}

Complex::Complex() : real(0), imag(0)
{
}

Complex Complex::operator*(Complex const &other) const
{
    double x = real * other.real - imag * other.imag;
    double y = real * other.imag + imag * other.real;
    return Complex{x, y};
}

Complex Complex::operator+(Complex const &other) const
{
    return Complex{real + other.real, imag + other.imag};
}

Complex Complex::operator*=(Complex const &other) const
{
    return *this * other;
}

Complex Complex::operator+=(Complex const &other) const
{
    return *this + other;
}

long mod_square(Complex const & num)
{
    return pow(num.real, 2) + pow(num.imag, 2);
}



double Complex::Real() const {
    return real;
}

double Complex::Imaginary() const {
    return imag;
}






Generator::Generator(int num_rec)
{
    this->num_rec = num_rec;
    tot_size = INIT_STEP_SIZE * pow(2, num_rec);
    plane = new Complex* [tot_size];
    inclusion_set = new bool *[tot_size];
    double x_s = START;
    double min_step = ((double)(END - START)) / tot_size;
    cout << min_step << endl;
    for (int i = 0; i < tot_size; i++)
    {
        double y_s = START;
        plane[i] = new Complex[tot_size];
        inclusion_set[i] = new bool[tot_size];
        for (int j = 0; j < tot_size; j++)
        {   
            // cout << x_s << " " << y_s << endl;
            plane[i][j] = Complex(x_s, y_s);
            inclusion_set[i][j] = false;
            y_s += min_step;
        }
        x_s += min_step;
    }
}

Generator::~Generator()
{
     for (int i = 0; i < tot_size; i++)
    {
        delete[] plane[i];
        delete[] inclusion_set[i];
    }
    delete[] plane;
    delete[] inclusion_set;
}


bool Generator::check_inclusion(Complex const &com_num, float prob)
{
    Complex zn = com_num;
    int iter_depth = (int) ITER_LIMIT * ((PROB_BIAS - prob) * (PROB_BIAS - prob));
    for (int i = 0; i < iter_depth; i++)
    {
        zn *= zn;
        zn += com_num;
        if(mod_square(zn) > 1)
            return false;
    }
    return true;
}

void Generator::convolution() {

}

void Generator::run() {
    int block_size = 2 ^ num_rec;

}

void Generator::run_first_iter(int block_size) {
    for(int i= 0; i < tot_size; i+= block_size) {
        for(int j = 0; j < tot_size; j += block_size) {
    cout << "Checking inclusion of " << plane[i][j].Real() << " + " << plane[i][j].Imaginary() << "i" << endl;

            inclusion_set[i][j] = check_inclusion(plane[i][j], 0.7);
        }
    }
    cout << "First iteration done" << endl;
}

void Generator::create_ouput_file() {
    ofstream f;
    f.open("output.bmp", ios::out | ios::binary);
    if(f) {
        cout << "The file created successfully" << endl;
    }
    f << 'B' << 'M';
    int num_bytes = tot_size / 8;
    unsigned int filesize = 62 + (tot_size * num_bytes);
    cout << "tot_size" << tot_size << " num bytes: " << num_bytes << "\tfilesize: " << filesize << endl;
    unsigned int Header[] = {filesize, 0, 62};
    f.write((char *)Header, 3 * sizeof(int));
    unsigned int Infoheader[12] = {40, tot_size, tot_size, 0x00010001, 0, 0, 0, 0, 0, 0, 0x00ffffff, 0x00000000};
    f.write((char *)Infoheader, 12*sizeof(int));
    unsigned char bit_buffer;
    for(int j = 0; j < tot_size; j++) {
        for(int i = 0; i < tot_size;) {
            for(int k = 0; k < 8; k++, i++) {
                if(inclusion_set[i][j]) {
                    bit_buffer |= (1<<k);
                }
            }
            f << bit_buffer;
            bit_buffer = 0;
        }
    }
    /*
   for(int i = 0; i < tot_size; i++) {
        for(int j = 0; j < tot_size; j++) {
            if(inclusion_set[i][j]) {

            }
        }
   }
   */
    f.close();
}