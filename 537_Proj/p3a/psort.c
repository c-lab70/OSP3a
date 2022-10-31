#include <stdio.h>
#include <sys/sysinfo.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

//stores the key and bytes (using mmap pointer) for each line.
//structs are all in an array that will be divided by thread and sorted
struct fline {
  int key;
  char *bytes;
};

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

FILE* fptr1 = fopen(argv[1], "r");

fseek(fptr1, 0, SEEK_END); // seek to end of file
int fsize = ftell(fptr1); // get current file pointer and consequentially size of the file in bytes
fseek(fptr1, 0, SEEK_SET); //https://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c

int num_cpu = get_nprocs(); //gets number of cpus availible

//map the file bytes to memory. That way, no need to copy to a char array and 
//write back. Just move to the output using pointer arithmetic.
//https://man7.org/linux/man-pages/man2/mmap.2.html
mmap(NULL, fsize, PROT_READ, MAP_SHARED, fptr1, 0); //https://linuxhint.com/using_mmap_function_linux/#:~:text=The%20mmap()%20function%20is,an%20array%20in%20the%20program.

if (fptr1 == NULL) { //no file, fail
  return 1;
}

if (fsize == 0) { //empty file, fail
  return 1;
}

int num_lines = fsize / 100; //number of lines

int num_lines_thread = num_lines / num_cpu; //number of lines assigned to each thread

struct fline line_arr[] = malloc(num_lines * sizeof(struct fline));

for (int i = 0; i < num_lines; i++) {
  struct fline line1;
  int j = 4;
  line1.key = *(int*)(fptr1 + i*100 + j-4);
  line1.bytes = (fptr1 + i*100 + 4); //correct or not????
}

/////////////////////////////////////OUTPUT

FILE* fptr2 = fopen(argv[2], "w");

for(int i = 0; i < num_lines; i++) {
  char *data_start = line_arr[i].bytes;
  for (int j = 4; j < 100; j++) {
    //fwrite(); //TO-DO: write data from mmap. how to do this?
  }
}

fclose(fptr1);
return 0;
}

/*
do not need to consider reallocating work if one thread finishes first

thread the read in and the sorting using lines/cpus 

final merge of all the sorted threads

define method that compares keys of structs in order to sort them

https://stackoverflow.com/questions/41346061/parallel-sorting-in-quick-sort-using-multi-threads-in-c

https://www.geeksforgeeks.org/quick-sort-using-multi-threading/#:~:text=QuickSort%20is%20a%20popular%20sorting,array%20is%20partitioned%20around%20it.
*/

//loop to write out characters.