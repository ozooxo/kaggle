#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h> 

// In my edition of gcc, CHAR is 8 bits, INT is 32 bits, LONG and 
// LONG LONG are 64 bits. This algorithm need these assumptions, 
// otherwise the RAM will not be enough.

#define PATTERN_TABLE_SIZE 67108864 // 2^26. 
// I tried 2^29, but it seems that gcc doesn't support such large array.

#define COUNT_TABLE_SIZE 71629613//71478593 
// any odd number should be fine, since the patterns are in 2's power. 
// some better hash function should be better, currently just use
// pattern%COUNT_TABLE_SIZE while COUNT_TABLE_SIZE = 2^25-1.

#define LOOP_SIZE 500000
// < 1000000 should be fine, to avoid overflow. choose 500000 in final run.

////////////////////////////////////////////////////////////////////////
// Utility Functions

void clean_array (int *grid, int n) {
	int i;
	for (i = 0; i < n; ++i) grid[i] = 0;
}

void clean_chararray (char *grid, int n) {
	int i;
	for (i = 0; i < n; ++i) grid[i] = 0;
}

void random_grid (int grid[][26]) {
	int m, n;
	int i, j;
	n = 4 + rand()%393;
	if (n <= 200) {
		m = 0;
		while (m < n) {
			i = 3 + rand()%20;
			j = 3 + rand()%20;
			if (grid[i][j] == 0) {
				grid[i][j] = 1;
				++m;
			}
		}
	}
	else {
		for (i = 3; i < 23; ++i) for (j = 3; j < 23; ++j) grid[i][j] = 1;
		m = 400;
		while (m > n) {
			i = 3 + rand()%20;
			j = 3 + rand()%20;
			if (grid[i][j] == 1) {
				grid[i][j] = 0;
				--m;
			}
		}
	}
}

int blank_grid(int grid[][26]) {
	int i, j; 
	for (i = 3; i < 23; ++i) for (j = 3; j < 23; ++j) {
		if (grid[i][j] == 1) return 0;
	}
	return 1;
}

void copy_grid (int old_grid[][26], int new_grid[][26]) {
	int i, j;
	for (i = 3; i < 23; ++i) for (j = 3; j < 23; ++j) old_grid[i][j] = new_grid[i][j];
}

void print_grid (int grid[][26]) {
	int i, j;
	for (i = 3; i < 23; ++i) {
		for (j = 3; j < 23; ++j) printf("%d ", grid[i][j]);
		printf("\n");
	}
}

////////////////////////////////////////////////////////////////////////
// IO Functions

void read_train (FILE *train, int *id, int *delta, int *start_grid, int *stop_grid) {
	fscanf (train, "%d,%d,", id, delta);
	int i, j;
	for (i = 3; i < 23; ++i) {
		for (j = 3; j < 23; ++j) fscanf(train, "%d,", &(start_grid[i*26+j]));
	}
	for (i = 3; i < 22; ++i) {
		for (j = 3; j < 23; ++j) fscanf(train, "%d,", &(stop_grid[i*26+j]));
	}
	for (j = 3; j < 22; ++j) fscanf(train, "%d,", &(stop_grid[22*26+j]));
	fscanf(train, "%d\n", &(stop_grid[22*26+22]));
}

void write_pred (FILE *pred, int id, int initial_grid[][26], int pred_grid[][26]) {
	int i, j;
	fprintf(pred, "%d,", id);
	for (i = 3; i < 23; ++i) {
		for (j = 3; j < 23; ++j) fprintf(pred, "%d,", pred_grid[i][j]);
	}
	for (i = 3; i < 22; ++i) {
		for (j = 3; j < 23; ++j) fprintf(pred, "%d,", pred_grid[i][j] - initial_grid[i][j]);
	}
	for (j = 3; j < 22; ++j) fprintf(pred, "%d,", pred_grid[22][j] - initial_grid[22][j]);
	fprintf(pred, "%d\n", pred_grid[22][22] - initial_grid[22][22]);
}

