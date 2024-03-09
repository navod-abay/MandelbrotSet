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

Complex & Complex::operator+=(Complex const &other)
{   
    imag += other.imag;
    real += other.real;
    return *this;
}

std::ostream & operator<<(std::ostream & stream, const Complex & value) {
    stream << value.real << " + "<< value.imag << "i";
    return stream; 
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
    otp.open("Generator.log", ios::out);
    this->num_rec = num_rec;
    tot_size = INIT_STEP_SIZE * pow(2, num_rec);
    plane = new Complex* [tot_size];
    inclusion_set = new bool *[tot_size];
    double x_s = START;
    double min_step = ((double)(END - START)) / tot_size;
    cout << "min_step: " << min_step << endl;
    cout << "Tot Size: " << tot_size << endl;
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
        zn = zn * zn;
        zn = com_num + zn;  
        if(mod_square(zn) > 4){
            return false;
        }
    }

    return true;
}

void Generator::convolution() {

}

void Generator::run() {
    int block_size = pow(2, num_rec);
    otp << "Block Size: " << block_size << ", start: "  << 0 << endl;

    runIter(block_size);
    cur_rec = num_rec - 1;
    int start = pow(2, cur_rec);
    do
    {
        otp << "Block Size: " << block_size << ", start: " << start << endl;
        // cin.get();
        runIter(block_size, start);
        start  /= 2;    
        block_size /= 2;
        cur_rec--;
    } while (block_size > 1);
    

}

void Generator::runIter(int block_size, int start) {
    for(int i= 0; i < tot_size; i+= block_size) {
        for(int j = start; j < tot_size; j += block_size) {
            otp << "Checking inclusion of " << i << " + " << j << "i" << endl;
            inclusion_set[i][j] = check_inclusion(plane[i][j], 0.7);
        }
    }
    for(int i= start; i < tot_size; i+= block_size) {
        for(int j = 0; j < tot_size; j += block_size ) {
            otp << "Checking inclusion of " << i << " + " << j << "i" << endl;
            inclusion_set[i][j] = check_inclusion(plane[i][j], 0.7);
        }
    }
    for(int i= start; i < tot_size; i+= block_size) {
        for(int j = start; j < tot_size; j += block_size ) {
            otp << "Checking inclusion of " << i << " + " << j << "i" << endl;
            inclusion_set[i][j] = check_inclusion(plane[i][j], 0.7);
        }    
    }
}

void Generator::runIter(int block_size) {
    for(int i= 0; i < tot_size; i+= block_size) {
        for(int j = 0; j < tot_size; j += block_size) {
            otp << "Checking inclusion of " << i << " + " << j << "i" << endl;
            inclusion_set[i][j] = check_inclusion(plane[i][j], 0.7);
        }
    }
}



void Generator::create_ouput_file() {
    ofstream f;
    f.open("output.bmp", ios::out | ios::binary);
    if(f) {
        cout << "The file created successfully" << endl;
    }
    f << 'B' << 'M';
    int num_bytes = tot_size / 8;
    unsigned int filesize = 54 + tot_size * tot_size * 3;
    cout << "tot_size" << tot_size << " num bytes: " << num_bytes << "\tfilesize: " << filesize << endl;
    unsigned int Header[] = {filesize, 0, 54};
    f.write((char *)Header, 3 * sizeof(int));
    unsigned int Infoheader[10] = {40, tot_size, tot_size, 0x00180001, 0, 0, 0, 0, 0, 0};
    f.write((char *)Infoheader, 10*sizeof(int));
    //unsigned char black[3] = {0, 0, 0};
    //unsigned char white[3] = {255, 255, 255};
    for(int j = 0; j < tot_size; j++){
        for(int i = 0; i < tot_size;) {
            for(int k = 0; k < 8; k++, i++) {
                if(inclusion_set[i][j]) {
                    f.write("~~~", 3);
                } else {
                    f.write("000", 3);
                }
            }
        }
    }
    f.close();
}