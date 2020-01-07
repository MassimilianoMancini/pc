#include <fstream>
#include <iostream>
#include <cstring>
#include <stdio.h>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <omp.h>

// Number of letters
#define NL 26
#define NL2 NL*NL
#define NL3 NL*NL*NL


int main(int argc, char *argv[])
{
    // Checking parameters, if openmp is activated then number of threads is allowed
    #if defined(_OPENMP)
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <filename> <size> <num_threads>\n";
        return 1;
    }
    #else
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <filename> <size> \n";
        return 1;
    }
    #endif
    
    // NL is number of letters
    int freqb[NL2] = {0};
    int freqt[NL3] = {0};
    
    // Some counters
    char c1 = 'a';
    char c2 = 'a';
    char c3 = 'a';
    
    // File length
    long long int length = 0;
    
    #if defined(_OPENMP)
    int nthreads = atoi(argv[3]);

    // check if second argument is 
    if (nthreads < 1) {
        std::cout << "Warning: invalid threads number. Default to 4 threads\n"; 
        nthreads = 4;
    }
    #else
    int nthreads = 0;
    #endif
    
    std::chrono::high_resolution_clock::time_point t1, t2;
    std::chrono::duration<double> time_span;
    
    // open up all files input and output
    std::ifstream is (argv[1], std::ifstream::in);
    std::ofstream ob ("bigrams.txt", std::ofstream::out);
    std::ofstream ot ("trigrams.txt", std::ofstream::out);
    std::ofstream log ("log.txt", std::ofstream::app);
    
    if (is) {
        is.seekg (0, is.end);
        length = is.tellg();
        is.seekg (0, is.beg);
    } else {
        std::cout << "Error in open input file " << argv[1];
    }
    
    length = std::min(length, atoll(argv[2]));

    // allocate memory for reading file:
    char* buffer = new char [length];

    // read data as a block:
    is.read (buffer,length);
    
    // start time for parallel part (consumers)
    t1 = std::chrono::high_resolution_clock::now();
    
    #if defined(_OPENMP)
    // set numeber of threads
    omp_set_num_threads(nthreads);
    
    // set block size to the nearest multiple of 6 in order to avoid bi/trigrams shift
    long long int blocksize = ((length/nthreads)/6)*6;
    #pragma omp parallel default(none) shared(buffer, length, freqb, freqt, blocksize)
    #endif
    {
        int c[6] = {0};
        int cc = 0;
        int i = 0;
        int ib = 0;
        int it = 0;
        
        #if defined(_OPENMP)
        #pragma omp for reduction (+:freqb,freqt) schedule (static, blocksize)
        #endif
        for (long long int j = 0; j <= length; j++) {
            // current char
            cc = buffer[j];
            
            // sanification
            if (cc >= 'A' && cc <= 'Z') {
                cc = cc + 32;
            }
            
            // insert char in c buffer
            if (cc >= 'a' && cc <= 'z') {
                c[i] = cc;
                i++;
            }
            // if c buffer is full, count 3 bigrams and 2  trigrams
            // avoid another for  cicle by unrolling
            if (i == 6) {
                ib = (c[0] - 'a') * NL + c[1] - 'a';
                freqb[ib] = freqb[ib] + 1;       
                ib = (c[2] - 'a') * NL + c[3] - 'a';
                freqb[ib] = freqb[ib]+1;
                ib = (c[4] - 'a') * NL + c[5] - 'a';
                freqb[ib] = freqb[ib]+1;
                it = (c[0] - 'a') * NL2 + (c[1] - 'a') * NL + c[2] - 'a';
                freqt[it] = freqt[it]+1;
                it = (c[3] - 'a') * NL2 + (c[4] - 'a') * NL + c[5] - 'a';
                freqt[it] = freqt[it]+1;
                
                // reset c buffer
                i = 0;
            } 
                
        }
    }
        
    t2 = std::chrono::high_resolution_clock::now();
    time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    
    log << nthreads << ", " << length << ", " << time_span.count() << "\n";
    
        
    for (int i = 0; i < NL2; i++) {
        if (freqb[i] > 0) {
            ob << c1 << c2 << " " << freqb[i] << "\n";
        }
        c2 = c2 + 1;
        if (c2 > 'z') {
            c2 = 'a';
            c1 = c1 + 1;
        }
    }
    c1 = 'a';
    c2 = 'a';
    for (int i = 0; i < NL3; i++) {
        if (freqt[i] > 0) {
            ot << c1 << c2 << c3 << " " << freqt[i] << "\n";
        }
        c3 = c3 + 1;
        if (c3 > 'z') {
            c3 = 'a';
            c2 = c2 + 1;
        }
        if (c2 > 'z') {
            c2 = 'a';
            c1 = c1 + 1;
        }
    }
    
    free(buffer);
    
    is.close();
    ob.close();
    ot.close();
    log.close();
    
}