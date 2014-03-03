#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h> 

// In my edition of gcc, CHAR is 8 bits, INT is 32 bits, LONG and 
// LONG LONG are 64 bits. This algorithm need these assumptions, 
// otherwise the RAM will not be enough.

#define PATTERN_TABLE_SIZE 67108864 // 2^26. 
// I tried 2^29, but it seems that gcc doesn't support such large array.

#define COUNT_TABLE_SIZE 71629613 //33554431 
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
	unsigned int pattern = 0;
	int m, n;
	
	pattern = pattern << 1;
	pattern += grid[i-3][j];

	for (m = -2; m <= -1; ++m) {
		for (n = -2; n <= 2; ++n) {
			pattern = pattern << 1;
			pattern += grid[i+m][j+n];
		}
	}

	for (n = -3; n <= 3; ++n) {
		pattern = pattern << 1;
		pattern += grid[i][j+n];
	}
	
	for (m = 1; m <= 2; ++m) {
		for (n = -2; n <= 2; ++n) {
			pattern = pattern << 1;
			pattern += grid[i+m][j+n];
		}
	}
	
	pattern = pattern << 1;
	pattern += grid[i+3][j];
	
	return pattern;
}

void vote_step (int start_grid[][26], int stop_grid[][26], char pattern_exist[],
                unsigned int count_case_0[], unsigned int count_case_1[]) {
	int i, j;
	unsigned int pattern;
	for (i = 3; i < 23; ++i) {
		for (j = 3; j < 23; ++j) {
			pattern = get_pattern_idx(stop_grid, i, j);
			if (read_pattern(pattern, pattern_exist) == 1) {
				if (start_grid[i][j] == 0) ++count_case_0[pattern%COUNT_TABLE_SIZE];
				else ++count_case_1[pattern%COUNT_TABLE_SIZE];
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
			start_grid[i][j] = vote_case[pattern%COUNT_TABLE_SIZE];		
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
	// delta = 1: 920273
	// delta = 2: 877782
	// delta = 4: 799987
	
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
	// delta = 1: 920531
	// delta = 2: 865257
	
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
Use training set pattern list, 29 grids:
Note: (1) starting region goes worse than 25 grid pattern, probably because the
voting table have so many zeroes. (2) crashing can cause significant effects.
should maximize COUNT_TABLE_SIZE in case the RAM is enough. (3) in case 
COUNT_TABLE_SIZE is odd, for numbers roughly the same size, the result are roughly
the same.

50000  0.130172
100000  0.128841
150000  0.128124
 
Use test set pattern list, 29 grids:
Note: training score is like an upper limit.
 
500000  0.128111
1000000  0.127867
1500000  0.127783
2000000  0.127736
2500000  0.127694
3000000  0.127682
3500000  0.127667
4000000  0.127648
4500000  0.127644
5000000  0.127632
5500000  0.127636
6000000  0.127635
6500000  0.127631
7000000  0.127627
7500000  0.127629
8000000  0.127618
8500000  0.127622
9000000  0.127616
9500000  0.127611
10000000  0.127608
10500000  0.127606
11000000  0.127603
11500000  0.127604
12000000  0.127602
12500000  0.127597
13000000  0.127599
13500000  0.127597
14000000  0.127595
14500000  0.127594
15000000  0.127591
15500000  0.127597
16000000  0.127595
16500000  0.127597
17000000  0.127593
17500000  0.127593
18000000  0.127589
18500000  0.127587
19000000  0.127585
19500000  0.127585
20000000  0.127585
20500000  0.127583
21000000  0.127581
21500000  0.127581
22000000  0.127581
22500000  0.127583
23000000  0.127579
23500000  0.127578
24000000  0.127575
24500000  0.127574
25000000  0.127578
25500000  0.127576
26000000  0.127574
26500000  0.127576
27000000  0.127576
27500000  0.127572
28000000  0.127571
28500000  0.127570
29000000  0.127569
29500000  0.127565
30000000  0.127567
30500000  0.127567
31000000  0.127570
31500000  0.127569
32000000  0.127567
32500000  0.127570
33000000  0.127569
33500000  0.127570
34000000  0.127568
34500000  0.127569
35000000  0.127568
35500000  0.127569
36000000  0.127569
36500000  0.127565
37000000  0.127564
37500000  0.127564
38000000  0.127564
38500000  0.127564
39000000  0.127562
39500000  0.127563
40000000  0.127561
40500000  0.127562
41000000  0.127560
41500000  0.127562
42000000  0.127560
42500000  0.127561
43000000  0.127563
43500000  0.127563
44000000  0.127561
44500000  0.127563
45000000  0.127564
45500000  0.127562
46000000  0.127563
46500000  0.127564
47000000  0.127565
47500000  0.127564
48000000  0.127564
48500000  0.127565
49000000  0.127567
49500000  0.127567
50000000  0.127565
50500000  0.127563
51000000  0.127564
51500000  0.127566
52000000  0.127565
52500000  0.127564
53000000  0.127563
53500000  0.127563
54000000  0.127562
54500000  0.127563
55000000  0.127561
55500000  0.127560
56000000  0.127561
56500000  0.127563
57000000  0.127563
57500000  0.127562
58000000  0.127561
58500000  0.127563
59000000  0.127563
59500000  0.127565
60000000  0.127565
60500000  0.127565
61000000  0.127563
61500000  0.127564
62000000  0.127563
62500000  0.127565
63000000  0.127562
63500000  0.127564
64000000  0.127560
64500000  0.127561
65000000  0.127560
65500000  0.127560
66000000  0.127561
66500000  0.127560
67000000  0.127560
67500000  0.127561
68000000  0.127559
68500000  0.127560
69000000  0.127559
69500000  0.127559
70000000  0.127558
70500000  0.127559
71000000  0.127557
71500000  0.127557
72000000  0.127557
72500000  0.127559
73000000  0.127558
73500000  0.127559
74000000  0.127559
74500000  0.127558
75000000  0.127558 ~ 0.11711 (8 hours)
75500000  0.127558
76000000  0.127558
76500000  0.127558
77000000  0.127560
77500000  0.127562
78000000  0.127560
78500000  0.127561
79000000  0.127563
79500000  0.127563
80000000  0.127564
80500000  0.127562
81000000  0.127562
81500000  0.127562
82000000  0.127565
82500000  0.127561
83000000  0.127563
83500000  0.127563
84000000  0.127562
84500000  0.127563
85000000  0.127563
85500000  0.127562
86000000  0.127563
86500000  0.127562
87000000  0.127563
87500000  0.127562
88000000  0.127562
88500000  0.127562
89000000  0.127561
89500000  0.127561
90000000  0.127561
90500000  0.127559
91000000  0.127559
91500000  0.127559
92000000  0.127561
92500000  0.127561
93000000  0.127561
93500000  0.127559
94000000  0.127560
94500000  0.127558
95000000  0.127560
95500000  0.127558
96000000  0.127559
96500000  0.127559
97000000  0.127558
97500000  0.127558
98000000  0.127559
98500000  0.127559
99000000  0.127557
99500000  0.127558
100000000  0.127559
100500000  0.127559
101000000  0.127559
101500000  0.127557
102000000  0.127558
102500000  0.127559
103000000  0.127560
103500000  0.127560
104000000  0.127559
104500000  0.127561
105000000  0.127561
105500000  0.127558
106000000  0.127558
106500000  0.127557
107000000  0.127559
107500000  0.127560
108000000  0.127559
108500000  0.127560
109000000  0.127560
109500000  0.127559
110000000  0.127559
110500000  0.127559
111000000  0.127560
111500000  0.127559
112000000  0.127560
112500000  0.127559
113000000  0.127560
113500000  0.127560
114000000  0.127562
114500000  0.127561
115000000  0.127560
115500000  0.127560
116000000  0.127561
116500000  0.127560
117000000  0.127561
117500000  0.127560
118000000  0.127560
118500000  0.127560
119000000  0.127560
119500000  0.127560
120000000  0.127559
120500000  0.127561
121000000  0.127561
121500000  0.127561
122000000  0.127561
122500000  0.127558
123000000  0.127558
123500000  0.127555
124000000  0.127556
124500000  0.127555
125000000  0.127554 ~ 0.11673 (13 hours)
125500000  0.127553
126000000  0.127553
126500000  0.127553
127000000  0.127555
127500000  0.127555
128000000  0.127555
128500000  0.127554
129000000  0.127556
129500000  0.127556
130000000  0.127556
130500000  0.127555
131000000  0.127554
131500000  0.127553
132000000  0.127553
132500000  0.127555
133000000  0.127555
133500000  0.127555
134000000  0.127554
134500000  0.127553
135000000  0.127554
135500000  0.127553
136000000  0.127551
136500000  0.127551
137000000  0.127550
137500000  0.127551
138000000  0.127553
138500000  0.127552
139000000  0.127554
139500000  0.127552
140000000  0.127553
140500000  0.127553
141000000  0.127552
141500000  0.127552
142000000  0.127551
142500000  0.127551
143000000  0.127552
143500000  0.127552
144000000  0.127552
144500000  0.127549
145000000  0.127549
145500000  0.127549
146000000  0.127549
146500000  0.127549
147000000  0.127550
147500000  0.127551
148000000  0.127550
148500000  0.127550
149000000  0.127549
149500000  0.127551
150000000  0.127550
150500000  0.127550
151000000  0.127549
151500000  0.127550
152000000  0.127549
152500000  0.127550
153000000  0.127549
153500000  0.127550
154000000  0.127550
154500000  0.127550
155000000  0.127551
155500000  0.127550
156000000  0.127550
156500000  0.127550
157000000  0.127550
157500000  0.127550
158000000  0.127550
158500000  0.127549
159000000  0.127549
159500000  0.127550
160000000  0.127550
160500000  0.127550
161000000  0.127550
161500000  0.127551
162000000  0.127552
162500000  0.127550
163000000  0.127551
163500000  0.127550
164000000  0.127550
164500000  0.127550
165000000  0.127550
165500000  0.127551
166000000  0.127551
166500000  0.127551
167000000  0.127552
167500000  0.127550
168000000  0.127551
168500000  0.127551
169000000  0.127552
169500000  0.127552
170000000  0.127551
170500000  0.127551
171000000  0.127551
171500000  0.127552
172000000  0.127551
172500000  0.127551
173000000  0.127550
173500000  0.127549
174000000  0.127550
174500000  0.127550
175000000  0.127550
175500000  0.127552
176000000  0.127551
176500000  0.127551
177000000  0.127552
177500000  0.127552 ~ 0.11650
178000000  0.127551


*/
