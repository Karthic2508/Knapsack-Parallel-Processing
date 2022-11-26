#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>
#define MAX 1500





int max(int a, int b) {
    return a > b ? a : b;
}





int thread_count;



/*
 * Arguments:
 * `N`: Total number of items.
 * `max_`: The maximum weight and value of an item.
 * `w`: Array of weights. 
 * `v`: Array of values.
 *  create_problem will generate the components passed in the argument by the user. rand function is use to generate the random inouts for the argumnets passed at the time of output.
 */
 
 
 
 
void create_problem(int N, int max_, int w[N], int v[N]) {
    int i;
    for (i=0 ; i<N ; i++) {
        w[i] = (rand() % max_) + 1;
        v[i] = (rand() % max_) + 1;
    }
}



/*
 * Implementation of parallel knapsack.
 * It uses openMP to parallelize the column using a pragma parallel for
 * The main improvement over the sequential version is
 * that it only uses an array of two rows because for every element
 * to be computed, we only need two elements from the previous row.
 */

 
 int solve(int N, int C, int *w, int *v) {
    int i, j;
    int dp[2][MAX];
    for (j=0 ; j<=C ; j++) {
        dp[0][j] = 0;
        dp[1][j] = 0;
    }

    // Start on the second row.
	
	
    int current = 1;
    for (i=1 ; i<=N ; i++) {
       #pragma omp parallel for num_threads(thread_count)
        for (j=1 ; j<=C ; j++) {
            dp[current][j] = dp[!current][j];
            if (j-w[i-1] >= 0) {
                dp[current][j] = max(dp[current][j], v[i-1] + dp[!current][j-w[i-1]]);
            }
        }
        current = !current;
		
		// * After the computation for whole j is done then at the end current is replaced with the maximum current value.
    }

    return dp[!current][C];
}



/*
 * Implementation of the professor of the Knapsack problem
 * Same arguments as the previous function
 */
 
 
 
int solve2(int n, int c, int weight[MAX], int profit[MAX]) {
    int i, j;
    int total[2][MAX];
    int current = 0;
    /* Max profit, if it fits */
    for (j = 0; j < c; j++) {
        if (weight[0] > j) {
            total[current][j] = 0;
        } else {
            total[current][j] = profit[0];
        }
    }

    for (i = 1; i < n; i++) {
        for (j = 0; j < c; j++) {
            if ( (j<weight[i]) ||
                (total[!current][j] >= total[!current][j-weight[i]] + profit[i])) {
                total[current][j] = total[!current][j];
            } else {
                total[current][j] = total[!current][j-weight[i]] + profit[i];
            }
        }
        current = !current;
    }

    return total[!current][c-1];
}




/*
 * Returns the amount of seconds passed from a time `tstart` to 
 * `tend`.
 */
 
 
double check_time(struct timeval tstart, struct timeval tend) {
    unsigned long long usec;
    if (tend.tv_usec > tstart.tv_usec) {
        usec = (tend.tv_sec - tstart.tv_sec) * 1000000
                    + tend.tv_usec - tstart.tv_usec;
    } else {
        usec = (tend.tv_sec - (tstart.tv_sec + 1)) * 1000000
                + (1000000 + tend.tv_usec - tstart.tv_usec);
    }
    return (double)usec;
}




int main(int argc, char *argv[]) {
    int N, C, ans = 0;
    int max_weights;
    int weight[MAX], value[MAX];
    struct timeval tstart, tend;
    double t1, t2;

    thread_count = 6;
    N = strtol(argv[1], NULL, 10);
    C = strtol(argv[2], NULL, 10);
    max_weights = strtol(argv[3], NULL, 10);

    create_problem(N, max_weights, weight, value);

    gettimeofday(&tstart, NULL);
    ans = solve(N, C, weight, value);
    gettimeofday(&tend, NULL);


    t1 = check_time(tstart, tend);
    printf("Create method finished in %lf seconds.\n", t1/1000000.0);

    gettimeofday(&tstart, NULL);
    solve2(N, C, weight, value);
    gettimeofday(&tend, NULL);

    t2 = check_time(tstart, tend);

    double s_scaling = t2 / (thread_count * t1);

    printf("Swap finished in %lf seconds.\n", t2/1000000.0);
    printf("Optimum wieght for : %d \n", ans);
    printf("Total time taken : %lf\n", s_scaling);

    return 0;
}