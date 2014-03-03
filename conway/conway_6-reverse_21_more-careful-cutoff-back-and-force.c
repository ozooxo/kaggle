#include <stdio.h>
#include <math.h> 

//#include <string.h>

////////////////////////////////////////////////////////////////////////
// Utility Functions

void clean_array (int *grid, int n) {
	int i;
	for (i = 0; i < n; ++i) grid[i] = 0;
}

void copy_grid (int old_grid[][24], int new_grid[][24]) {
	int i, j;
	for (i = 2; i < 22; ++i) for (j = 2; j < 22; ++j) old_grid[i][j] = new_grid[i][j];
}

void print_grid (int grid[][24]) {
	int i, j;
	for (i = 2; i < 22; ++i) {
		for (j = 2; j < 22; ++j) printf("%d ", grid[i][j]);
		printf("\n");
	}
}

////////////////////////////////////////////////////////////////////////
// IO Functions

void read_train (FILE *train, int *id, int *delta, int *start_grid) {
	fscanf (train, "%d,%d,", id, delta);
	int i, j;
	for (i = 2; i < 22; ++i) {
		for (j = 2; j < 22; ++j) fscanf(train, "%d,", &(start_grid[i*24+j]));
	}
	while (fgetc(train) != '\n') ; // eat everything else in this line
}

void write_pred (FILE *pred, int id, int initial_grid[][24], int pred_grid[][24]) {
	int i, j;
	fprintf(pred, "%d,", id);
	for (i = 2; i < 22; ++i) {
		for (j = 2; j < 22; ++j) fprintf(pred, "%d,", pred_grid[i][j]);
	}
	for (i = 2; i < 21; ++i) {
		for (j = 2; j < 22; ++j) fprintf(pred, "%d,", pred_grid[i][j] - initial_grid[i][j]);
	}
	for (j = 2; j < 21; ++j) fprintf(pred, "%d,", pred_grid[21][j] - initial_grid[21][j]);
	fprintf(pred, "%d\n", pred_grid[21][21] - initial_grid[21][21]);
}

void read_test (FILE *test, int *id, int *delta, int stop_grid[][24]) {
	fscanf (test, "%d,%d,", id, delta);
	int i, j;
	for (i = 2; i < 21; ++i) {
		for (j = 2; j < 22; ++j) fscanf(test, "%d,", &(stop_grid[i][j]));
	}
	for (j = 2; j < 21; ++j) fscanf(test, "%d,", &(stop_grid[21][j]));
	fscanf(test, "%d\n", &(stop_grid[21][21]));
}

void write_submission (FILE *submission, int id, int pred_grid[][24]) {
	int i, j;
	fprintf(submission, "%d,", id);
	for (i = 2; i < 21; ++i) {
		for (j = 2; j < 22; ++j) fprintf(submission, "%d,", pred_grid[i][j]);
	}
	for (j = 2; j < 21; ++j) fprintf(submission, "%d,", pred_grid[21][j]);
	fprintf(submission, "%d\n", pred_grid[21][21]);
}

////////////////////////////////////////////////////////////////////////

