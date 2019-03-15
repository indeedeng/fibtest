/*
 * Copyright (C) 2019 Indeed Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 * in compliance with the License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the
 * License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: "Dave Chiluk"
 *
 * Description: This application runs the fibonacci sequence on multiple
 * threads each pinned to a *  CPU for a requested amount of time.  It then prints
 * out the total number of iterations it was able to complete.
 */

// This is needed for CPU* functions
#define _GNU_SOURCE	
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <pthread.h>
#include <sys/sysinfo.h>
#include <errno.h>
#include <sched.h>
#include <time.h>

// Returns number of iterations completed.
static void *fast( void *arg );
static void *slow( void *arg );

// Handle alarm and print number of iterations.
void handle_sigalrm();

struct thread_info {
	pthread_t thread_id;
	unsigned long iterations;
	unsigned long fib_res;	
};

struct timespec endtime;

void usage ()
{
	fprintf (stdout,
	"Usage: $0 -t <threads> \n"
	"This application runs the fibonacci sequence on multiple\n"
	"threads each pinned to a CPU for a requested amount of time.\n"
	"It then prints out the total number of iterations it was able to complete.\n"
	"\n"
	"It divides execution into fast and slow threads. Fast threads run as fast\n"
	"as possible and slow threads run 100 iterations and the sleep for 10ms.\n"
	"\n"
	"Options\n"
	"-v, Verbose prints total number of iterations per thread\n"
	"-f <NUM>, number of fast threads (Default=1).\n"
	"-t <NUM>, total number of threads (Default=Number of CPUs)\n"
	"-s <NUM>, Sets the duration of the test (Default=5 seconds)\n"
	);
}

int main(int argc, char *argv[])
{	
	unsigned long total=0;
	int c, num_threads=2, procs, fast_threads=1;
	struct thread_info *tinfo;
	bool verbose=false;
	cpu_set_t cpuset;
	int seconds=5; // Default test duration

	// Get the number of processors
	// CHILUK: this should handle the case where the task group is pinned to a subset
	//  of cores.
	procs = get_nprocs_conf();
	num_threads = procs;

	// Argument processing.
	while ((c = getopt (argc, argv, "vf:t:s:")) != -1)
	{
		// CHILUK: fix arguements and usage.
		switch (c)
		{
			case 'v':
				verbose = true;
				break;
			case 'f':
				fast_threads = (int) strtol(optarg, NULL, 0);
				break;
			case 't':
				num_threads = (int) strtol(optarg, NULL, 0);
				break;
			case 's':
				seconds = (int) strtol(optarg, NULL, 0); 
				break;	
			case '?':
			default:
				usage();
				exit(0);
		}
		if( num_threads < fast_threads)
			num_threads = fast_threads;
	}

	tinfo = calloc(num_threads, sizeof(struct thread_info));
	
	// Set the end time
	if (clock_gettime(CLOCK_REALTIME_COARSE, &endtime )) {
		fprintf(stderr, "Unable to get endtime");
		exit(1);
	}
	endtime.tv_sec += seconds;

	// Spawn requested number of threads
	for( int tnum=0; tnum < num_threads; tnum++) {
		// CHILUK: check return values
		// Balance threads accross cores.
		if( tnum % procs < fast_threads )
			pthread_create(&tinfo[tnum].thread_id, NULL, fast, &tinfo[tnum]);
		else
			pthread_create(&tinfo[tnum].thread_id, NULL, slow, &tinfo[tnum]);
		// Set the cpu affinity to tnum%(number of cpus)
		CPU_ZERO(&cpuset);
		CPU_SET(tnum % procs, &cpuset);
		pthread_setaffinity_np(tinfo[tnum].thread_id, sizeof(cpu_set_t), &cpuset );
	}

	// Wait for thread completion.
	for( int tnum=0; tnum < num_threads; tnum++)
		// CHILUK: check return values
		pthread_join(tinfo[tnum].thread_id, NULL);

	// Add up the total number of iterations per thread.
	for( int tnum=0; tnum < num_threads; tnum++)
	{
		if (verbose)
			printf("fib[%lu] = %lu \n", tinfo[tnum].iterations, tinfo[tnum].fib_res);
		total += tinfo[tnum].iterations;
	}	

	printf("Iterations Completed(M): %lu \n", total/1000000 );
	free(tinfo);
	exit(0);
}

// Checks to see if we have reached the end of our test time.
bool done()
{
	struct timespec cur;
	if (clock_gettime( CLOCK_REALTIME_COARSE , &cur)) {
		fprintf(stderr, "Unable to get curtime");
		exit(1);
	}
	if (cur.tv_sec == endtime.tv_sec)
		return cur.tv_nsec > endtime.tv_nsec;
	else
		return cur.tv_sec > endtime.tv_sec;
}

// Runs the fibonacci sequence 100 iterations at a time then sleeps for 10ms.
static void *slow( void *arg) 
{
	struct thread_info *tinfo = arg;
	long int a=0;
	long int b=1;
	long int c=0;
	long int i=0;

	while( ! done() )
	{
		for(int j=0; j< 100; j++)
		{
			i++;
			c=a+b;
			a=b;
			b=c;
		}
		usleep ( 10000 );
	}
	// We've timed out.
	tinfo->iterations=i;
	tinfo->fib_res=a;
	
	return (void *)tinfo;
}

// Runs the fibonacci sequence 1 million iterations before checking if we've timed out.
static void *fast( void *arg) 
{
	struct thread_info *tinfo = arg;
	long int a=0;
	long int b=1;
	long int c=0;
	long int i=0;

	while( ! done() )
	{
		for(int j=0; j< 1000000; j++)
		{
			i++;
			c=a+b;
			a=b;
			b=c;
		}
	}
	// We've timed out.
	tinfo->iterations=i;
	tinfo->fib_res=a;
	return (void *)tinfo;
}
