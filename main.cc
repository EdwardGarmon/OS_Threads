/*
    OS project 1

    Johnathan Hocevar
    Edward Garmon
    Donovan Sip


*/

#include <pthread.h>
#include <iostream>
#include <fstream>
#include <semaphore.h>
#include <unistd.h>

using namespace std;

// Global constants
int MAX = 10;    // Maximum number of threads allowed

// Global variables
sem_t semaphore;    // global semaphore, used for mutual exclusion
pthread_t tid[256]; // array of thread identifiers

int num1[256];
int num2[256];
int product[512];
int credit[256];  // Stores which thread calculated which digit

int l2 = 0; // Length of number 2
int l1 = 0; // Length of number 1
int counter;    // variable to be decremented by each thread (will start at l2)

// Reads a big number from a file given
int readBigNumber(int buf [] , int len, istream * in){

    char num; 
    int p = 0;
    in->get(num);
    while(num != '\n' && in->good() && p < len){
        
        buf[p] = (int)(num) - 48;
        p++;
        in->get(num);
    }

    return p;
}

// Prints a number stored in a n array given its length as well
void printBigNumber(int buffer[], int numL, ostream * out)
{
    cout << "printing number size " << numL << endl;
    for (int x = 0; x < numL; x++)
    {       
        if(x == 0 && buffer[x] == 0) continue;
        *out << buffer[x];

    }
    *out << endl;
}


//Johnathan's ideas to do partial summing
void addPartialSum(int partial_sum[])
{
    for (int i = l1+l2-1; i >= 0; i--) {
        product[i+1] += partial_sum[i+1];
        product[i] += product[i+1] / 10;
        product[i+1] %= 10;
    }
    product[0] += partial_sum[0];
}

//Edward's idea to use for loop across the other num
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

    int* partial_sum = new int[512];

    for(int i = 0; i < 512; i++){
        partial_sum[i] = 0;
    }

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
        multRange(partial_sum, work_counter, id);
    }
}

int main(int argc, char * argv[])
{   

    if(argc == 1){
        cout << "input an input file name" << endl;
        return -1 ;
    }

    char * fileName = argv[1];



    int i, no_threads;
	char n[100];

    ifstream file(fileName);
    ofstream out("output.txt");


    while(!file.eof()){
        
        //clear product in between reading inputs
        if(l1 + l1 > 0){

            for(int i = 0; i < l1+l2; i++){
                product[i] = 0;
            }

        }
        
        //read in number of threads
        file >> no_threads;
        file.ignore();

        //get around bad number parsing
        if(file.eof()) break;

        //read in two big numbers
        l1 = readBigNumber(num1, 256, &file);
        l2 = readBigNumber(num2, 256, &file);

        MAX = min(l1,l2);

        //display size of numbers
        cout << l1 << " size of num 1"<< endl;
        cout << l2 << " size of num 2" << endl;

        counter = l2;

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


        printBigNumber(product, l1 + l2,&out);
        //printBigNumber(credit, l1,&cout);

    }

    out.close();
    file.close();

    return 0;
}