void read_test (FILE *test, int *id, int *delta, int stop_grid[][26]) {
	fscanf (test, "%d,%d,", id, delta);
	int i, j;
	for (i = 3; i < 22; ++i) {
		for (j = 3; j < 23; ++j) fscanf(test, "%d,", &(stop_grid[i][j]));
	}
	for (j = 3; j < 22; ++j) fscanf(test, "%d,", &(stop_grid[22][j]));
	fscanf(test, "%d\n", &(stop_grid[22][22]));
}

void write_submission (FILE *submission, int id, int pred_grid[][26]) {
	int i, j;
	fprintf(submission, "%d,", id);
	for (i = 3; i < 22; ++i) {
		for (j = 3; j < 23; ++j) fprintf(submission, "%d,", pred_grid[i][j]);
	}
	for (j = 3; j < 22; ++j) fprintf(submission, "%d,", pred_grid[22][j]);
	fprintf(submission, "%d\n", pred_grid[22][22]);
}

////////////////////////////////////////////////////////////////////////

void write_pattern (unsigned int pattern, char pattern_exist[]) {
	unsigned int first = pattern >> 3;
	unsigned int last = pattern & 0b111;
	pattern_exist[first] = pattern_exist[first] | (0b1 << last);
}

int read_pattern (unsigned int pattern, char pattern_exist[]) {
	unsigned int first = pattern >> 3;
	unsigned int last = pattern & 0b111;
	return (pattern_exist[first] >> last) & 0b1;
}
 
int sum_bit (char word) {
	int n, sum = 0;
	for (n = 0; n < 8; ++n) {
		sum += word & 0b1;
		word >>= 1;
	}
	return sum;
}
 
int sum_patterns (char pattern_exist[], int size) {
	int n, sum = 0;
	for (n = 0; n < size; ++n) sum += sum_bit(pattern_exist[n]);
	return sum;
}

////////////////////////////////////////////////////////////////////////

void conway_step (int start_grid[][26], int stop_grid[][26]) {
	int i, j, sum;
	for (i = 3; i < 23; ++i) {
		for (j = 3; j < 23; ++j) {
			sum = start_grid[i-1][j-1] + start_grid[i-1][j] + start_grid[i-1][j+1]
				+ start_grid[i][j-1] + start_grid[i][j+1]
				+ start_grid[i+1][j-1] + start_grid[i+1][j] + start_grid[i+1][j+1];
			if (start_grid[i][j] == 1) {
				if (sum > 3) stop_grid[i][j] = 0;
				else if (sum < 2) stop_grid[i][j] = 0;
				else stop_grid[i][j] = 1;
			}
			else if (sum == 3) stop_grid[i][j] = 1;
			else stop_grid[i][j] = 0;
		}
	}
}

