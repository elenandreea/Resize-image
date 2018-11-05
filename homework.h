#ifndef HOMEWORK_H
#define HOMEWORK_H

typedef struct {
  char type[4];
  int width;
  int height;
  int maxVal;
  unsigned char **matrix;
}image;

typedef struct {
  int threadID;
  image *in;
  image *out;
}trd;

void readInput(const char * fileName, image *img);

void writeData(const char * fileName, image *img);

void resize(image *in, image * out);

void* threadFunction(void *var);

#endif /* HOMEWORK_H */
