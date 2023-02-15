#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <mpi.h>

/* matrix size */
#define N 2500

// Random values are [0, MAX_VAL]
#define MAX_VAL 5

// Nomber of checks
#define NBCHECKS 10
// acceptable error (in check)
#define ERROR   1.e-3

#define DIFFTEMPS(a,b) (((b).tv_sec - (a).tv_sec) + ((b).tv_usec - (a).tv_usec)/1000000.)

/* global to avoid stack overflow */
double a[N][N],b[N][N],c[N][N];

MPI_Status status;

int main()
{
  struct timeval tv_init, tv_begin, tv_end;
  gettimeofday( &tv_init, NULL);
 
    MPI_Init(NULL, NULL);
    int rank;
    int world;
     int rows; 
    int offset;
    int source;
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world);
    
  /***************************************************************************/
  // initialization
  if(rank == 0){
    
  srand((unsigned int)time(NULL));
  for (int i=0;i<N;i++){
    for (int j=0;j<N;j++)
    {
      b[i][j]=(double)rand()/(double)(RAND_MAX/MAX_VAL);
      a[i][j]=(double)rand()/(double)(RAND_MAX/MAX_VAL);
    }
 
    }
    rows= N/(world-1); 
     offset =0;


    for(int i=1;i<=world-1;i++){
        //ack rows
        MPI_Send(&rows,1,MPI_INT,i,1,MPI_COMM_WORLD);
        //ack offset
        MPI_Send(&offset,1,MPI_INT,i,1,MPI_COMM_WORLD);
        
        // Send rows of the Matrix A which will be assigned to slave process to compute
      MPI_Send(&a[offset][0], rows*N, MPI_DOUBLE,i,1, MPI_COMM_WORLD);
      // Matrix B is sent
      MPI_Send(&b, N*N, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
      
      // Offset is modified according to number of rows sent to each process
      offset = offset + rows;
    }
      // Root process waits untill the each slave proces sent their calculated result with message tag 2
    for (int i = 1; i <= world-1; i++)
    {
      source = i;
      // Receive the offset of particular slave process
      MPI_Recv(&offset, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
      // Receive the number of rows that each slave process processed
      MPI_Recv(&rows, 1, MPI_INT, source, 2, MPI_COMM_WORLD, &status);
      // Calculated rows of the each process will be stored int Matrix C according to their offset and
      // the processed number of rows
      MPI_Recv(&c[offset][0], rows*N, MPI_DOUBLE, source, 2, MPI_COMM_WORLD, &status);
    }
    
    /***************************************************************************/
  // check some arbitrary values
  int check_ok = 1;
  for(int checks=0 ; checks<NBCHECKS ; checks++)
  {
    int i = rand()%N;
    int j = rand()%N;
    double val = 0.;
    for(int k=0 ; k<N ; k++){
      val += a[i][k] * b[k][j];}
    if(fabs(val - c[i][j]) > ERROR)
    {
      fprintf(stderr, "BAD RESULTS !");
      fprintf(stderr, " (value[%d][%d] = %g should be %g)\n",
              i, j, c[i][j], val);
      check_ok = 0;
    }
  }
  if(check_ok){
    fprintf(stderr, "Ok results :)\n");
}

  
  
  }
   if ( rank > 0){
  
   source = 0;
  
     // The slave process receives the offset value sent by root process
    MPI_Recv(&rows, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
    // The slave process receives number of rows sent by root process  
    MPI_Recv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
    // The slave process receives the sub portion of the Matrix A which assigned by Root 
    MPI_Recv(&a, rows*N, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);
    // The slave process receives the Matrix B
    MPI_Recv(&b, N*N, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);
    
    /***************************************************************************/
  // compute
  gettimeofday( &tv_begin, NULL);
for (int k = 0; k<N; k++) {
      for (int i = 0; i<rows; i++) {
        // Set initial value of the row summataion
        c[i][k] = 0.0;
        // Matrix A's element(i, j) will be multiplied with Matrix B's element(j, k)
        for (int j = 0; j<N; j++)
          c[i][k] = c[i][k] + a[i][j] * b[j][k];
      
    }}
    
    
    // Calculated result will be sent back to Root process (process 0) with message tag 2
    
    // Offset will be sent to Root, which determines the starting point of the calculated
    // value in matrix C 
    MPI_Send(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
    // Number of rows the process calculated will be sent to root process
    MPI_Send(&rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
    // Resulting matrix with calculated rows will be sent to root process
    MPI_Send(&c, rows*N, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);

  gettimeofday( &tv_end, NULL);
  
   
  
  }
   

  

  
  /***************************************************************************/
  /* execution times */
  printf("Init : %lfs, Compute : %lfs\n",
         DIFFTEMPS(tv_init,tv_begin),
         DIFFTEMPS(tv_begin,tv_end));
  MPI_Finalize();
  return( 0 );
}