unsigned int get_pattern_idx (int grid[][26], int i, int j) {
	unsigned long int pattern = 0;
	unsigned int hashed = 0;
	int m, n;

	for (m = -3; m <= 3; ++m) {
		for (n = -3; n <= 3; ++n) {
			pattern = pattern << 1;
			pattern += grid[i+m][j+n];
		}
	}
	

	hashed = ((pattern & 0b0111110000000000000000000000000000000000000000000) >> 24)
	       | ((pattern & 0b0000000110000000000000000000000000000000000000000) >> 23)
	       | ((pattern & 0b0000000000001110000000000000000000000000000000000) >> 20)
	       | ((pattern & 0b0000000000000000000011000000000000000000000000000) >> 15)
	       | ((pattern & 0b0000000000000000000000000001100000000000000000000) >> 10)
	       | ((pattern & 0b0000000000000000000000000000000000111000000000000) >> 5)
	       | ((pattern & 0b0000000000000000000000000000000000000000110000000) >> 2)
	       | ((pattern & 0b0000000000000000000000000000000000000000000111110) >> 1);
	hashed = (hashed%7301)%1024;
	
	hashed = hashed
	       | ((pattern & 0b0000000001110000000000000000000000000000000000000) >> 18)
	       | ((pattern & 0b0000000000000001000000000000000000000000000000000) >> 15)
	       | ((pattern & 0b0000000000000000000100000000000000000000000000000) >> 12)
	       | ((pattern & 0b0000000000000000000000100000000000000000000000000) >> 10)
	       | ((pattern & 0b0000000000000000000000000010000000000000000000000) >> 7)
	       | ((pattern & 0b0000000000000000000000000000010000000000000000000) >> 5)
	       | ((pattern & 0b0000000000000000000000000000000001000000000000000) >> 2)
	       | ((pattern & 0b0000000000000000000000000000000000000111000000000) << 1);
	hashed = (hashed%1298173)%1048576;
	
	hashed = hashed
	       | ((pattern & 0b0000000000000000111000000000000000000000000000000) >> 4) 
	       |  (pattern & 0b0000000000000000000000011100000000000000000000000)
	       | ((pattern & 0b0000000000000000000000000000001110000000000000000) << 4);
/*
	hashed = ((pattern & 0b0011100000000000000000000000000000000000000000000) >> 31)
	       | ((pattern & 0b0000000010000000000000000000000000000000000000000) >> 28)
	       | ((pattern & 0b0000000000001000000000000000000000000000000000000) >> 25)
	       | ((pattern & 0b0000000000000010000000000000000000000000000000000) >> 24)
	       | ((pattern & 0b0000000000000000000011000000000000000000000000000) >> 19)
	       | ((pattern & 0b0000000000000000000000000001100000000000000000000) >> 14)
	       | ((pattern & 0b0000000000000000000000000000000000100000000000000) >> 9)
	       | ((pattern & 0b0000000000000000000000000000000000001000000000000) >> 8)
	       | ((pattern & 0b0000000000000000000000000000000000000000100000000) >> 5)
	       | ((pattern & 0b0000000000000000000000000000000000000000000011100) >> 2);
	hashed = (((hashed*241)%349081)%7001)%256;
	hashed = hashed << 21;
*/
/*
	hashed = ((pattern & 0b0001000000000000000000000000000000000000000000000) >> 42)
	       | ((pattern & 0b0000000000000000000001000000000000000000000000000) >> 25)
	       | ((pattern & 0b0000000000000000000000000001000000000000000000000) >> 20)
	       | ((pattern & 0b0000000000000000000000000000000000000000000001000) >> 3)
	       | ((pattern & 0b0000000010000000000000000000000000000000000000000) >> 36)
	       | ((pattern & 0b0000000000001000000000000000000000000000000000000) >> 31)
	       | ((pattern & 0b0000000000000000000000000000000000001000000000000) >> 6)
	       | ((pattern & 0b0000000000000000000000000000000000000000100000000) >> 1);
	hashed = hashed << 21;
	
	hashed = hashed
	       | ((pattern & 0b0000000001110000000000000000000000000000000000000) >> 19)
	       | ((pattern & 0b0000000000000001000000000000000000000000000000000) >> 16)
	       | ((pattern & 0b0000000000000000000100000000000000000000000000000) >> 13)
	       | ((pattern & 0b0000000000000000000000100000000000000000000000000) >> 11)
	       | ((pattern & 0b0000000000000000000000000010000000000000000000000) >> 8)
	       | ((pattern & 0b0000000000000000000000000000010000000000000000000) >> 6)
	       | ((pattern & 0b0000000000000000000000000000000001000000000000000) >> 3)
	       | ((pattern & 0b0000000000000000000000000000000000000111000000000) >> 0);
	//hashed = (((hashed*1013)%86026621)%1298173)%1048576;
	
	hashed = hashed
	       | ((pattern & 0b0000000000000000111000000000000000000000000000000) >> 24) 
	       | ((pattern & 0b0000000000000000000000011100000000000000000000000) >> 20)
	       | ((pattern & 0b0000000000000000000000000000001110000000000000000) >> 16);
*/
	//printf("%d\n", hashed);

	return hashed;
}

