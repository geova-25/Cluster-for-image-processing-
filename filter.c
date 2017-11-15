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
#include "filtro.c"

#define HEIGTH 400
#define WEIGHT 400

//-----------------------Esta es la funcion que es llamada para aplicar el filtro
//-----------------------Aqui va lo tuyo will jaja

char* filterBla(unsigned char* image_bytes, int sizeOfBufferOfBytesToFilter)
{
  for (int j = 0; j < sizeOfBufferOfBytesToFilter; j++)
  {
    image_bytes[j] = 255 - image_bytes[j];
  }
}

//------------------------------------------------------------------------------

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




  //----------------------------------Fin de abrir archivo
  //------------------------------------Distribute Image
  int number_of_bytes_per_process = 0;
  int number_of_rows = 0;
  int number_of_bytes_per_row = 0;
  int number_of_rows_per_process = 0;
  int total_bytes = 0;

  number_of_bytes_per_process = filelen/world_size;
  number_of_rows = HEIGTH;
  number_of_rows_per_process = number_of_rows/world_size;
  number_of_bytes_per_row = number_of_bytes_per_process/number_of_rows_per_process;
  total_bytes = HEIGTH * WEIGHT;


  unsigned char* bytes_of_this_process = (unsigned char*)calloc(number_of_rows_per_process * number_of_bytes_per_row,sizeof(char));
  int iAux = 0;
  for (int j = world_rank * number_of_rows_per_process * number_of_bytes_per_row; j < (world_rank+1)*number_of_rows_per_process * number_of_bytes_per_row; j++) {
    bytes_of_this_process[iAux]=  buffer[j];
    iAux = iAux + 1;
  }
  //------------------------------------Here we execute the filter of the image

  filterBla(bytes_of_this_process,  number_of_rows_per_process * number_of_bytes_per_row);

  //------------------------------------End of Execute Filter


  //------------------------------------Message intercomunication logic of MPI


  //-----To take into account

  // bytes_of_this_process = Array of pointers to unsigned char pointers of each process
  // number_of_bytes_of_this_process = Quantity of bytes for each proccess

  //-If the process is not the cero process
  if (world_rank != 0)
  {
    //-Then it sends to the process cero the part of the image that he processes
    MPI_Send(bytes_of_this_process,  number_of_rows_per_process * number_of_bytes_per_row, MPI_BYTE, 0, 0,MPI_COMM_WORLD);
  }
  //-If the process indeed is the one with id 0
  else
  {
    //-Here the final result of the image will be stored
    unsigned char* finalResult = (unsigned char*)calloc(filelen,sizeof(unsigned char));
    //-This is the buffer that receives the data send by each process
    unsigned char* temporary_bytes_received = (unsigned char*)calloc(number_of_bytes_per_process,sizeof(unsigned char));
    //-This loop saves the processing of the part of the image of process id 0
    for (int i = 0; i < number_of_bytes_per_process; i++)
    {
      finalResult[i] = bytes_of_this_process[i];
    }

    //-This loop waits in order for the data of the other processes from the 1 to the n process that sends data

    for (int i = 1; i < world_size; i++)
    {
      //-Wait for the data of the first process
      MPI_Recv(temporary_bytes_received,  number_of_rows_per_process * number_of_bytes_per_row, MPI_BYTE, i, 0, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);
      printf("Process %d received data %d from process %d\n", world_rank,0, i);
      //-This loop saves the data of the actual process into the final image
      for (int j = 0; j <  number_of_rows_per_process * number_of_bytes_per_row; j++)
      {
        finalResult[i* number_of_rows_per_process * number_of_bytes_per_row + j] = temporary_bytes_received[j];
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
