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
                int count_case_0[], int count_case_1[]) {
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
    double prob;	
} FLIP_POINT;

typedef struct {
    int size;
	FLIP_POINT items[400];
} FLIP_STACK;

void push_flip (FLIP_STACK *ps, int iflp, int jflp, double prob) {
	// I didn't exam stack overflow, since it will not happen in the correct case.
	ps->items[ps->size++] = (FLIP_POINT){iflp, jflp, prob};
}

FLIP_POINT pop_flip (FLIP_STACK *ps) {
	if (ps->size == 0){
		fputs("Error: stack underflow\n", stderr);
		return (FLIP_POINT){0, 0, 0};
	} 
	else return ps->items[--ps->size];
}

void quicksort_flips (FLIP_POINT *a, int n) {
	if (n < 2) return;
	FLIP_POINT p = a[n/2];
	FLIP_POINT *l = a;
	FLIP_POINT *r = a + n - 1;
	while (l <= r) {
		if ((*l).prob < p.prob) {
			l++;
			continue;
		}
		if ((*r).prob > p.prob) {
			r--;
			continue;
		}
		FLIP_POINT t = *l;
		*l++ = *r;
		*r-- = t;
	}
	quicksort_flips(a, r - a + 1);
	quicksort_flips(l, a + n - l);
}

void reverse_step (int start_grid[][24], int stop_grid[][24], int vote_case[], double probability_case[]) {
	int i, j, n, idx;
	//int count_ambiguity[4] = {0, 0, 0, 0};
	FLIP_STACK fs = {.size = 0};
	for (i = 2; i < 22; ++i) {
		for (j = 2; j < 22; ++j) {	
			idx = get_pattern_idx(stop_grid, i, j);
			start_grid[i][j] = vote_case[idx];
			
			//if ((0.15 < probability_case[idx]) && (probability_case[idx] < 0.85)) ++count_ambiguity[0];
			//if ((0.3 < probability_case[idx]) && (probability_case[idx] < 0.7)) ++count_ambiguity[1];
			//if ((0.4 < probability_case[idx]) && (probability_case[idx] < 0.6)) ++count_ambiguity[2];
			//if ((0.45 < probability_case[idx]) && (probability_case[idx] < 0.55)) ++count_ambiguity[3];
			
			// put all the points with median probability into a stack
			if (fabs(0.5-probability_case[idx]) < 0.125) push_flip(&fs, i, j, fabs(0.5-probability_case[idx]));
		}
	}
	
	//printf("%d,%d,%d,%d,%d,\n", count_1s_grid(stop_grid),
	//       count_ambiguity[0], count_ambiguity[1], count_ambiguity[2], count_ambiguity[3]);
	//printf("%d %d %f\n", imax, jmax, probability_max);
	
	quicksort_flips(fs.items, fs.size); // sort the stack
	
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

int count_case_0[5][2097152], count_case_1[5][2097152]; // 2097152 = pow(2, 21)
int vote_case[5][2097152];
double probability_case[5][2097152];

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
			if (count_case_0[i][n] < count_case_1[i][n]) vote_case[i][n] = 1;
			else vote_case[i][n] = 0;
			
			if (count_case_0[i][n]+count_case_1[i][n] == 0) probability_case[i][n] = 0;
			// for the patterns happen really less frequently, we assume the original center is definitely 0.
			else probability_case[i][n] = (double)count_case_1[i][n] / (double)(count_case_0[i][n]+count_case_1[i][n]);
		}
	}
	
	//------------------------------------------------------------------
	
	// check prediction accuracy
	///*
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
		//for (i = 0; i < 1; ++i) {
			conway_step(start_grid, stop_grid);
			copy_grid(start_grid, stop_grid);
		}
		
		for (i = delta-1; i >= 0; --i) {
		//for (i = 0; i >= 0; --i) {
			reverse_step (start_grid, stop_grid, vote_case[i], probability_case[i]);
			copy_grid(stop_grid, start_grid);
		}
		write_pred(pred, id, initial_grid, start_grid);
		difference_all += difference_grids(initial_grid, start_grid);
	}
	printf("training set score: %f\n", difference_all/50000);
	fclose(pred);
	//*/

	fclose(train);
	
	//------------------------------------------------------------------
	
	// make submission file
	/*
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
			reverse_step (start_grid, stop_grid, vote_case[i], probability_case[i]);
			copy_grid(stop_grid, start_grid);
		}
		write_submission(submission, id, start_grid);
	}
	
	fclose(test);
	fclose(submission);
	*/
	
	return 0;
	
	// goes the reverse check (see everything after vote_case in reverse_step).
	// however, the imporvement seems really tiny.
	// for the cutoff of points which need to be checked reversely,
	// I use all the predicted data (50000+50000) and got the following table.
	// threshold | score
	//     0     | 0.128226
	//     0.05  | 0.127253
	//     0.1   | 0.126986
	//     0.12  | 0.126851
	//     0.125 | 0.126803  <--
	//     0.127 | 0.126826
	//     0.13  | 0.126824
	//     0.135 | 0.126889
	//     0.14  | 0.126914
	//     0.15  | 0.127000
	
	// so I use 0.127 (should use 0.125) to predict the test set.
	// predicted set score: 0.12957 (-0.0003)
	// the improvement is even smaller than I thought, I think it should be around 0.0014.
	// :-(
	
	// probably the imporvement of the training set, is because we use the training set
	// itself to calculate the scores, which gives bias.
	
	//---------------------
	// then use 0.125, and set the probably to be 0.5 if the pattern never appear.
	// predicted set score: 0.13034 (+0.0004)  -- even go worse~~
	
	//---------------------
	// if ruling out the unfrequent patterns, it doesn't seem help.
	//     0.125 | 0.127049 (rule out < 2)
	//     0.125 | 0.127095 (rule out < 4)
}
