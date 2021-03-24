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

const int MAX = 10;	   // Maximum number of threads allowed
const int UPPER = 100; // Number of times the counter is incremented by
					   // each thread

// Global variables

int counter = 0;	// variable to be incremented by each thread
sem_t semaphore;	// global semaphore, used for mutual exclusion
pthread_t tid[MAX]; // array of thread identifiers

int num1[256], num2[256];
int product[512];
int credit[256];

int l2;
int l1;

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

void printBigNumber(int buffer[], int numL)
{
	bool foundBeg = false;
	for (int x = 0; x < numL; x++)
	{

		cout << buffer[x];
	}
	cout << endl;
}

void multRange(int num1[], int num2[], int product[], int i, int len2, int id)
{
	credit[i] = id;

	for(int x = 0; x < len2; x++){

		product[i+x + 1]  += num1[i] * num2[x];

	}

}


void carry(int product [], int index){
	sem_wait(&semaphore);
	product[index - 1] += product[index] / 10;
	product[index] %= 10;
	sem_post(&semaphore);
}


void *threadMultiply(void *arg)
{

	int id = (long)arg;

	int work_counter;

	while (true)
	{

		sem_wait(&semaphore);

		work_counter = counter;
		

		if (counter == l1)
		{
			sem_post(&semaphore);
			return NULL;
		}

		counter++;
		sem_post(&semaphore);

		cout << "thread " << id << " beginning work " << work_counter << endl;
		multRange(num1, num2, product, work_counter, l2, id);
	}
}

void *threadCarry(void *arg)
{

	int id = (long)arg;

	int work_counter;

	while (true)
	{

		sem_wait(&semaphore);

		work_counter = counter;
		

		if (counter == -1)
		{
			sem_post(&semaphore);
			return NULL;
		}

		counter--;
		sem_post(&semaphore);

		cout << "thread " << id << " beginning work " << work_counter << endl;
		carry(product,work_counter);
	}
}


int main()
{
	int i, no_threads;

	ifstream infile("./test.txt");
	FILE *file = fopen("./test.txt", "r");

	l1 = readBigNumber(num1, 256, file);
	l2 = readBigNumber(num2, 256, file);

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

	for (i = 0; i < no_threads; i++)
		pthread_join(tid[i], NULL);

	printBigNumber(product, l1 + l2);

	counter = l1 + l2;

	for (i = 0; i < no_threads; i++)
		pthread_create(&tid[i], NULL, threadCarry, (void *)i);

	for (i = 0; i < no_threads; i++)
		pthread_join(tid[i], NULL);
	


	printBigNumber(product, l1 + l2);
	printBigNumber(credit, l1 + l2);

	return 0;
}