void conway_step (int start_grid[][24], int stop_grid[][24]) {
	int i, j, sum;
	for (i = 2; i < 22; ++i) {
		for (j = 2; j < 22; ++j) {
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

int get_pattern_idx (int grid[][24], int i, int j) {
	int pattern = 0;
	int m, n;
	for (n = -1; n <= 1; ++n) {
		pattern = pattern << 1;
		pattern += grid[i-2][j+n];
	}
	for (m = -1; m <= 1; ++m) {
		for (n = -2; n <= 2; ++n) {
			pattern = pattern << 1;
		pattern += grid[i+m][j+n];
		}
	}
	for (n = -1; n <= 1; ++n) {
		pattern = pattern << 1;
		pattern += grid[i+2][j+n];
	}
	return pattern;
}

void vote_step (int start_grid[][24], int stop_grid[][24], 
                unsigned int count_case_0[], unsigned int count_case_1[]) {
	int i, j, idx;
	for (i = 2; i < 22; ++i) {
		for (j = 2; j < 22; ++j) {
			// Currently INT_MAX = 2147483647. It is quite safe 
			// so I don't need to check potential overflow.
			// #include <limits.h>
			idx = get_pattern_idx(stop_grid, i, j);
			if (start_grid[i][j] == 0) ++count_case_0[idx];
			else ++count_case_1[idx];
		}
	}
}

int cross (int pattern) {
	return (pattern >> 5) & 0b10001110001;
}

int vote_pattern (int pattern, int count_0, int count_1, int delta) {
	double probability, threshold;
	if (count_0 + count_1 == 0) return 0;
	// for the patterns happen really less frequently, we assume the original center is definitely 0.
	// if go back-and-force, things goes worse (0.125489->0.125741)
	else probability = (double)count_1 / (double)(count_0 + count_1);
	
	switch (cross(pattern)) {
		// since independent, every parameter can be adjust seperately
		case 0b10001010001:
			threshold = 0.640;
			break;
			// 0.640: 0.125605 <--
			// 0.630: 0.125605
			// 0.550: 0.125605
		case 0b10001010000:
		case 0b10001000001:
		case 0b10000010001:
		case 0b00001010001:
			threshold = 0.640;
			break;
		case 0b10001000000:
		case 0b10000000001:
		case 0b00000010001:
		case 0b00001010000:
			threshold = 0.630;
			break;
			// 0.600: 0.125494
			// 0.620: 0.125494
			// 0.630: 0.125489 <--
			// 0.640: 0.125492
			// 0.680: 0.125568
		case 0b10000010000:
		case 0b00001000001:
			threshold = 0.535;
			break;
			// 0.500: 0.125507
			// 0.520: 0.125496 
			// 0.530: 0.125491
			// 0.535: 0.125492 <--
			// 0.540: 0.125492
			// 0.560: 0.125497
			// 0.580: 0.125496
			// 0.620: 0.125501
			// 0.640: 0.125501
			// 0.670: 0.125524
		case 0b00000000001:
		case 0b00000010000:
		case 0b00001000000:
		case 0b10000000000:
			threshold = 0.705;
			break;
			// "Blinker" goes back if this threshold goes smaller (may <0.55 or more),
			// but predict goes worse if we want "blinker".
			// 0.500: 0.126045
			// 0.550: 0.125859
			// 0.600: 0.125605
			// 0.640: 0.125537
			// 0.660: 0.125525
			// 0.700: 0.125503
			// 0.705: 0.125501 <--
			// 0.708: 0.125502
			// 0.710: 0.125500
			// 0.720: 0.125512
			// 0.750: 0.125545
		case 0b00000000000:
			threshold = 0.760;
			break;
			// 0.630: 0.125604
			// 0.637: 0.125592
			// 0.640: 0.125591
			// 0.643: 0.125587
			// 0.646: 0.125587
			// 0.650: 0.125586
			// 0.660: 0.125586
			// 0.680: 0.125555
			// 0.700: 0.125550
			// 0.740: 0.125543
			// 0.750: 0.125537
			// 0.760: 0.125537 <--
			// 0.770: 0.125538
			// 0.800: 0.125556
		case 0b10001110001:
			threshold = 0.607;
			break;
		case 0b10001110000:
		case 0b10001100001:
		case 0b10000110001:
		case 0b00001110001:
			threshold = 0.607;
			break;
		case 0b10001100000:
		case 0b10000100001:
		case 0b00000110001:
		case 0b00001110000:
			if (probability > 0.607) return 1;
			else return 0;
			break;
			// 0.604: 0.125592
			// 0.607: 0.125591 <--
			// 0.610: 0.125593
		case 0b10000110000:
		case 0b00001100001:
			threshold = 0.607;
			break;
			// 0.604: 0.125591
			// 0.607: 0.125591 <--
			// 0.610: 0.125590
			// 0.615: 0.125596
			// 0.620: 0.125609
		case 0b00000100001:
		case 0b00000110000:
		case 0b00001100000:
		case 0b10000100000:
			threshold = 0.603;
			break;
			// 0.595: 0.125693
			// 0.600: 0.125624
			// 0.602: 0.125594
			// 0.603: 0.125591 <--
			// 0.604: 0.125599
			// 0.607: 0.125598
			// 0.610: 0.125617
		case 0b00000100000:
			threshold = 0.611;
			break;
			// 0.600: 0.125610
			// 0.605: 0.125607
			// 0.607: 0.125605
			// 0.608: 0.125603
			// 0.609: 0.125600
			// 0.610: 0.125599
			// 0.611: 0.125598 <--
			// 0.613: 0.125600
			// 0.630: 0.125699
		default:
			threshold = 0;
	}
	
	double gap = 0.14;
	if (probability > (threshold+gap)) return 1;
	else if (probability < (threshold-gap)) return 0;
	else if (probability > threshold) return 3;
	else return 2;
	// 0.00: 0.125489
	// 0.05: 0.125365
	// 0.10: 0.125109
	// 0.13: 0.124991
	// 0.14: 0.124948 <--
	// 0.15: 0.124970
	// 0.20: 0.125166
}

////////////////////////////////////////////////////////////////////////

int count_1s_grid (int grid[][24]) {
	int i, j, count = 0;
	for (i = 2; i < 22; ++i) {
		for (j = 2; j < 22; ++j) count += grid[i][j];
	}
	return count;
}

void flip_point (int i, int j, int grid[][24]) {
	if (grid[i][j] == 1) grid[i][j] = 0;
	else grid[i][j] = 1;
}

double difference_grids (int a_grid[][24], int another_grid[][24]) {
	int i, j;
	int same = 0, different = 0;
	for (i = 2; i < 22; ++i) {
		for (j = 2; j < 22; ++j) {
			if (a_grid[i][j] == another_grid[i][j]) ++same;
			else ++different;
		}
	}
	return (double)different / (double)(same+different);
}

double difference_forward (int start_grid[][24], int stop_grid[][24]) {
	int forward_grid[24][24];
	clean_array((int*)forward_grid, 24*24);
	conway_step(start_grid, forward_grid);
	return difference_grids(forward_grid, stop_grid);
}

typedef struct {
    int i;
    int j;	
} FLIP_POINT;

typedef struct {
    int size;
	FLIP_POINT items[400];
} FLIP_STACK;

void push_flip (FLIP_STACK *ps, int iflp, int jflp) {
	// I didn't exam stack overflow, since it will not happen in the correct case.
	ps->items[ps->size++] = (FLIP_POINT){iflp, jflp};
}

FLIP_POINT pop_flip (FLIP_STACK *ps) {
	if (ps->size == 0){
		fputs("Error: stack underflow\n", stderr);
		return (FLIP_POINT){0, 0};
	} 
	else return ps->items[--ps->size];
}

void reverse_step (int start_grid[][24], int stop_grid[][24], int vote_case[]) {
	int i, j, n, idx;
	FLIP_STACK fs = {.size = 0};
	for (i = 2; i < 22; ++i) {
		for (j = 2; j < 22; ++j) {	
			idx = get_pattern_idx(stop_grid, i, j);
			start_grid[i][j] = vote_case[idx]%2;
			if (vote_case[idx] >= 2) push_flip(&fs, i, j);		
		}
	}
	
	FLIP_POINT ijp;
	double score_start, score_flip;
	for (n = fs.size; n > 0; --n) {
		score_start = difference_forward(start_grid, stop_grid);
		ijp = pop_flip(&fs);
		flip_point(ijp.i, ijp.j, start_grid);
		score_flip = difference_forward(start_grid, stop_grid);
		if (score_start <= score_flip) flip_point(ijp.i, ijp.j, start_grid);
	}
}

////////////////////////////////////////////////////////////////////////

unsigned int count_case_0[5][2097152], count_case_1[5][2097152]; // 2097152 = pow(2, 21)
int vote_case[5][2097152];
//double probability_case[5][2097152];

int main () {
	
	// utility parameters
	int n, i;
	
	clean_array((int*)count_case_0, 2097152*5);
	clean_array((int*)count_case_1, 2097152*5);
	
	// read the train.csv data
	FILE *train;
	train = fopen ("train.csv", "r");
	while (fgetc(train) != '\n') ;  // skip the head line

	int id, delta, initial_grid[24][24], start_grid[24][24], stop_grid[24][24];
	clean_array((int*)initial_grid, 24*24);
	clean_array((int*)start_grid, 24*24);
	clean_array((int*)stop_grid, 24*24);
	
	// read all the train set and make statistical table for them
	for (n = 0; n < 50000; ++n) {
		read_train(train, &id, &delta, (int*)start_grid);
		for (i = 0; i < delta; ++i) {
			conway_step(start_grid, stop_grid);
			vote_step(start_grid, stop_grid, count_case_0[i], count_case_1[i]);
			copy_grid(start_grid, stop_grid);
		}
	}
	
	// vote for statistical table
	for (i = 0; i < 5; ++i) {
		for (n = 0; n < 2097152; ++n) {
			vote_case[i][n] = vote_pattern(n, count_case_0[i][n], count_case_1[i][n], i);
			
			//if (count_case_0[i][n]+count_case_1[i][n] == 0) probability_case[i][n] = 0;
			//else probability_case[i][n] = (double)count_case_1[i][n] / (double)(count_case_0[i][n]+count_case_1[i][n]);
		}
	}
	
	//------------------------------------------------------------------
	
	// check prediction accuracy
	/*
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
		read_train (train, &id, &delta, (int*)initial_grid);
		copy_grid(start_grid, initial_grid);

		for (i = 0; i < delta; ++i) {
			conway_step(start_grid, stop_grid);
			copy_grid(start_grid, stop_grid);
		}
		
		for (i = delta-1; i >= 0; --i) {
			reverse_step (start_grid, stop_grid, vote_case[i]);
			copy_grid(stop_grid, start_grid);
		}
		write_pred(pred, id, initial_grid, start_grid);
		difference_all += difference_grids(initial_grid, start_grid);
	}
	printf("training set score: %f\n", difference_all/50000);
	fclose(pred);
	*/

	fclose(train);
	
	//------------------------------------------------------------------
	
	// make submission file
	///*
	FILE *test;
	test = fopen ("test.csv", "r");
	while (fgetc(train) != '\n') ;  // skip the head line
	
	FILE *submission;
	submission = fopen ("submission.csv", "w");
	fprintf(submission, "id,");
	for (n = 1; n < 400; ++n) fprintf(submission, "start.%d,", n);
	fprintf(submission, "start.400\n");
	
	for (n = 0; n < 50000; ++n) {
		read_test(test, &id, &delta, stop_grid);
		
		for (i = delta-1; i >= 0; --i) {
			reverse_step (start_grid, stop_grid, vote_case[i]);
			copy_grid(stop_grid, start_grid);
		}
		write_submission(submission, id, start_grid);
	}
	
	fclose(test);
	fclose(submission);
	//*/
	
	return 0;
	// training set score: 0.124948 (-0.00054)
	// real score: 0.12685 (-0.00002)
	// actual improvement is really tiny compare to the training set one.
}