void vote_step (int start_grid[][26], int stop_grid[][26], char pattern_exist[],
                unsigned int count_case_0[], unsigned int count_case_1[]) {
	int i, j;
	unsigned int pattern;
	for (i = 3; i < 23; ++i) {
		for (j = 3; j < 23; ++j) {
			pattern = get_pattern_idx(stop_grid, i, j);
			if (read_pattern(pattern, pattern_exist) == 1) {
				if (start_grid[i][j] == 0) ++count_case_0[(pattern%179423311)%COUNT_TABLE_SIZE];
				else ++count_case_1[(pattern%179423311)%COUNT_TABLE_SIZE];
			}
		}
	}
}

char vote_pattern (unsigned int pattern, int count_0, int count_1, int delta) {
	double probability;
	if (count_0 + count_1 == 0) probability = 0;
	// for the patterns happen really less frequently, we assume the original center is definitely 0.
	else probability = (double)count_1 / (double)(count_0 + count_1);
	
	if (probability > 0.50) return 1;
	else return 0;
}

void reverse_step (int start_grid[][26], int stop_grid[][26], char vote_case[]) {
	int i, j;
	unsigned int pattern;
	for (i = 3; i < 23; ++i) {
		for (j = 3; j < 23; ++j) {	
			pattern = get_pattern_idx(stop_grid, i, j);
			start_grid[i][j] = vote_case[(pattern%179423311)%COUNT_TABLE_SIZE];		
		}
	}
}

double difference_grid (int a_grid[][26], int another_grid[][26]) {
	int i, j;
	int same = 0, different = 0;
	for (i = 3; i < 23; ++i) {
		for (j = 3; j < 23; ++j) {
			if (a_grid[i][j] == another_grid[i][j]) ++same;
			else ++different;
		}
	}
	return (double)different / (double)(same+different);
}

////////////////////////////////////////////////////////////////////////

char pattern_exist[5][PATTERN_TABLE_SIZE];

unsigned int count_case_0[5][COUNT_TABLE_SIZE], count_case_1[5][COUNT_TABLE_SIZE];
char vote_case[5][COUNT_TABLE_SIZE];

int initial_grid[26][26], start_grid[26][26], stop_grid[26][26];

void write_trainset_pattern () {
	
	int n, id, delta;
	int i, j;
	
	FILE *train;
	train = fopen ("train.csv", "r");
	while (fgetc(train) != '\n'); // skip the head line
	
	for (n = 0; n < 50000; ++n) {
		read_train (train, &id, &delta, (int*)initial_grid, (int*)stop_grid);
		//printf("%x\n", get_pattern_idx(stop_grid, 10, 10));
		for (i = 3; i < 23; ++i) {
			for (j = 3; j < 23; ++j) {
				write_pattern(get_pattern_idx(stop_grid, i, j), pattern_exist[delta-1]);
			}
		}
	}

	//for (n = 0; n < 5; ++n) printf("%d\n", (int)pattern_exist[0][n]);
	//printf("number of existing patterns (delta = 1): %d", sum_patterns (pattern_exist[0], PATTERN_TABLE_SIZE));
	
	fclose(train);
}


void write_testset_pattern () {
	
	int n, id, delta;
	int i, j;
	
	
	FILE *test;
	test = fopen ("test.csv", "r");
	while (fgetc(test) != '\n'); // skip the head line
	
	for (n = 0; n < 50000; ++n) {
		read_test(test, &id, &delta, stop_grid);
		for (i = 3; i < 23; ++i) {
			for (j = 3; j < 23; ++j) {
				write_pattern(get_pattern_idx(stop_grid, i, j), pattern_exist[delta-1]);
			}
		}
	}
	
	//write_pattern(1234567, pattern_exist[0]);
	//printf("%d\n", read_pattern(1234567, pattern_exist[0]));
	//printf("%d\n", read_pattern(1234568, pattern_exist[0]));

	//printf("number of existing patterns (delta = 1): %d", sum_patterns (pattern_exist[0], PATTERN_TABLE_SIZE));
	
	fclose(test);
}

////////////////////////////////////////////////////////////////////////

