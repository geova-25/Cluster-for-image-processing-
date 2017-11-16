#include <stdio.h>
#include <stdint.h>

int mascara[5][5] = {
          { 1,  4,  7,  4, 1} ,
          { 4, 16, 26, 16, 4} ,
          { 7, 26, 41, 26, 7} ,
          { 4, 16, 26, 16, 4} ,
          { 1,  4,  7,  4, 1}};

int sumTotal = 213;


void filter(int h, int w, char img[h][w], int imgf[h][w])
{
  int maxh = h - 2;
  int maxw = w - 2;
  int result;
  int i;
  int j;
  int m = 2,n = 2;

  for (i = 0; i < h; i++)
  {
    for (j = 0; j < w; j++)
    {
      imgf[i][j] = img[i][j];
    }
  }


  for (i = 2; i < maxh; i++)
  {
    for (j = 2; j < maxw; j++)
    {

    result = 1*img[i-2][j-2] +  4*img[i-2][j-1] + 7*img[i-2][j]  +  4*img[i-2][j+1] + 1*img[i-2][j+2]
    		+4*img[i-1][j-2] + 16*img[i-1][j-1] + 26*img[i-1][j] + 16*img[i-1][j+1] + 4*img[i-1][j+2]
    		+7*img[i][j-2]   + 26*img[i][j-1]   + 41*img[i][j]   + 26*img[i][j+1]   + 7*img[i][j+2]
 			+4*img[i+1][j-2] + 16*img[i+1][j-1] + 26*img[i+1][j] + 16*img[i+1][j+1] + 4*img[i+1][j+2]
    		+1*img[i+2][j-2] +  4*img[i+2][j-1] + 7*img[i+2][j]  +  4*img[i+2][j+1] + 1*img[i+2][j+2];
    result = result/273;

    //printf("el valor %d\n", result+128);

    imgf[i][j] = result;


    }
  }


  //printf("%d   ", imgf[3][3] + 128);
}


/**
 * Función principal, aquí se colocan los valores de height h y width w
 * Abre el archivo, lo filtra, mapea y genera un nuevo archivo con el resultado
**/
/*
int main()
{

  int h = 1024;
  int w = 1024;

  FILE *fptr;
  fptr = fopen("prueba.data","rb"); //Se abre el archivo original

  if(fptr == NULL)
  {
    printf("Error abriendo archivo");
    return 1;
  }

  char num[h][w];


  fread(num, sizeof(num), 1, fptr);



  int i;
  int j;



  fclose(fptr);


  int imgf[h][w];
  filter(h,w, num,imgf);				//Se filtra la matriz




  fptr = fopen("img.data","wb");//Se genera el archivo modificado

  if(fptr == NULL)
  {
    printf("Error abriendo archivo");
    return 1;
  }

  //char numf[h-2][w-2];
  char numf[h][w];

  for (i = 0; i < h; i++)
  {
    for (j = 0; j < w; j++)
    {
      numf[i][j] = imgf[i][j];
    }
  }

  fwrite(numf, sizeof(numf), 1, fptr);			//Se guarda el archivo modificado

  return 0;
}*/
