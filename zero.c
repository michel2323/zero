#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <mpi.h>
#include "zero_compression.h"

double * generate_nzeros(double *res,int n, int pzeros) {
  int i=0;
  int r=0;
  printf("Generating %d doubles of data.\n", n);
  if(pzeros < 0 || pzeros > 100) {
    printf("Wrong bounds for 0 <= pzeros=%d <=100.\n", pzeros); 
    exit(1);
  }
#pragma omp parallel for
  for(i=0;i<n;i=i+1) {
    int seed;
    r = rand_r(&seed)%100;
    if(r<pzeros) {
      res[i]=0;
    } else {
      res[i]=(double) r;
    }
  }
}

void output(double *data, int size) {
  int i=0;
  for(i=0;i<size;i=i+1) {
    printf("%f ", data[i]);
  }
  printf("\n");
}
void diff(double *data1, double *data2, int size) {
  int i=0;
  int diff=0;
  int cdiff=0;
  for(i=0;i<size;i=i+1) {
    if(data1[i]!=data2[i]) {
      diff=1;
      cdiff=cdiff+1;
    }
  }
  if(diff==0) {
    printf("The data fields do match!\n");
  } else {
    printf("The data fields are different %d times.\n",cdiff);
  }
}

int main(int argc, char * argv[]) {
  MPI_Init(&argc,&argv);
  int ierr;
  int pzeros=0;
  int n=0;
  int rank=0;
  if(argc==1) {
    n=134217728;
  }
  else {
    n=atoi(argv[1]);
  }

  int m=0;
  double *data=malloc(sizeof(double)*n);
  double *ddata=malloc(sizeof(double)*n);
  char *cdata=malloc(sizeof(double)*n);

  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

  for(pzeros=10;pzeros<=100;pzeros=pzeros+10) {
    MPI_Barrier(MPI_COMM_WORLD);
    double t0=omp_get_wtime(); 
    generate_nzeros(data,n,pzeros);
    double t1=omp_get_wtime(); 
    if(rank==0)
      printf("%f seconds for generating data\n",t1-t0);
    MPI_Barrier(MPI_COMM_WORLD);
    t0=omp_get_wtime(); 
    zero_compress_(data, &n, cdata, &m, &ierr);
    t1=omp_get_wtime(); 
    if(rank==0)
      printf("%f seconds for compressing data\n",t1-t0);
    if(m>n*8) {
      printf("Compressed data is bigger than uncompressed data! %d %d\n", n*8,m);
    }
    long int nl=(long int) n;
    MPI_Barrier(MPI_COMM_WORLD);
    t0=omp_get_wtime(); 
    zero_decompress(cdata, ddata, &nl, &ierr);
    t1=omp_get_wtime(); 
    if(rank==0)
      printf("%f seconds for decompressing data\n",t1-t0);
    diff(data,ddata,n);
  }
  free(data); free(ddata), free(cdata);
  MPI_Finalize();

}

