#include <math.h>
#include <fstream>
#include <thread>
#include <vector>
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
    convolution_file.open("Convolutionfile.csv", ios::out);
    num_skips = 0;
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
         << plane[y_size - 1][x_size - 1] << endl;
    for (int i = 0; i < y_size; i++)
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
    if (prob == 128 || prob < -256)
    {
        this->num_skips++;
        return prob;
    }
    Complex zn = com_num;
    int iter_depth = ITER_LIMIT;
    for (short i = 0; i < iter_depth; i++)
    {
        zn = zn * zn;
        zn = com_num + zn;
        if (mod_square(zn) > 4)
        {
            return -(i + 1);
        }
    }

    return 128;
}

void Generator::convolutionIter(int block_size, int cur_rec, ThreadLimits thread_limits)
{
    for (int i = thread_limits.x_start; i < thread_limits.x_end; i += block_size)
    {
        for (int j = thread_limits.y_start; j < thread_limits.y_end; j += block_size)
        {
            inclusion_set[i][j] = convolutionSingleCell(i, j, cur_rec, thread_limits, block_size);
            // convolution_file << inclusion_set[i][j] << "," ;
        }
        // convolution_file << endl;
    }
    // convolution_file << endl << endl;
}

short Generator::convolutionSingleCell(int x, int y, int cur_rec, ThreadLimits thread_limits, int block_size) const
{
    short num_included = 0;
    int CONVO_BLOCK_SIZE = INIT_CONVO_BLOCK_SIZE + (num_rec - cur_rec);
    if (x < block_size * CONVO_BLOCK_SIZE + thread_limits.x_start || x >= thread_limits.x_end - block_size * CONVO_BLOCK_SIZE)
    {
        if (inclusion_set[x][y] > 0)
            return 1;
        return inclusion_set[x][y];
    }
    if (y < block_size * CONVO_BLOCK_SIZE + thread_limits.y_start || y >= thread_limits.y_end - block_size * CONVO_BLOCK_SIZE)
    {
        if (inclusion_set[x][y] > 0)
            return 1;
        return inclusion_set[x][y];
    }
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
    else if (inclusion_set[x][y] < -256)
    {
        if (num_included == 0)
        {
            return inclusion_set[x][y] + 256;
        }
        else
        {
            return inclusion_set[x][y];
        }
    }
    else
    {
        if (num_included == 0)
        {
            return inclusion_set[x][y] - 256;
        }
        else
        {
            return inclusion_set[x][y];
        }
    }
    return num_included;
}

void Generator::printSet()
{
    ofstream log;
    log.open("Set.csv");
    for (int i = 0; i < y_size; i++)
    {
        for (int j = 0; j < x_size; j++)
        {
            log << inclusion_set[i][j] << ",";
        }
        log << endl;
    }
}

void Generator::run()
{
    int block_size = pow(2, num_rec);
    otp << "Block Size: " << block_size << ", start: " << 0 << endl;
    
    runIter(block_size);
    
    int cur_rec = num_rec - 1;
    int start = pow(2, cur_rec);
    const int threadCount = std::thread::hardware_concurrency();           // 16
    int numInitXBlocks = ceil(x_size / block_size);                        // 24
    int numInitYBlocks = ceil(y_size / block_size);                        // 16
    int ncols = ceil(sqrt(threadCount * numInitXBlocks / numInitYBlocks)); // 4
    int nrows = ceil(threadCount / ncols);                                 // 4
    while (ncols * nrows > threadCount)
    {
        ncols--;
        nrows = ceil(threadCount / ncols);
    }
    cout << "starting run" << endl;
    int threadXSize = numInitXBlocks / ncols; // 6
    int threadYSize = numInitYBlocks / ncols; // 4
    int numThreads = ncols * nrows;           // 16
    std::vector<std::thread> threads;
    cout << "Going to create threads" << endl;
    for (int c = 0; c < ncols - 1; c++)
    {
        for (int d = 0; d < nrows - 1; d++)
        {
            ThreadLimits thread_limit(c, d, threadXSize, threadYSize, block_size);
            threads.emplace_back(&Generator::iterationsWithConvolutions, this, start, cur_rec, block_size, thread_limit);
        }
    }
    for (int d = 0; d < nrows - 1; d++)
    {
        ThreadLimits thread_limit(ncols * threadYSize * block_size, y_size, d * threadXSize * block_size, (d + 1) * threadXSize * block_size - 1);
        threads.emplace_back(&Generator::iterationsWithConvolutions, this, start, cur_rec, block_size, thread_limit);
    }
    for (int c = 0; c < ncols - 1; c++)
    {
        ThreadLimits thread_limit(c * threadYSize * block_size, (c + 1) * threadYSize * block_size, nrows * threadXSize * block_size, x_size);
        threads.emplace_back(&Generator::iterationsWithConvolutions, this, start, cur_rec, block_size, thread_limit);
    }
    ThreadLimits thread_limit(ncols * threadYSize, y_size, nrows * threadXSize, x_size);
    threads.emplace_back(&Generator::iterationsWithConvolutions, this, start, cur_rec, block_size, thread_limit);
    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }
    cout << "Done" << endl;    
}

