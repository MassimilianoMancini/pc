#include <fstream>
#include <iostream>
#include <cstring>
#include <stdio.h>
#include <cstdlib>
#include <time.h> 

#define NL 26
#define NL2 NL*NL
#define NL3 NL*NL*NL


int main(int argc, char *argv[])
{
    // Checking parameters, if openmp is activated then number of threads is allowed
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <filename> <size> \n";
        return 1;
    }
    
    // NL is number of letters
    int freqb[NL2] = {0};
    int freqt[NL3] = {0};
    
    // Some counters
    char c1 = 'a';
    char c2 = 'a';
    char c3 = 'a';

    // open up all files input and output
    std::ofstream of (argv[1], std::ofstream::out);
	std::ofstream ob ("gen_bigrams.txt", std::ofstream::out);
    std::ofstream ot ("gen_trigrams.txt", std::ofstream::out);
   
    // File length
	long long int length = 0;
    length = atoll(argv[2]);

	char c[6] = {0};
	int ib = 0;
	int it = 0;
	int i = 0;
	
	srand(time(NULL));
	
	for (long long int j = 0; j <= length; j++) {
		c[i] = rand() % 26 + 97;
		
		i++;
		
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
			
			for (int k = 0; k <= 5; k++) {
				of << c[k];
			}
			i = 0;
		}
			
	}
            
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
    
    
    of.close();
    ob.close();
    ot.close();    
}
