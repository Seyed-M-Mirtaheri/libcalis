#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<inttypes.h>
#include <math.h>

#define MIN_POWER 10
#define MAX_POWER 18
#define BASE 2
#define NUM_TRIALS 3

void run_benchmarks();
void getData(int* data, int size);

void textbook_merge_sort(int arr[],int low,int high);
void textbook_merge(int arr[],int low,int mid,int high);

void iterative_merge_sort(int arr[], int low, int high);
void iterative_merge( int a[], int b[], int c[], int m, int n);

void run_iterative_coarse();
void iterative_coarse_merge_sort(int arr[], int low, int high);
void _iterative_coarse_merge_sort(int arr[], int low, int high, int granularity_power);
void iterative_coarse_merge( int a[], int b[], int c[], int m, int n);

void iterative_coarse_local_merge_sort(int arr[], int low, int high);

void insertion_sort(int arr[], int low, int high);

int main() 
{
  run_benchmarks();
  // run_iterative_coarse();

  return 0;
}

void run_benchmarks()
{
  int results[4][MAX_POWER - MIN_POWER + 1];
  FILE *result_file;

  int i, j, k, l;
  int* data;
  int size;
  clock_t start, end;
  clock_t cpu_clocks_used;

  int min_time;
  int new_time;

  void (*sorter_ptr_array[4])(int[], int, int);

  sorter_ptr_array[0] = &textbook_merge_sort;
  sorter_ptr_array[1] = &iterative_merge_sort;
  sorter_ptr_array[2] = &iterative_coarse_merge_sort;
  sorter_ptr_array[3] = &iterative_coarse_local_merge_sort;

  for(k = 0; k < 4; k++) {

    for (i = MIN_POWER; i <= MAX_POWER; i++) {
			
      size = (int)pow((double) BASE, i);
			
      for (j = 0; j < NUM_TRIALS; j++) {
				
	data = malloc(sizeof(int) * size);
	getData(data, size);
				
	start = clock();

	sorter_ptr_array[k](data, 0, size - 1);
				
	end = clock();
				
	cpu_clocks_used = end - start;
				
	new_time = (int) ((cpu_clocks_used * 1000000) / CLOCKS_PER_SEC);
				
	if (j == 0)
	  min_time = new_time;
	else 
	  min_time = (min_time > new_time) ? new_time : min_time;

	free(data);
      }
			
      results[k][i - MIN_POWER] = min_time;

      printf("%d\t%d\n", i, min_time);
    }
  }

  result_file = fopen("results/benchmark", "w");

  fprintf(result_file, "ticks textbook iterative granular local\n");

  for (i = MIN_POWER; i <= MAX_POWER; i++) {
		
    fprintf(result_file, "%d", i - MIN_POWER + 1);
		
    for(k = 0; k < 4; k++) 
      fprintf(result_file, " %d", results[k][i - MIN_POWER]);
		
    fprintf(result_file, "\n");
  }
  fclose(result_file);

  result_file = fopen("results/benchmark_no_textbook", "w");

  fprintf(result_file, "ticks iterative granular local\n");

  for (i = MIN_POWER; i <= MAX_POWER; i++) {
		
    fprintf(result_file, "%d", i - MIN_POWER + 1);
		
    for(k = 1; k < 4; k++) 
      fprintf(result_file, " %d", results[k][i - MIN_POWER]);
		
    fprintf(result_file, "\n");
  }
  fclose(result_file);


}

void getData(int* data, int size)
{
  int i;
  char file_name[100];
  FILE * input_file_pointer;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;

  if (data == NULL) {
    printf("Error allocating memory!\n");
    exit(0);
  }

  sprintf(file_name, "input/data/%d", size);

  input_file_pointer = fopen(file_name, "r");
  if (input_file_pointer == NULL)
    exit(EXIT_FAILURE);

  i = 0;
  while ((read = getline(&line, &len, input_file_pointer)) != -1)
    data[i++] = atoi(line);

  if (line)
    free(line);
}

///////////////////////////////////////////////////////////
/////////////////// TEXTBOOK IMPL /////////////////////////
///////////////////////////////////////////////////////////

void textbook_merge_sort(int arr[],int low,int high){

  int mid;

  if(low<high){
    mid=(low+high)/2;
    textbook_merge_sort(arr,low,mid);
    textbook_merge_sort(arr,mid+1,high);
    textbook_merge(arr,low,mid,high);
  }
}

void textbook_merge(int arr[],int low,int mid,int high)
{	
  int i,m,k,l;
  int* temp;

  temp = malloc(sizeof(int) * (high));

  if (temp == NULL) {
    printf("Error allocating memory!\n");
    exit(0);
  }

  l=low;
  i=low;
  m=mid+1;
	
  while((l <= mid) && (m <= high)){
		
    if(arr[l] <= arr[m]){
      temp[i] = arr[l];
      l++;
    }else{
      temp[i] = arr[m];
      m++;
    }
    i++;
  }

  if(l>mid){
    for(k = m; k <= high; k++){
      temp[i] = arr[k];
      i++;
    }
  }
  else{
    for(k=l; k<=mid; k++){
      temp[i] = arr[k];
      i++;
    }
  }
   
  for(k=low; k<=high; k++){
    arr[k] = temp[k];
  }

  free(temp);
}

///////////////////////////////////////////////////////////
/////////////////// ITERATIVE IMPL ////////////////////////
///////////////////////////////////////////////////////////

/**
 * Note: this implementation only works if size of the array is
 * a power of two. 
 */ 
