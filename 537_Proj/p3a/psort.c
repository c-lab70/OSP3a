#include <stdio.h>
#include <sys/sysinfo.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>


//stores the key and bytes (using mmap pointer) for each line.
//structs are all in an array that will be divided by thread and sorted
struct fline {
  int32_t key;
  char *bytes;
};

typedef struct{
  struct fline *a;
  int b;
  int c;
} myargs_t;

//Quicksort implementation for project adapted from https://www.programiz.com/dsa/quick-sort

// function to swap elements
void swap(struct fline *a, struct fline *b) {
  struct fline t = *a;
  *a = *b;
  *b = t;
}

// function to find the partition position
int partition(struct fline *array, int low, int high) {
  // select the rightmost element as pivot
  int32_t pivot = array[high].key;

  // pointer for greater element
  int i = (low - 1);

  // traverse each element of the array
  // compare them with the pivot
  for (int j = low; j < high; j++) {
    if (array[j].key <= pivot) {

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

void quickSort(struct fline *array, int low, int high) {
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

void *quickSortThread(void *args){
  myargs_t *qsArgs = (myargs_t *) args;
  quickSort(qsArgs->a, qsArgs->b, qsArgs->c);
  void* a = NULL;
  return a;
}

int main(int argc, char *argv[]) {

int fcode = open(argv[1], O_RDONLY);

struct stat st;
stat(argv[1], &st);
int fsize = st.st_size; //https://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c

int num_cpu = get_nprocs(); //gets number of cpus availible

//map the file bytes to memory. That way, no need to copy to a char array and 
//write back. Just move to the output using pointer arithmetic.
//https://man7.org/linux/man-pages/man2/mmap.2.html
char* file_start = mmap(NULL, fsize, PROT_READ, MAP_SHARED, fcode, 0); //https://linuxhint.com/using_mmap_function_linux/#:~:text=The%20mmap()%20function%20is,an%20array%20in%20the%20program.

if (fcode < 0) { //no file, fail
  fprintf(stderr, "An error has occured\n");
  exit(0);
}

if (fsize == 0) { //empty file, fail
  fprintf(stderr, "An error has occured\n");
  exit(0);
}

int num_lines = (fsize + 1) / 100; //number of lines

int num_lines_thread = num_lines / num_cpu; //number of lines assigned to each thread
if(num_lines < num_cpu){
  num_lines_thread = num_lines;
  num_cpu = 1;
}
else{
  num_lines_thread = num_lines / num_cpu; //number of lines assigned to each thread
}

int remainder = num_lines % num_cpu;
//int lines_last_thread = num_lines_thread + remainder;

struct fline* line_arr = malloc(num_lines * sizeof(struct fline));

for (int i = 0; i < num_lines; i++) {

    char key[5]; 
    for(int j = 0; j < 4; j++){
        key[j] = *(file_start + j + i*100);

    }
    key[4] = '\0';
    
    int32_t myInt1 = key[3] + (key[2] << 8) + (key[1] << 16) + (key[0] << 24);

    line_arr[i].key = myInt1;
    //printf("Key %d : %d\n", i, line_arr[i].key);

  line_arr[i].bytes = (file_start + i*100 + 4); //correct or not????
}
//using one thread
// pthread_t thread;
// myargs_t args = {line_arr, 0, num_lines - 1};
// pthread_create(&thread, NULL, quickSortThread, &args);
// void** a;
// pthread_join(thread, a);

if(num_lines / num_cpu > 1){
  pthread_t thread_arr[num_cpu];

  for(int i = 0; i < num_cpu; i++){
  
    if(i == num_cpu -1){
      myargs_t args = {line_arr, i * num_lines_thread, (i + 1) * num_lines_thread + (remainder - 1)};
      pthread_create(&thread_arr[i], NULL, quickSortThread, &args);
    }
    else{
      myargs_t args = {line_arr, i * num_lines_thread, (i + 1) * num_lines_thread - 1};
      pthread_create(&thread_arr[i], NULL, quickSortThread, &args);
    }
  }

  for(int i = 0; i < num_cpu; i++){
    void** a = NULL;
    pthread_join(thread_arr[i], a);
  }

}

quickSort(line_arr, 0, num_lines - 1);


///////////////////////////////////// Write-Out

int fptr2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);

if(fptr2 == -1){
    perror("open");
    exit(1);
}


//char nl = '\0';
// for(int i = 0; i < num_lines; i++) {

//     char key[4]; 
//     int n = line_arr[i].key;
//     key[0] = (n >> 24) & 0xFF;
//     key[1] = (n >> 16) & 0xFF;
//     key[2] = (n >> 8) & 0xFF;
//     key[3] = n & 0xFF;

    
//     //printf("Key %d : %d\n", i, line_arr[i].key);
//     ssize_t size = write(fptr2, key, 4);
//     ssize_t size_1 = write(fptr2, line_arr[i].bytes, 96);
//     if (size <= 0 || size_1  <= 0) {
//       printf("FAIL");
//       exit(0);
//     }
// }


fsync(fptr2);

close(fcode);
close(fptr2);
return 0;
}

/* Notes:
do not need to consider reallocating work if one thread finishes first
thread the read in and the sorting using lines/cpus 
final merge of all the sorted threads
define method that compares keys of structs in order to sort them
https://stackoverflow.com/questions/41346061/parallel-sorting-in-quick-sort-using-multi-threads-in-c
https://www.geeksforgeeks.org/quick-sort-using-multi-threading/#:~:text=QuickSort%20is%20a%20popular%20sorting,array%20is%20partitioned%20around%20it.
*/