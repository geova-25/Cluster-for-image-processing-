// Author: Wes Kendall
// Copyright 2011 www.mpitutorial.com
// This code is provided freely with the tutorials on mpitutorial.com. Feel
// free to modify it for your own use. Any distribution of the code must
// either provide a link to www.mpitutorial.com or keep this header intact.
//
// Example using MPI_Send and MPI_Recv to pass a message around in a ring.
//
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  // Initialize the MPI environment
  MPI_Init(NULL, NULL);
  // Find out rank, size
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);



  //------------------------------------Abrir archivo
  FILE *fp;
  FILE *fp1;
  int filelen;
  fp=fopen("cat_400_400.data", "rb");
  if(fp == NULL) printf("Error Opening original file\n");
  else
  {
    fseek (fp, 0, SEEK_END);
    filelen = ftell(fp);
    printf("size: %d\n", filelen);
    fseek (fp, 0, SEEK_SET);
  }

  char* buffer = (char *)calloc(filelen, sizeof(char));
  fread(buffer, filelen, 1, fp); // Read in the entire file


  for (int i = 0; i < filelen; i++) {
    buffer[i] = buffer[i] + 1;
  }



  //----------------------------------Fin de abrir archivo
  //------------------------------------Distribute Image
  int number_of_bytes_per_process = 0;
  number_of_bytes_per_process = filelen/world_size;
  //printf("number_of_bytes_per_process: %d\n",number_of_bytes_per_process);
  unsigned char** bytes_per_process = (unsigned char**)calloc(world_size,sizeof(char*));
  for (int i = 0; i < world_size ; i++) {
    bytes_per_process[i] = (unsigned char*)calloc(number_of_bytes_per_process,sizeof(unsigned char));
    printf("i: %d\n",i);
    printf(" i * number_of_bytes_per_process: %d\n",i* number_of_bytes_per_process);
    int jAux = 0;
    for (int j = i * number_of_bytes_per_process; j < (i+1)*number_of_bytes_per_process; j++) {
      bytes_per_process[i][jAux] =  buffer[j];
      jAux = jAux + 1;
    }
  }
  //------------------------------------Execute Filter

  for (int i = 0; i < world_size; i++) {
    for (int j = 0; j < number_of_bytes_per_process; j++) {
        bytes_per_process[i][j] = bytes_per_process[i][j] + 200;
    }
  }

  //------------------------------------End of Execute Filter
  //------------------------------------Message logic
  //-----To take into account

  // bytes_per_process = Array of pointers to unsigned char pointers of each process
  // number_of_bytes_per_process = Quantity of bytes for each proccess


  // Receive from the lower process and send to the higher process. Take care
  // of the special case when you are the first process to prevent deadlock.
  if (world_rank != 0)
  {
    MPI_Send(bytes_per_process[world_rank], number_of_bytes_per_process, MPI_BYTE, 0, 0,MPI_COMM_WORLD);
  }
  else
  {
    unsigned char* finalResult = (unsigned char*)calloc(filelen,sizeof(unsigned char));
    unsigned char* temporary_bytes_received = (unsigned char*)calloc(number_of_bytes_per_process,sizeof(unsigned char));
    for (int i = 0; i < number_of_bytes_per_process; i++)
    {
      finalResult[i] = bytes_per_process[0][i];
    }
    for (int i = 1; i < world_size; i++)
    {
      MPI_Recv(temporary_bytes_received, number_of_bytes_per_process, MPI_BYTE, i, 0, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);
      printf("Process %d received data %d from process %d\n", world_rank,0, i);
      for (int j = 0; j < number_of_bytes_per_process; j++)
      {
        finalResult[i*number_of_bytes_per_process + j] = temporary_bytes_received[j];
      }
    }
    fp1 = fopen("cat_result_2.data", "wb");
    if(fp1 == NULL) printf("Error Opening Final file\n");
    else
    {
      fwrite(finalResult,1,filelen,fp1);
    }

    fclose(fp);
    fclose(fp1);
  }
  MPI_Finalize();
}