void iterative_merge_sort(int arr[], int low, int high)
{
  int j, k, m, *w;
	
  int n = high+1;

  w = calloc(n, sizeof(int));

  for( k = 1; k < n; k *= 2 ) {

    for( j = 0; j < (n - k); j += 2 * k )
      iterative_merge(arr + j, arr + j + k, w + j, k, k);
		
    for ( j = 0; j < n; j++) 
      arr[j] = w[j];
  }
	
  free(w);
}

void iterative_merge( int a[], int b[], int c[], int m, int n )
{
  int i = 0, j = 0, k = 0;
  while (i < m && j < n) {
    if( a[i] < b[j] )
      c[k++] = a[i++];
    else
      c[k++] = b[j++];
  }
	
  while ( i < m )
    c[k++] = a[i++];
	 
  while ( j < n )
    c[k++] = b[j++];
}

///////////////////////////////////////////////////////////
////////////// ITERATIVE GRANULAR IMPL ////////////////////
///////////////////////////////////////////////////////////

void run_iterative_coarse()
{
  int i, j, k;
  int* data;
  int size;
  clock_t start, end;
  clock_t cpu_clocks_used;

  int min_time;
  int new_time;

  size = (int)pow((double) BASE, MAX_POWER);
			
  for (int granularity_power = 1; granularity_power < 10; granularity_power++) {
	
    for (j = 0; j < NUM_TRIALS; j++) {
				
      data = malloc(sizeof(int) * size);
      getData(data, size);
		
      start = clock();
      _iterative_coarse_merge_sort(data, 0, size - 1, granularity_power);
      end = clock();
		
      cpu_clocks_used = end - start;
			
      new_time = (int) ((cpu_clocks_used * 1000000) / CLOCKS_PER_SEC);
			
      if (j == 0)
	min_time = new_time;
      else 
	min_time = (min_time > new_time) ? new_time : min_time;
			
      free(data);
    }
		
    printf("%d\t%d\n", granularity_power, min_time);
  }
}

void iterative_coarse_merge_sort(int arr[], int low, int high)
{
  _iterative_coarse_merge_sort(arr, low, high, 4);
}


inline void _iterative_coarse_merge_sort(int arr[], int low, int high, int granularity_power)
{
  int j, k, m, *w;
	
  int n = high+1;

  int granularity = BASE << (granularity_power-1);

  w = calloc(n, sizeof(int));
	
  for( k = 0; k < n; k += granularity ) {
    insertion_sort(arr, k, k + granularity - 1);
  }

  for( k = granularity; k < n; k *= 2 ) {

    for( j = 0; j < (n - k); j += 2 * k )
      iterative_coarse_merge(arr + j, arr + j + k, w + j, k, k);
		
    for ( j = 0; j < n; j++) 
      arr[j] = w[j];
  }

  free(w);
}

void iterative_coarse_merge( int a[], int b[], int c[], int m, int n )
{
  int i = 0, j = 0, k = 0;
  while (i < m && j < n) {
    if( a[i] < b[j] )
      c[k++] = a[i++];
    else
      c[k++] = b[j++];
  }
	
  while ( i < m )
    c[k++] = a[i++];
	 
  while ( j < n )
    c[k++] = b[j++];
}

///////////////////////////////////////////////////////////
///////// ITERATIVE GRANULAR LOCAL IMPL ///////////////////
///////////////////////////////////////////////////////////

void iterative_coarse_local_merge_sort(int arr[], int low, int high)
{
  int j, k, m, *w;
	
  int n = high+1;

  int granularity = BASE << 3;

  w = calloc(n, sizeof(int));
	
  for( k = 0; k < n; k += granularity ) {
    insertion_sort(arr, k, k + granularity - 1);
  }

  int num_units = n >> 4;
  int k_shadow;
  int power;
  int temp;

  for (k = 1; k < num_units; k += 2) {
		
    iterative_coarse_merge(
			   arr + ( (k - 1) * granularity), 
			   arr + ( k * granularity), 
			   w, 
			   granularity, 
			   granularity);
		
    temp = ((k-1)*granularity);
    for ( j = 0; j < granularity*2; j++) 
      arr[j + temp] = w[j];

    k_shadow = k >> 1;
    power = 4;

    while (k_shadow & 1) {
			
      iterative_coarse_merge(
			     arr + ((k - power + 1) * granularity), 
			     arr + ((k - (power >> 1) + 1  ) * granularity), 
			     w, 
			     granularity * (power >> 1), 
			     granularity * (power >> 1));
		
      temp = ((k - power + 1) * granularity);
      for ( j = 0; j < granularity * power; j++) 
	arr[j + temp] = w[j];

      power = power << 1;
      k_shadow = k_shadow >> 1;
    }
  }



  /*

    for( k = granularity; k < n; k *= 2 ) {

    for( j = 0; j < (n - k); j += 2 * k )
    iterative_coarse_merge(arr + j, arr + j + k, w + j, k, k);
		
    for ( j = 0; j < n; j++) 
    arr[j] = w[j];
    }

    free(w);
  */
}

///////////////////////////////////////////////////////////
////////////////////// INSERTION IMPL /////////////////////
///////////////////////////////////////////////////////////

inline void insertion_sort(int arr[], int low, int high){
  int d, t;

  for (int i = low + 1 ; i <= high; i++) {
    d = i;
 
    while ( d > low && arr[d] < arr[d-1]) {
      t = arr[d];
      arr[d] = arr[d-1];
      arr[d-1] = t;
 
      d--;
    }
  }
 
}
