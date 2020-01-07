#include <stdio.h>
#include <crypt.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <chrono>
#include <ctime>

const char alphabet[64] = {'.', '/', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                           'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
                           'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                           'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
                           'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};


long long int n;


char *pwdes;
char found_pw[9] = "";
int found = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

struct chunk_info {
    long long int start;
    long long int size;
};

void* worker (void *input) {
    
    // struct to get args
	struct chunk_info *ci;
    ci = (chunk_info *) input;
    
    long long int i = 0;
    
	// used in reentrant crypt func
    struct crypt_data cryptdata;
    cryptdata.initialized = 0;
        
    // calc starting password (reverse linear address to string)
	char test[] = "........";
    
    for (int j = 7; j >= 0; j--) {
        test[j] = alphabet[ci->start % 64];
        ci->start = ci->start / 64;    
    }
     
    char *testdes = (char *) malloc(14);
    strncpy(testdes, crypt_r(test, "00", &cryptdata),13);
    
    int index[8] = {0};
    
    while ((i < ci->size) && (strcmp(pwdes, testdes) != 0) && (! found)) {
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
        strncpy(testdes, crypt(test, "00"),13);
    }
    
	// if pw is found, mutex is used to access shared variables
    if (strcmp(pwdes, testdes) == 0) {
        pthread_mutex_lock(&lock);
        found = 1;
        strcpy(found_pw, test);
        pthread_mutex_unlock(&lock);
        pthread_exit(NULL);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    
    if (argc < 3) {
        printf("Usage: %s <password> <trials * 10^6> <threads>\n", argv[0]);
        return 1;
    }
    
    struct timespec t1, t2;
    double time_span;
    
	// get start time
    clock_gettime(CLOCK_REALTIME, &t1);
    
    pwdes = (char *) malloc (14);
    strncpy(pwdes, crypt(argv[1], "00"), 13);
    
	// number of keys
    if (atoi(argv[2]) == 0) {
        n = 281474976710656; // 64^8 or 2^48
    } else {
        n = atoi(argv[2]) * 1000000;
    }
    
	// number of threads to use
    int n_threads = atoi(argv[3]);
    int thread_idx = n_threads - 1;
    
    pthread_t tid[n_threads];
    struct chunk_info ci[n_threads];
    
	// number of keys per thread
    long long int size = n/n_threads;
    
    for (int i = 0; i < thread_idx; i++) {
        ci[i].start = size * i;
        ci[i].size = size;
        pthread_create(&tid[i], NULL, worker, (void *) &ci[i]);
    }
    
	// if n_threads is odd, we catch remainder
    ci[thread_idx].start = size * (thread_idx);
    ci[thread_idx].size = n - ci[thread_idx].start;
    
    pthread_create(&tid[thread_idx], NULL, worker, (void *) &ci[thread_idx]);
    
	// join all threads
    for (int i = 0; i < n_threads; i++) {
        pthread_join(tid[i], NULL);
    }
    
	// get stop time
    clock_gettime(CLOCK_REALTIME, &t2);
    time_span = (t2.tv_sec - t1.tv_sec) + (double)((t2.tv_nsec - t1.tv_nsec))/1000000000;
    
    if (found) {
        printf("Found pw: %s in %lf secs\n", found_pw, time_span);
    } else {
        printf("Not found, tested %llu in %lf secs\n", n, time_span);
    }
    return 0;
}

