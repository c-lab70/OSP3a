#include <stdio.h>
#include <sys/sysinfo.h>
#include <stdlib.h>
#include <string.h>

//Quicksort implementation for project adapted from https://www.programiz.com/dsa/quick-sort

// function to swap elements
void swap(int *a, int *b) {
  int t = *a;
  *a = *b;
  *b = t;
}

// function to find the partition position
int partition(int array[], int low, int high) {

  // select the rightmost element as pivot
  int pivot = array[high];

  // pointer for greater element
  int i = (low - 1);

  // traverse each element of the array
  // compare them with the pivot
  for (int j = low; j < high; j++) {
    if (array[j] <= pivot) {

      // if element smaller than pivot is found
      // swap it with the greater element pointed by i
      i++;

      // swap element at i with element at j
      swap(&array[i], &array[j]);
    }
  }

  // swap the pivot element with the greater element at i
  swap(&array[i + 1], &array[high]);

  // return the partition point
  return (i + 1);
}

void quickSort(int array[], int low, int high) {
  if (low < high) {

    // find the pivot element such that
    // elements smaller than pivot are on left of pivot
    // elements greater than pivot are on right of pivot
    int pi = partition(array, low, high);

    // recursive call on the left of pivot
    quickSort(array, low, pi - 1);

    // recursive call on the right of pivot
    quickSort(array, pi + 1, high);
  }
}

int main(int argc, char *argv[]) {

FILE* fptr1;
char str[96];

fptr1 = fopen(argv[1], "r");

if (fptr1 == NULL) {
        return 1;
}

char c; //hey
char v;

for (int m = 0; c != EOF; m++) {

int key = *(int*)(fptr1 + i*100 + j-4);

for (i = 4; i < 100 && str[i - 1] != EOF; i++) {
        str[i] = fgetc(fptr1);
    }
}

fclose(fptr1);

return 0;
}