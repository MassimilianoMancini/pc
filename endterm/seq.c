#include <stdio.h>
#include <crypt.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

const char alphabet[64] = {'.', '/', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                           'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
                           'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                           'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
                           'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
long long int n;



int main(int argc, char *argv[]) {
    
    if (argc < 3) {
        printf("Usage: %s <password> <trials * 10^6>\n", argv[0]);
        return 1;
    }
    
	// Number of keys
    if (atoi(argv[2]) == 0) {
        n = 281474976710656; // 64^8 or 2^48
    } else {
        n = atoi(argv[2]) * 1000000;
    }
    
    struct timespec t1, t2;
    double time_span;
    
	// get start time
    clock_gettime(CLOCK_REALTIME, &t1);
    
    char salt[] = "00";
    char *pwdes = (char *) malloc (14);
    char *testdes = (char *) malloc(14);
        
    long long int i = 0;
	
	// initial password
    char test[] = "........";
    int index[8] = {0};
    
    strncpy(pwdes, crypt(argv[1], salt), 13);
    strncpy(testdes, crypt(test, salt),13);
    
	// test until number of trials is reached or pw is found
    while ((i < n) && (strcmp(pwdes, testdes) != 0)) {
        index[7]++;
        if (index[7] == 64) {
            index[7] = 0;
            test[7] = '.';
            index[6]++;
            if (index[6] == 64) {
                index[6] = 0;
                test[6] = '.';
                index[5]++;
                if (index[5] == 64) {
                    index[5] = 0;
                    test[5] = '.';
                    index[4]++;
                    if (index[4] == 64) {
                        index[4] = 0;
                        test[4] = '.';
                        index[3]++;
                        if (index[3] == 64) {
                            index[3] = 0;
                            test[3] = '.';
                            index[2]++;
                            if (index[2] == 64) {
                                index[2] = 0;
                                test[2] = '.';
                                index[1]++;
                                if (index[1] == 64) {
                                    index[1] = 0;
                                    test[1] = '.';
                                    index[0]++;
                                } else {
                                    test[1] = alphabet[index[1]];
                                }
                            } else {
                                test[2] = alphabet[index[2]];
                            }
                        } else {
                            test[3] = alphabet[index[3]];
                        }
                    } else {
                        test[4] = alphabet[index[4]];
                    }                    
                } else {
                    test[5] = alphabet[index[5]];
                }
            } else {
                test[6] = alphabet[index[6]];
            }           
        } else {
            test[7] = alphabet[index[7]];
        }
        
        i++;
        strncpy(testdes, crypt(test, salt),13);
    }
    
	// get stop time and calc time_span
    clock_gettime(CLOCK_REALTIME, &t2);
    time_span = (t2.tv_sec - t1.tv_sec) + (double)((t2.tv_nsec - t1.tv_nsec))/1000000000;
    
    if (strcmp(pwdes, testdes) == 0) {
        printf("Found password %s in %lf secs\n", test, time_span);
    } else {
        printf("Not found, tested %llu in %lf secs\n", n, time_span);
    }
}