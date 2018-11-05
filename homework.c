#include "homework.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

int num_threads;
int resize_factor;

void readInput(const char * fileName, image *img) {

  FILE *myFile = fopen(fileName, "r");
  fscanf(myFile,"%s\n",img->type);
  fscanf(myFile,"%d\n",&img->width);
  fscanf(myFile,"%d\n",&img->height);
  fscanf(myFile,"%d\n",&img->maxVal);

  int option = 0;
  if(img->type[1] == '5')
    option = 1;
  else
    option = 3;

  img->matrix = (unsigned char**)calloc(img->height, sizeof(unsigned char*));
  int i;
  for(i = 0; i < img->height; i++)
    img->matrix[i] = (unsigned char*)calloc(option * img->width, sizeof(unsigned char));

  for(i = 0; i < img->height; i++)
    fread(img->matrix[i], option * img->width, 1, myFile);

  fclose(myFile);
}

void writeData(const char * fileName, image *img) {

  FILE *myFile = fopen(fileName,"wb");

  fprintf(myFile, "%s\n", img->type);
  fprintf(myFile, "%d ", img->width);
  fprintf(myFile, "%d\n", img->height);
  fprintf(myFile, "%d\n", img->maxVal);

  int option = 0;
  if(img->type[1] == '5')
    option = 1;
  else
    option = 3;

  int i;
  for(i = 0; i < img->height; i++)
    fwrite(img->matrix[i], option * img->width, 1, myFile);

  fclose(myFile);
}

void resize(image *in, image *out) {

  strcpy(out->type,in->type);
  out->width = in->width / resize_factor;
  out->height = in->height / resize_factor;
  out->maxVal = in->maxVal;

  int option = 0;
  if(in->type[1] == '5')
    option = 1;
  else
    option = 3;

  out->matrix = (unsigned char**)calloc(out->height, sizeof(unsigned char*));
  int i;
  for(i = 0; i < out->height; i++)
    out->matrix[i] = (unsigned char*)calloc(option * out->width, sizeof(unsigned char));


  trd threadImg[num_threads];
  pthread_t tid[num_threads];

	for(i = 0;i < num_threads; i++){
     threadImg[i].in = in;
     threadImg[i].out = out;
     threadImg[i].threadID = i;
  }

	for(i = 0; i < num_threads; i++) {
		pthread_create(&(tid[i]), NULL, threadFunction, &(threadImg[i]));
	}

	for(i = 0; i < num_threads; i++) {
		pthread_join(tid[i], NULL);
	}
}

void* threadFunction(void *var) {
  trd threadImg = *(trd *) var;

  int id = threadImg.threadID;
  image *in = threadImg.in;
  image *out = threadImg.out;

  int start,end,begin,finish;

  start = id * (out->height / num_threads) * resize_factor;
  end = (id + 1) * (out->height / num_threads) * resize_factor;

  begin = id * (out->height / num_threads);
  finish = (id + 1) * (out->height / num_threads);

  if (threadImg.threadID == num_threads - 1){
    end = in->height;
  }

  int option = 0;
  if(in->type[1] == '5')
    option = 1;
  else
    option = 3;

  int myNewWidth = option * in->width;

  int i,j;
  int m, n, k = 0, l = 0;
  int nr ;
  int sum1 = 0, sum2 = 0, sum3 = 0, sum = 0;

  if(in->type[1] == '6'){
    if(resize_factor == 3){
      nr = 16;
      k = begin;
      for(i = start ; i + resize_factor <= end; i = i + 3){
        for(j = 0; j + resize_factor <= myNewWidth; j = j + 9){

          sum1 = 0;
          sum2 = 0;
          sum3 = 0;

          sum1 = sum1 + in->matrix[i][j] + in->matrix[i][j+3] * 2 + in->matrix[i][j+6]
               + in->matrix[i+1][j] * 2 + in->matrix[i+1][j+3] * 4 + in->matrix[i+1][j+6] * 2
               + in->matrix[i+2][j] + in->matrix[i+2][j+3] * 2 + in->matrix[i+2][j+6];
          sum2 = sum2 + in->matrix[i][j+1] + in->matrix[i][j+4] * 2 + in->matrix[i][j+7]
               + in->matrix[i+1][j+1] * 2 + in->matrix[i+1][j+4] * 4 + in->matrix[i+1][j+7] * 2
               + in->matrix[i+2][j+1] + in->matrix[i+2][j+4] * 2 + in->matrix[i+2][j+7];
          sum3 = sum3 + in->matrix[i][j+2] + in->matrix[i][j+5] * 2 + in->matrix[i][j+8]
               + in->matrix[i+1][j+2] * 2 + in->matrix[i+1][j+5] * 4 + in->matrix[i+1][j+8] * 2
               + in->matrix[i+2][j+2] + in->matrix[i+2][j+5] * 2 + in->matrix[i+2][j+8];

          out->matrix[k][l] = sum1/nr;
          out->matrix[k][l+1] = sum2/nr;
          out->matrix[k][l+2] = sum3/nr;
          l += 3;
        }
        k++;
        l = 0;
      }

    }else{

      nr = resize_factor * resize_factor;
      k = begin;
      for(i = start ; i  + resize_factor <= end; i = i + resize_factor){
        for(j = 0; j + resize_factor <= myNewWidth; j = j + resize_factor * 3){
          sum1 = 0;
          sum2 = 0;
          sum3 = 0;
          for(n = i; n < i + resize_factor; n++){
            for(m = j; m < j + resize_factor * 3; m = m + 3){
              sum1 = sum1 + in->matrix[n][m];
              sum2 = sum2 + in->matrix[n][m+1];
              sum3 = sum3 + in->matrix[n][m+2];
            }
          }
          out->matrix[k][l] = sum1/nr;
          out->matrix[k][l+1] = sum2/nr;
          out->matrix[k][l+2] = sum3/nr;
          l+=3;
        }
        k++;
        l = 0;
      }
    }
  }

  j = 0, m = 0, n = 0;
  sum = 0, k = 0, l = 0;

  if(in->type[1] == '5'){
    if(resize_factor == 3){
      k = begin;
      nr = 16;
      for(i = start ; i  + resize_factor <= end; i = i + 3){
        for(j = 0; j  + resize_factor <= myNewWidth; j = j + 3){
          sum = 0;
          sum = sum + in->matrix[i][j] + in->matrix[i][j+1] * 2 + in->matrix[i][j+2]
              + in->matrix[i+1][j] * 2 + in->matrix[i+1][j+1] * 4 + in->matrix[i+1][j+2] * 2
              + in->matrix[i+2][j] + in->matrix[i+2][j+1] * 2 + in->matrix[i+2][j+2];
          out->matrix[k][l] = sum/nr;
          l++;
        }
        k++;
        l = 0;
      }

    }else {
      k = begin;
      nr = resize_factor * resize_factor;
      for(i = start ; i  + resize_factor <= end; i = i + resize_factor){
        for(j = 0; j  + resize_factor <= myNewWidth; j = j + resize_factor){
          sum = 0;
          for(n = i; n < i + resize_factor; n++){
            for(m = j; m < j + resize_factor; m++){
              sum = sum + in->matrix[n][m];
            }
          }
          out->matrix[k][l] = sum/nr;
          l++;
        }
        k++;
        l = 0;
      }
    }
  }

  return NULL;

}