void random_generator (int loop_num) {
	int n, i, delta;
	
	for (n = 0; n < loop_num; ++n) {
		random_grid(start_grid);
		for (delta = 0; delta < 5; ++delta) {
			conway_step(start_grid, stop_grid);
			copy_grid(start_grid, stop_grid);
		}
		copy_grid(initial_grid, start_grid);
		if (blank_grid(initial_grid)) continue;
		//print_grid(initial_grid);
		for (delta = 0; delta < 5; ++delta) {
			conway_step(start_grid, stop_grid);
			if (blank_grid(stop_grid)) break;
			vote_step(initial_grid, stop_grid, pattern_exist[delta], count_case_0[delta], count_case_1[delta]);
			copy_grid(start_grid, stop_grid);
		}
	}
	
	// vote for statistical table
	for (i = 0; i < 5; ++i) {
		for (n = 0; n < COUNT_TABLE_SIZE; ++n) {
			vote_case[i][n] = vote_pattern(n, count_case_0[i][n], count_case_1[i][n], i);
			if ((count_case_0[i][n] > UINT_MAX/2) || (count_case_1[i][n] > UINT_MAX/2)) {
				count_case_0[i][n] = count_case_0[i][n]/2;
				count_case_1[i][n] = count_case_1[i][n]/2;
			}
		}
	}
	
	//printf("%d %d\n", count_case_0[0][0], count_case_1[0][0]);
}

void estimate_train_set () {
	int n, id, delta;
	
	FILE *train;
	train = fopen ("train.csv", "r");
	while (fgetc(train) != '\n') ;  // skip the head line
	
	FILE *pred;
	pred = fopen ("pred.csv", "w");
	fprintf(pred, "id,");
	for (n = 1; n < 401; ++n) fprintf(pred, "start.%d,", n);
	for (n = 1; n < 400; ++n) fprintf(pred, "diff.%d,", n);
	fprintf(pred, "diff.400\n");
	
	double difference_all = 0;
	for (n = 0; n < 50000; ++n) {
		read_train (train, &id, &delta, (int*)initial_grid, (int*)stop_grid);
		reverse_step (start_grid, stop_grid, vote_case[delta-1]);
		write_pred(pred, id, initial_grid, start_grid);
		difference_all += difference_grid(initial_grid, start_grid);
	}
	printf(" %f\n", difference_all/50000);
	fclose(pred);

	fclose(train);
}

void write_submitted_file (int k) {
	int n, id, delta;
	
	FILE *test;
	test = fopen ("test.csv", "r");
	while (fgetc(test) != '\n') ;  // skip the head line
	
	FILE *submission;
	char sub_name[80];
	sprintf(sub_name, "%s%d%s", "submission", k, ".csv");
	submission = fopen (sub_name, "w");
	fprintf(submission, "id,");
	for (n = 1; n < 400; ++n) fprintf(submission, "start.%d,", n);
	fprintf(submission, "start.400\n");
	
	for (n = 0; n < 50000; ++n) {
		read_test(test, &id, &delta, stop_grid);
		
		reverse_step (start_grid, stop_grid, vote_case[delta-1]);
		write_submission(submission, id, start_grid);
	}
	
	fclose(test);
	fclose(submission);
}

////////////////////////////////////////////////////////////////////////

