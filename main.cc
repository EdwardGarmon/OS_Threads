// ***********************************************************************
// Example of use of pthreads library.
//
// Save this program in "m.cc", then compile using:
//    g++ m.cc -lpthread
// To execute, run the associated "a.out" file (or specify executable on
// the -o option).  The program will prompt for the number of threads to
// create.  Each thread will then increment a counter UPPER times.  The
// counter is global, i.e., shared, and so the access to it is protected
// by a binary semaphore.
// ***********************************************************************

#include <pthread.h>
#include <iostream>
#include <fstream>
#include <semaphore.h>
#include <unistd.h>

using namespace std;

// Global constants
const int MAX = 10;    // Maximum number of threads allowed

// Global variables
sem_t semaphore;    // global semaphore, used for mutual exclusion
pthread_t tid[MAX]; // array of thread identifiers

int num1[256];
int num2[256];
int product[512];
int credit[256];  // Stores which thread calculated which digit

int l2; // Length of number 2
int l1; // Length of number 1
int counter;    // variable to be decremented by each thread (will start at l2)

// Reads a big number from a file given
int readBigNumber(int buffer[], int bufferSize, FILE *input)
{
    int read = getc(input);
    int p = 0;
    for (p = 0; p < 256 && read != '\n' && read != -1; p++)
    {

        buffer[p] = read - 48;
        read = getc(input);
    }
    return p;
}

// Prints a number stored in a n array given its length as well
void printBigNumber(int buffer[], int numL)
{
    bool foundBeg = false;
    for (int x = 0; x < numL; x++)
    {
        cout << buffer[x];
    }
    cout << endl;
}

void addPartialSum(int partial_sum[])
{
    printBigNumber(partial_sum, l1+l2);
    for (int i = l1+l2-1; i >= 0; i--) {
        product[i+1] += partial_sum[i+1];
        product[i] += product[i+1] / 10;
        product[i+1] %= 10;
    }
    product[0] += partial_sum[0];
    printBigNumber(product, l1+l2);
}

void multRange(int partial_sum[], int digit1, int id)
{
    credit[digit1] = id;
    // sem_wait(&semaphore);
    for (int digit2 = l1 - 1; digit2 >= 0; digit2--)
    {
        partial_sum[digit1 + digit2 + 1] += num1[digit1] * num2[digit2];
        partial_sum[digit1 + digit2] += partial_sum[digit1 + digit2 + 1] / 10;
        partial_sum[digit1 + digit2 + 1] %= 10;
    }
    // sem_post(&semaphore);
}

// Thread passed to each thread
void *threadMultiply(void *arg)
{
    int id = (long long)arg; // Used to monitor which thead did which digit

    int work_counter; // an "offset" for which digit we are calculating

    int* partial_sum = new int[256]{0};

    // Provide an infinite loop for the thread to check if there is work
    while (true)
    {
        // Only 1 thread can update the global digit counter at once
        sem_wait(&semaphore);

        work_counter = counter; // Grab the current digit

        if (counter < 0)
        {
            sem_post(&semaphore);
            return partial_sum;
        }
        counter--; // Move to the next digit
        sem_post(&semaphore);

        cout << "thread " << id << " beginning work " << work_counter << endl;
        multRange(partial_sum, work_counter, id);
    }
}

int main()
{
    int i, no_threads;

    ifstream infile("./test.txt");
    FILE *file = fopen("./test.txt", "r");

    l1 = readBigNumber(num1, 256, file);
    l2 = readBigNumber(num2, 256, file);
    counter = l2 - 1;

    printBigNumber(num1, l1);
    printBigNumber(num2, l2);

    no_threads = getc(file) - 48;

    cout << no_threads << " no of threads" << endl;

    if ((no_threads <= 0) || (no_threads >= MAX))
    {
        cout << "Invalid thread count -- defaulting to 4" << endl;
        no_threads = 4;
    }

    sem_init(&semaphore, 0, 1);

    for (i = 0; i < no_threads; i++)
        pthread_create(&tid[i], NULL, threadMultiply, (void *)i);

    int* partial_sum;
    for (i = 0; i < no_threads; i++) {
        pthread_join(tid[i], (void **)&partial_sum);
        addPartialSum(partial_sum);
    }

    printBigNumber(product, l1 + l2);
    printBigNumber(credit, l1 + l2);

    return 0;
}