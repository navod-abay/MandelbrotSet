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
    num_skips = 0;
    otp.open("Generator.log", ios::out);
    convolution_file.open("Convolutionfile.csv", ios::out);
    this->num_rec = num_rec;
    tot_size = INIT_STEP_SIZE * pow(2, num_rec);
    plane = new Complex* [tot_size];
    inclusion_set = new short *[tot_size];
    double x_s = START;
    double min_step = ((double)(END - START)) / tot_size;
    cout << "min_step: " << min_step << endl;
    cout << "Tot Size: " << tot_size << endl;
    for (int i = 0; i < tot_size; i++)
    {
        double y_s = START;
        plane[i] = new Complex[tot_size];
        inclusion_set[i] = new short[tot_size];
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
    cout << num_skips;
     for (int i = 0; i < tot_size; i++)
    {
        delete[] plane[i];
        delete[] inclusion_set[i];
    }
    delete[] plane;
    delete[] inclusion_set;
    otp.close();
    convolution_file.close();
}


short Generator::check_inclusion(Complex const &com_num, short prob)
{
    if(prob == 128 || prob == -128) {
        this->num_skips++;
        return prob;
    }
    Complex zn = com_num;
    int iter_depth = ITER_LIMIT;
    for (int i = 0; i < iter_depth; i++)
    {
        zn = zn * zn;
        zn = com_num + zn;  
        if(mod_square(zn) > 4){
            return -128;
        }
    }

    return 128;
}

void Generator::convolutionIter(int block_size){
    short prob;
    for(int i = 0; i < tot_size; i += block_size) {
            for(int j = 0; j < tot_size; j += block_size) {
                inclusion_set[i][j] = convolutionSingleCell(i, j, block_size);
                convolution_file << prob << "," ;
        }
            convolution_file << endl;
        }
        convolution_file << endl << endl;
}


short Generator::convolutionSingleCell(int x, int y, int block_size) const {
    short num_included = 0;
    int CONVO_BLOCK_SIZE = INIT_CONVO_BLOCK_SIZE + (num_rec - cur_rec);
    if(x < block_size * CONVO_BLOCK_SIZE || x >= tot_size - block_size * CONVO_BLOCK_SIZE )
        return 0;
    if(y < block_size * CONVO_BLOCK_SIZE || y >= tot_size - block_size * CONVO_BLOCK_SIZE )
        return 0;
    for (int i = x - block_size * CONVO_BLOCK_SIZE; i < x + block_size * CONVO_BLOCK_SIZE; i += block_size) {
        for (int j = y - block_size * CONVO_BLOCK_SIZE; j < y + block_size * CONVO_BLOCK_SIZE; j += block_size) {
            if(inclusion_set[i][j] > 0) 
                num_included++;
        }
    }
    if(inclusion_set[x][y] > 0) {
        if(num_included == CONVO_BLOCK_SIZE * CONVO_BLOCK_SIZE) {
            return 128;
        } else {
            return 1;
        }
    } else {
        if(num_included == 0) {
            return -128;
        } else {
            return -1;
        }
    }
    return num_included;
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
        cout << "cur_rec: " << cur_rec << "\tnum_rec: " << num_rec << endl; 
        convolutionIter(block_size);
        // cin.get();
        runIter(block_size, start);
        start  /= 2;    
        block_size /= 2;
        cur_rec--;
    } while (block_size > 1);
    

}

void Generator::runIter(const int block_size,const  int start) {
    int k = 0, l = 0;
    for(int i= 0; i < tot_size; i+= block_size) {
        for(int j = start; j < tot_size; j += block_size) {
            otp << "Checking inclusion of " << i << " + " << j << "i" << endl;
            inclusion_set[i][j] = check_inclusion(plane[i][j], inclusion_set[i][k]);
            k += block_size;
        }
        k = 0;
    }
    k = 0;
    for(int i= start; i < tot_size; i+= block_size) {
        for(int j = 0; j < tot_size; j += block_size ) {
            otp << "Checking inclusion of " << i << " + " << j << "i" << endl;
            inclusion_set[i][j] = check_inclusion(plane[i][j], inclusion_set[k][j]);
        }
        k += block_size;
    }
    k = 0;
    for(int i= start; i < tot_size; i+= block_size) {
        for(int j = start; j < tot_size; j += block_size ) {
            otp << "Checking inclusion of " << i << " + " << j << "i" << endl;
            inclusion_set[i][j] = check_inclusion(plane[i][j], inclusion_set[k][l]);
            l += block_size;
        }    
        k += block_size;
        l = start;
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
                if(inclusion_set[i][j] > 0) {
                    f.write("ÿÿÿ", 3);
                } else {
                    f.write("", 3);
                }
            }
        }
    }
    f.close();
}