int main () {
	
	// utility parameters
	clean_chararray((char*)pattern_exist, PATTERN_TABLE_SIZE*5);
	
	clean_array((int*)count_case_0, COUNT_TABLE_SIZE*5);
	clean_array((int*)count_case_1, COUNT_TABLE_SIZE*5);

	clean_array((int*)initial_grid, 26*26);
	clean_array((int*)start_grid, 26*26);
	clean_array((int*)stop_grid, 26*26);
	
	//------------------------------------------------------------------
	
	//write_trainset_pattern ();
	write_testset_pattern ();
	
	//------------------------------------------------------------------
	
	int k = 1;
	
	while (1) {
		printf("%d ", k*LOOP_SIZE);
		
		random_generator (LOOP_SIZE);
		estimate_train_set ();
		write_submitted_file (k);
		
		++k;
	}
	
	return 0;
}
/*
500000  0.132270
1000000  0.132106
1500000  0.132041
2000000  0.132009
2500000  0.131983
3000000  0.131968
3500000  0.131955
4000000  0.131948
4500000  0.131946
5000000  0.131938
5500000  0.131931
6000000  0.131922
6500000  0.131922
7000000  0.131920
7500000  0.131919
8000000  0.131916
8500000  0.131916
9000000  0.131911
9500000  0.131906
10000000  0.131906
10500000  0.131901
11000000  0.131905
11500000  0.131903
12000000  0.131899
12500000  0.131897
13000000  0.131894
13500000  0.131889
14000000  0.131891
14500000  0.131891
15000000  0.131889
15500000  0.131886
16000000  0.131881
16500000  0.131882
17000000  0.131881
17500000  0.131883
18000000  0.131882
18500000  0.131882
19000000  0.131882
19500000  0.131880
20000000  0.131880
20500000  0.131882
21000000  0.131881
21500000  0.131883
22000000  0.131882
22500000  0.131885
23000000  0.131885
23500000  0.131884
24000000  0.131881
24500000  0.131881
25000000  0.131878
25500000  0.131879
26000000  0.131878
26500000  0.131880
27000000  0.131881
27500000  0.131882
28000000  0.131885
28500000  0.131880
29000000  0.131877
29500000  0.131880
30000000  0.131881
30500000  0.131882
31000000  0.131879
31500000  0.131879
32000000  0.131878
32500000  0.131878
33000000  0.131881
33500000  0.131880
34000000  0.131880
34500000  0.131878
35000000  0.131878
35500000  0.131877
36000000  0.131877
36500000  0.131877
37000000  0.131873
37500000  0.131874
38000000  0.131873
38500000  0.131873
39000000  0.131872
39500000  0.131873
40000000  0.131874
40500000  0.131874
41000000  0.131874
41500000  0.131876
42000000  0.131875
42500000  0.131875
43000000  0.131874
43500000  0.131875
44000000  0.131873
44500000  0.131870
45000000  0.131870
45500000  0.131869
46000000  0.131869
46500000  0.131870
47000000  0.131870
47500000  0.131868
48000000  0.131868
48500000  0.131868
49000000  0.131866
49500000  0.131868
50000000  0.131870
50500000  0.131869
51000000  0.131868
51500000  0.131868
52000000  0.131869
52500000  0.131869
53000000  0.131869
53500000  0.131868
54000000  0.131868
54500000  0.131868
55000000  0.131867
55500000  0.131868
56000000  0.131869
56500000  0.131869
57000000  0.131867
57500000  0.131868
58000000  0.131869
58500000  0.131871
59000000  0.131870
59500000  0.131870
60000000  0.131870
60500000  0.131869
61000000  0.131869
61500000  0.131867
62000000  0.131867
62500000  0.131867
63000000  0.131869
63500000  0.131869
64000000  0.131870
64500000  0.131869
65000000  0.131868
65500000  0.131869
66000000  0.131866
66500000  0.131868
67000000  0.131866
67500000  0.131867
68000000  0.131867
68500000  0.131866
69000000  0.131867
69500000  0.131864
70000000  0.131865
70500000  0.131864
71000000  0.131864
71500000  0.131864
72000000  0.131865
72500000  0.131864
73000000  0.131865
73500000  0.131866
74000000  0.131867
74500000  0.131868
75000000  0.131869
75500000  0.131868
76000000  0.131869
76500000  0.131868
77000000  0.131869
77500000  0.131868
78000000  0.131867
78500000  0.131868
79000000  0.131868
79500000  0.131869
80000000  0.131868
80500000  0.131869
81000000  0.131867
81500000  0.131866
82000000  0.131868
82500000  0.131868
83000000  0.131866
83500000  0.131866
84000000  0.131866
84500000  0.131865
85000000  0.131866
85500000  0.131867
86000000  0.131867
86500000  0.131866
87000000  0.131865
87500000  0.131865
88000000  0.131866
88500000  0.131865
89000000  0.131866
89500000  0.131866
90000000  0.131864
90500000  0.131866 ~ 0.12054 (16 hours)
*/
