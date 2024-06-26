#include <math.h>
#include <fstream>
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

Complex &Complex::operator+=(Complex const &other)
{
    imag += other.imag;
    real += other.real;
    return *this;
}

std::ostream &operator<<(std::ostream &stream, const Complex &value)
{
    stream << value.real << " + " << value.imag << "i";
    return stream;
}

long mod_square(Complex const &num)
{
    return pow(num.real, 2) + pow(num.imag, 2);
}

double Complex::Real() const
{
    return real;
}

double Complex::Imaginary() const
{
    return imag;
}

// The number of rows is y_size the number of columns is x_size

Generator::Generator(int num_rec) : num_rec(num_rec)
{
    double min_step;
    // num_skips = 0;
    otp.open("Generator.log", ios::out);
    // convolution_file.open("Convolutionfile.csv", ios::out);

    if (x_end - x_start < y_end - y_start)
    {
        cout << "It's here" << endl;
        x_size = INIT_STEP_SIZE * pow(2, num_rec);
        min_step = ((double)(x_end - x_start)) / x_size;
        y_size = (int)(y_end - y_start) / min_step;
    }
    else
    {
        y_size = INIT_STEP_SIZE * pow(2, num_rec);
        min_step = ((double)(y_end - y_start)) / y_size;
        x_size = (int)(x_end - x_start) / min_step;
    }

    if (y_size % 8)
    {
        y_size += (8 - y_size % 8);
    }

    plane = new Complex *[y_size];
    inclusion_set = new short *[y_size];
    cout << "x_size: " << x_size << endl;
    cout << "y_size: " << y_size << endl;
    double y_s = y_end;
    for (int i = 0; i < y_size; i++)
    {
        double x_s = x_start;
        plane[i] = new Complex[x_size];
        inclusion_set[i] = new short[x_size];
        for (int j = 0; j < x_size; j++)
        {
            // cout << x_s << " " << y_s << endl;
            plane[i][j] = Complex(x_s, y_s);
            inclusion_set[i][j] = false;
            x_s += min_step;
        }
        y_s -= min_step;
    }
}

Generator::~Generator()
{
    cout << num_skips;
    cout << plane[0][0] << endl
         << plane[0][x_size - 1] << endl
         << plane[y_size - 1][0] << endl
         << plane[y_size - 1][x_size -1 ] << endl;
    for (int i = 0; i < y_size; i++)
    {
        delete[] plane[i];
        delete[] inclusion_set[i];
    }
    delete[] plane;
    delete[] inclusion_set;
    otp.close();
    // convolution_file.close();
}

short Generator::check_inclusion(Complex const &com_num, short prob)
{
    if (prob == 128 || prob == -128)
    {
        this->num_skips++;
        return prob;
    }
    Complex zn = com_num;
    int iter_depth = ITER_LIMIT;
    for (int i = 0; i < iter_depth; i++)
    {
        zn = zn * zn;
        zn = com_num + zn;
        if (mod_square(zn) > 4)
        {
            return -128;
        }
    }

    return 128;
}

void Generator::convolutionIter(int block_size)
{
    short prob;
    for (int i = 0; i < y_size; i += block_size)
    {
        for (int j = 0; j < x_size; j += block_size)
        {
            inclusion_set[i][j] = convolutionSingleCell(i, j, block_size);
            // convolution_file << prob << "," ;
        }
        // convolution_file << endl;
    }
    // convolution_file << endl << endl;
}

short Generator::convolutionSingleCell(int x, int y, int block_size) const
{
    short num_included = 0;
    int CONVO_BLOCK_SIZE = INIT_CONVO_BLOCK_SIZE + (num_rec - cur_rec);
    if (x < block_size * CONVO_BLOCK_SIZE || x >= y_size - block_size * CONVO_BLOCK_SIZE)
        return 0;
    if (y < block_size * CONVO_BLOCK_SIZE || y >= x_size - block_size * CONVO_BLOCK_SIZE)
        return 0;
    for (int i = x - block_size * CONVO_BLOCK_SIZE; i < x + block_size * CONVO_BLOCK_SIZE; i += block_size)
    {
        for (int j = y - block_size * CONVO_BLOCK_SIZE; j < y + block_size * CONVO_BLOCK_SIZE; j += block_size)
        {
            if (inclusion_set[i][j] > 0)
                num_included++;
        }
    }
    if (inclusion_set[x][y] > 0)
    {
        if (num_included == CONVO_BLOCK_SIZE * CONVO_BLOCK_SIZE)
        {
            return 128;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        if (num_included == 0)
        {
            return -128;
        }
        else
        {
            return -1;
        }
    }
    return num_included;
}

void Generator::run()
{
    int block_size = pow(2, num_rec);
    otp << "Block Size: " << block_size << ", start: " << 0 << endl;

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
        start /= 2;
        block_size /= 2;
        cur_rec--;
    } while (block_size > 1);
}

void Generator::runIter(const int block_size, const int start)
{
    int k = 0, l = 0;
    for (int i = 0; i < y_size; i += block_size)
    {
        for (int j = start; j < x_size; j += block_size)
        {
            otp << "Checking inclusion of " << i << " + " << j << "i" << endl;
            inclusion_set[i][j] = check_inclusion(plane[i][j], inclusion_set[i][k]);
            k += block_size;
        }
        k = 0;
    }
    k = 0;
    for (int i = start; i < y_size; i += block_size)
    {
        for (int j = 0; j < x_size; j += block_size)
        {
            otp << "Checking inclusion of " << i << " + " << j << "i" << endl;
            inclusion_set[i][j] = check_inclusion(plane[i][j], inclusion_set[k][j]);
        }
        k += block_size;
    }
    k = 0;
    for (int i = start; i < y_size; i += block_size)
    {
        for (int j = start; j < x_size; j += block_size)
        {
            otp << "Checking inclusion of " << i << " + " << j << "i" << endl;
            inclusion_set[i][j] = check_inclusion(plane[i][j], inclusion_set[k][l]);
            l += block_size;
        }
        k += block_size;
        l = start;
    }
}

void Generator::runIter(int block_size)
{
    for (int i = 0; i < y_size; i += block_size)
    {
        for (int j = 0; j < x_size; j += block_size)
        {
            otp << "Checking inclusion of " << i << " + " << j << "i" << endl;
            inclusion_set[i][j] = check_inclusion(plane[i][j], 0.7);
        }
    }
}

void Generator::create_ouput_file()
{
    ofstream f;
    f.open("output.bmp", ios::out | ios::binary);
    if (f)
    {
        cout << "The file created successfully" << endl;
    }
    f << 'B' << 'M';
    int num_bytes = x_size * y_size / 8;
    unsigned int filesize = 54 + x_size * y_size * 3;
    //  cout << "tot_size" << tot_size << " num bytes: " << num_bytes << "\tfilesize: " << filesize << endl;
    unsigned int Header[] = {filesize, 0, 54};
    f.write((char *)Header, 3 * sizeof(int));
    unsigned int Infoheader[10] = {40, x_size, y_size, 0x00180001, 0, 0, 0, 0, 0, 0};
    f.write((char *)Infoheader, 10 * sizeof(int));
    // unsigned char black[3] = {0, 0, 0};
    // unsigned char white[3] = {255, 255, 255};
    for (int j = 0; j < y_size; j++)
    {
        for (int i = 0; i < x_size;)
        {
            for (int k = 0; k < 8; k++, i++)
            {
                if (inclusion_set[j][i] > 0)
                {
                    f.write("ÿÿÿ", 3);
                }
                else
                {
                    f.write("", 3);
                }
            }
        }
    }
    f.close();
}