void Generator::iterationsWithConvolutions(int start, int cur_rec, int block_size, ThreadLimits thread_limits)
{
    do
    {
        otp << "Block Size: " << block_size << ", start: " << start << endl;
        cout << "cur_rec: " << cur_rec << "\tnum_rec: " << num_rec << endl;
        ThreadLimits thread_limits(0, y_size, 0, x_size);
        convolutionIter(block_size, cur_rec, thread_limits);
        runIter(block_size, start, thread_limits);
        start /= 2;
        block_size /= 2;
        cur_rec--;
    } while (block_size > 1);
}

void Generator::runIter(const int block_size, const int start, ThreadLimits thread_limits)
{
    int k = 0, l = 0;
    for (int i = thread_limits.x_start; i < thread_limits.x_end; i += block_size)
    {
        for (int j = thread_limits.y_start + start; j < thread_limits.y_end; j += block_size)
        {
            // otp << "Checking inclusion of " << i << " + " << j << "i" << endl;
            inclusion_set[i][j] = check_inclusion(plane[i][j], inclusion_set[i][k]);
            k += block_size;
        }
        k = 0;
    }
    k = 0;
    for (int i = thread_limits.x_start + start; i < thread_limits.x_end; i += block_size)
    {
        for (int j = thread_limits.y_start; j < thread_limits.y_end; j += block_size)
        {
            // otp << "Checking inclusion of " << i << " + " << j << "i" << endl;
            inclusion_set[i][j] = check_inclusion(plane[i][j], inclusion_set[k][j]);
        }
        k += block_size;
    }
    k = 0;
    for (int i = thread_limits.x_start + start; i < thread_limits.x_end; i += block_size)
    {
        for (int j = thread_limits.y_start + start; j < thread_limits.y_end; j += block_size)
        {
            // otp << "Checking inclusion of " << i << " + " << j << "i" << endl;
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
            // otp << "Checking inclusion of " << i << " + " << j << "i" << endl;
            inclusion_set[i][j] = check_inclusion(plane[i][j], 0);
        }
    }
}

void Generator::create_ouput_file()
{
    unsigned int colors[8] = {0, 255, 1710847, 3355647, 5066239, 6711039, 8421631, 10066431};
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
    for (int j = 0; j < y_size; j++)
    {
        for (int i = 0; i < x_size;)
        {
            for (int k = 0; k < 8; k++, i++)
            {
                f.write((char *)(colors + inclusion_set[j][i]), 3);
            }
        }
    }
    f.close();
}

void Generator::mapColours()
{
    for (int i = 0; i < y_size; i++)
    {
        for (int j = 0; j < x_size; j++)
        {
            if (inclusion_set[i][j] < -256)
            {
                inclusion_set[i][j] += 256;
            }
            if (inclusion_set[i][j] == 1 || inclusion_set[i][j] == 128)
            {
                inclusion_set[i][j] = 0;
            }
            else if (inclusion_set[i][j] < 0 && inclusion_set[i][j] >= -5)
            {
                inclusion_set[i][j] = 1;
            }
            else if (inclusion_set[i][j] < -5 && inclusion_set[i][j] >= -10)
            {
                inclusion_set[i][j] = 2;
            }
            else if (inclusion_set[i][j] < -10 && inclusion_set[i][j] >= -15)
            {
                inclusion_set[i][j] = 3;
            }
            else if (inclusion_set[i][j] < -15 && inclusion_set[i][j] >= -23)
            {
                inclusion_set[i][j] = 4;
            }
            else if (inclusion_set[i][j] < -23 && inclusion_set[i][j] >= -35)
            {
                inclusion_set[i][j] = 5;
            }
            else if (inclusion_set[i][j] < -35 && inclusion_set[i][j] >= -50)
            {
                inclusion_set[i][j] = 6;
            }
            else
            {
                inclusion_set[i][j] = 7;
            }
        }
    }
    printSet();
}