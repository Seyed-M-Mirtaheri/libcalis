all: calis merge_sort

calis: calis.h calis_example.c
	cc calis_example.c -lm -o calis_example

merge_sort: merge_sort.c
	# outputs binary; change later to output library
	cc merge_sort.c -o merge_sort

clean:
	rm -f calis_example
	#rm -f merge_sort
