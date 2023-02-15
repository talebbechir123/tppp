#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <mpi.h>


int main(){
    MPI_Init(NULL, NULL);
    int rank;
    int world;
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world);
   
    int step = 1;

    while(step<world){
      
            printf("step %d \n",step);
            if(rank%(step*2) ==0){
                printf(" proc number %d recivng tab from proc number %d \n",rank,rank-1);
            }else {
                
             printf(" proc number %d sending tab to proc number %d \n",rank,rank+1);
            }
        
        

        step=step*2;
    }


      



MPI_Finalize();
}