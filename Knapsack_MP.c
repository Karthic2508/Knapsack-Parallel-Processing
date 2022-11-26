#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <time.h>
#include <sys/times.h>
#include <sys/time.h>

#define INCLUDED 0
#define NOT_INCLUDED 1
#define max(a,b) (a>b?a:b)
#define MAIN_PROC 0
#define N_MAX 1000
#define VALUE_MAX 100
#define WEIGHT_MAX 1000
#define EVAL_TAG 0x20

void outputs();
void inputs(int argc, char *argv[]);

int numproc, rank;
int knapsack_capacity, n, seed;
int *value;
int *weight;
int table_cost[N_MAX][WEIGHT_MAX];
int table_s[N_MAX][WEIGHT_MAX];
int *map;
int colPerProc;


int main(int argc,char *argv[]) {
	int i, j, k;
	int cost_with_i, cost_without_i;
	MPI_Status status;
	MPI_Request request;

	 /* Timing variables */
  	struct timeval etstart, etstop;  /* Elapsed times using gettimeofday() */
	struct timezone tzdummy;
	unsigned long long usecstart, usecstop;


	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&numproc);

	value = malloc(sizeof(int)*N_MAX);
	weight =(int*)malloc(sizeof(int)*N_MAX);
	map = (int*)malloc(sizeof(int)*N_MAX);
	
	inputs(argc, argv);	
	MPI_Barrier(MPI_COMM_WORLD);

  	/* Start Clock */
	if(rank==0) printf("\nStarting clock.\n");
	gettimeofday(&etstart, &tzdummy);

	//For each item
	for(i=0;i<n;i++){
		// Each rank computes its columns
		for(j=rank; j<knapsack_capacity; j+=numproc){

			//Find the best value with the new item
			if(j - weight[i]<0)
				cost_with_i=0;

			else if(i == 0 && j-weight[i] >= 0)
				cost_with_i=value[i];

			else{
				MPI_Recv(&cost_with_i, 1, MPI_INT, map[j-weight[i]], i-1, MPI_COMM_WORLD, &status);
				cost_with_i+=value[i];
			}

			//Find the best value without the new item
			cost_without_i = (i==0) ? 0 : table_cost[i-1][j];

			//Compute T[i][j]
			table_cost[i][j] = max(cost_with_i, cost_without_i);
			table_s[i][j] = (table_cost[i][j]==cost_without_i) ? NOT_INCLUDED : INCLUDED;

			//Send to all procs that could need the new value (non blancking)
			for(k=j+1; k<knapsack_capacity; k++){
				if(k-j == weight[i+1]){
					MPI_Isend(&table_cost[i][j], 1, MPI_INT, map[k], i, MPI_COMM_WORLD, &request);
				}
			}

		}
	}


	/* Stop Clock */
	gettimeofday(&etstop, &tzdummy);
	if(rank==0) printf("Stopped clock.\n");
	usecstart = (unsigned long long)etstart.tv_sec * 1000000 + etstart.tv_usec;
	usecstop = (unsigned long long)etstop.tv_sec * 1000000 + etstop.tv_usec;

	MPI_Barrier(MPI_COMM_WORLD);

	outputs();

	/* Display timing results */
	if(rank==0) printf("\nElapsed time = %g ms.\n", (float)(usecstop - usecstart)/(float)1000);

	MPI_Finalize();
	return 0;
}


void inputs(int argc, char *argv[]){
	int i;

	if(argc != 4){
		if(rank == MAIN_PROC)
			printf("Error: knapsack <max capacity weight> <number of items> <seed>\n");
		MPI_Finalize();
		exit(0);
	}

	knapsack_capacity = atoi(argv[1])+1;
	n  = atoi(argv[2]);
	seed = atoi(argv[3]);
	
	if(n>N_MAX){
		if(rank == MAIN_PROC)
			printf("Error: max number of items is %d\n", N_MAX);
		MPI_Finalize();
		exit(0);
	}
	if(knapsack_capacity>WEIGHT_MAX+1){
		if(rank == MAIN_PROC)
			printf("Error: capacity maximum is %d\n", WEIGHT_MAX);
		MPI_Finalize();
		exit(0);
	}

	srandom(seed);

	for(i=0;i<n;i++){
		value[i]=random()%VALUE_MAX;
		weight[i]=random()%(knapsack_capacity-1)+1;
		if(rank == MAIN_PROC)
			printf("\tItem %d: value = %d / Weight = %d\n", i, value[i], weight[i]);
	}


	for (i = 0; i < knapsack_capacity; i++){
		map[i] = i%numproc;
	}

}

void outputs(){
	int i, j;
	MPI_Status status;
	MPI_Request request;

	int table_svg[N_MAX][WEIGHT_MAX];

	// Assimilate Results and Print them
	if(rank != map[knapsack_capacity-1]){
		for(j=rank; j<knapsack_capacity-1; j+=numproc){
			MPI_Isend(table_s, N_MAX*WEIGHT_MAX, MPI_INT, map[knapsack_capacity-1], EVAL_TAG, MPI_COMM_WORLD, &request);
		}
	}else{

		memcpy(table_svg, table_s, N_MAX*WEIGHT_MAX);

		printf("\n\nResults:\n");
		printf("\tKnapsack max weight: %d / Number of items: %d\n", knapsack_capacity-1, n);
		printf("\tBest value: %d \n", table_cost[n-1][knapsack_capacity-1]);
		printf("\tItems taken: ");
		
		j=knapsack_capacity-1;

		for(i=n-1; j>0 && i>=0; i--){
			if(table_s[i][j] == INCLUDED){
				printf("%d, ",i);

				j-=weight[i];
				
				if(j > 0 ){
					if (map[j]!=rank) {
						MPI_Recv(table_s, N_MAX*WEIGHT_MAX, MPI_INT, map[j], EVAL_TAG, MPI_COMM_WORLD, &status);
					} else {
						memcpy(table_s, table_svg, N_MAX*WEIGHT_MAX);
					}

				}
			}
		}
		printf("\n\n");
		
	}
}