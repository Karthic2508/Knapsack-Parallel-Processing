PROBLEM STATEMENT

    The Parallel Processing Approach to the Dynamic Programming Algorithm of Knapsack Problem 
PROPOSED SOLUTION 

    Overview 
    
        This paper aims at comparing the serial, shared memory parallelization, and distributed memory parallelization of the dynamic programming algorithm for the Knapsack Problem. 
        
        
        Knapsack Problem is one of the most popular optimization problems. This is the decision-making problem and uses for real-world situations such as business projects, airline cargo business, cryptography, and decision-making industry processes, etc. 
        
        
        The purpose of this project is intended to develop the computing technology of the dynamic programming algorithm of the Knapsack Problem and to study the optimal technic for parallel computing of the algorithm.
        
        
        The algorithm under consideration is the table based dynamic programming algorithm based on Bellman's optimality principle. 
        
        For the shared memory parallelization to solve this problem with the dynamic programming algorithm, we used OpenMP. (That API supports multi-platform shared-memory parallel programming in C/C++, Fortran and defines a portable, scalable model with a simple and flexible interface for developing parallel applications on platforms from the desktop to the supercomputer.)
        
        
        For the distributed memory parallelization, we employed the MPI. The structure of the algorithm, the data distribution, synchronization, and communication schemes are explained in detail. Extensive experiments for the developed algorithms were carried out. 
        
