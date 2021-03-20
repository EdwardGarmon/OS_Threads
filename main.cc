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
#include <semaphore.h>
#include <unistd.h>

using namespace std;

// Global constants

const int MAX = 10;     // Maximum number of threads allowed
const int UPPER = 100; // Number of times the counter is incremented by
								// each thread

// Global variables

int counter = 0;        // variable to be incremented by each thread 
sem_t semaphore;	      // global semaphore, used for mutual exclusion
pthread_t tid[ MAX ];   // array of thread identifiers

// ***********************************************************************
// Function that each thread will execute.  If the semaphores work
// properly, then the function will effectively do, count += UPPER.
// ***********************************************************************

void *
increment( void *arg )
{
	// Parameter passed is the thread number.  Since the parameter could be 
	// anything, it is passed as void * and must be "converted".

	int id = (long) arg;
	int a;

	cout << "Thread " << id << " is starting!" << endl;

	// Simply iterate UPPER times, incrementing the counter each iteration.
	// Since the variable counter is a shared/global variable, protect
	// the modification of the variable by a semaphore.

	for( int j = 0; j < UPPER; j++ )
	{
		sem_wait( &semaphore ); // semaphore WAIT operation
		counter++;
//		a=counter*2;
		sem_post( &semaphore ); // semaphore SIGNAL operation
	}

	cout << "Thread " << id << " is ending!" << endl;
	return NULL;
}

// ***********************************************************************
// The main program:
//    1) Reads in, n, the number of threads to create.
//    2) Creates n threads, each one identical, and running the
//       increment() function.
//    3) Wait for each thread to complete execution.  Since each thread
//       simply increments the counter UPPER times, the value of the
//       counter when each thread terminates should be n*UPPER.
// ***********************************************************************


int readBigNumber(int buffer [], int bufferSize, FILE * input){
	int read = getc(input);
	int p = 0;
	for(p = 0; p < 256 && read != '\n' && read != -1; p++){
		buffer[p] = read - 48;
		read = getc(input);
	}
	return p;
}


void printBigNumber(int buffer [], int numL) {
	for(int x = 0; x < numL; x++){
		cout << buffer[x];
	}
	cout << endl;
}

int main()
{
	int i, no_threads;

	int num1 [256], num2 [256];

	int product [512];

	FILE * testFile = fopen("test.txt","r");

	if(testFile == NULL){
		cout << "error opening file" << endl;
		return -1;
	}

	int l1 = readBigNumber(num1,256,testFile);
	int l2 = readBigNumber(num2,256,testFile);





	printBigNumber(num1,l1);
	printBigNumber(num2,l2);
	printBigNumber(product,l1+l2);


	// Prompt for number of threads.  Default to four if an invalid number
	// is given.

	/*	cout << "Enter number of threads to create: " << endl;
	cin >> no_threads;
	if( ( no_threads <= 0 ) || ( no_threads >= MAX ) )
	{
		cout << "Invalid thread count -- defaulting to 4" << endl;
		no_threads = 4;
	}

	// Initialize the mutual exclusion semaphore.

	sem_init( &semaphore, 0, 1 );

	// Create no_threads identical threads each one running the increment()
	// function, saving the thread-id in the tid array, and using the default 
	// attributes.  Each thread calls the function increment(), with one
	// argument "i", used as the thread number.

	for( i = 0; i < no_threads; i++ ) 
		pthread_create( &tid[ i ], NULL, increment, (void *) i );

	// Wait for all the threads to exit.  This suspends the calling thread,
	// i.e., the main program, until all target threads complete.

	for( i = 0; i < no_threads; i++ ) 
		pthread_join( tid[ i ], NULL );
  
	// Display final contents of counter.  We know what the value should
	// be if the mutual exclusion semaphores worked properly.

	cout << endl << "Final value of counter: " << counter;
	cout << " (should be " << no_threads * UPPER << ")" << endl;
	*/
	return 0;
}