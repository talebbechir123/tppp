#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    int taille = atoi(argv[1]);
    int nbre = atoi(argv[2]);
    MPI_Init(&argc, &argv);
    int rank;
    int world;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world);
    //exchange messge of size X with other processes
    
    int *tab = malloc(taille*sizeof(int));
    int *tab2 = malloc(taille*sizeof(int));
    double time_start, time_end, time_of_message_start, time_of_message_end;
    int i;
     int size;
    for(i=0;i<nbre;i++){
        tab[i] = rank;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    time_start = MPI_Wtime();

    MPI_Status status;
    int j=0;
    while(j< nbre){
         if(rank == 0){
            j++;
        time_of_message_start = MPI_Wtime();
        
        MPI_Send(tab, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
        MPI_Recv(tab2, 1, MPI_INT, world-1, 0, MPI_COMM_WORLD, &status);
        
        MPI_Get_count(&status, MPI_INT, &size);
       
       
    }
    else{
        time_of_message_end = MPI_Wtime();
        MPI_Recv(tab2, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, &status);
        
        MPI_Get_count(&status, MPI_INT, &size);
      
           j++;
        MPI_Send(tab, 1, MPI_INT, (rank+1)%world, 0, MPI_COMM_WORLD);
       
    }   
    }
   
    MPI_Barrier(MPI_COMM_WORLD);
    time_end = MPI_Wtime();
    //get the time and processus number
    printf("time: %f, rank: %d \n",time_end-time_start,rank);
    printf("time of message: %f, rank: %d \n",time_of_message_end-time_of_message_start,rank);
  
    MPI_Finalize();
}





