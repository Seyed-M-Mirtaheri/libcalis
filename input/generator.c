#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

void makeFile(int size);

/**
 * This is a simple function to generate files with random data in them. 
 * Contents of thse files are later used by the tester application.
 */
int main(int argc, char *argv[])
{
	system("exec rm data/*");

	int min_power = 3;
	int max_power = 20;
	int base = 2;

	for (int power = min_power; power <= max_power; power++)
		makeFile((int) pow((double) base, power));

	return(0);
}

void makeFile(int size)
{
   	char file_name[100];
	sprintf(file_name, "data/%d", size);

	FILE *input_file = fopen(file_name, "w");

	if (input_file == NULL) {
		printf("Error opening file!\n");
		exit(1);
	}

	int i;
	for(i = 0; i < size; i++) {
		fprintf(input_file, "%d\n", rand());
	}

	fclose(input_file);
}
