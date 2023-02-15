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
    int des;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world);
    if(rank==0){
        printf(" root ");
    }
    for(int i=1;i<=rank;i++){
        printf(" step : %d \n",i);
       if(i%2==0){
         if(rank%2==0){
            des = rank +1 ;
        }else {
            des= rank -1;
        }

       }else{
        if(rank%2==0){
            des = rank -1 ;
        }else {
            des= rank +1;
        }

       }
       if (des >= 0&& des < world){
          printf("My rank is %d  and my des is  %d",rank, des);

       }
    }


     MPI_Finalize();
}