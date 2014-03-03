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

void read_train (FILE *train, int *id, int *delta, int *start_grid, int *stop_grid) {
	fscanf (train, "%d,%d,", id, delta);
	int i, j;
	for (i = 2; i < 22; ++i) {
		for (j = 2; j < 22; ++j) fscanf(train, "%d,", &(start_grid[i*24+j]));
	}
	for (i = 2; i < 21; ++i) {
		for (j = 2; j < 22; ++j) fscanf(train, "%d,", &(stop_grid[i*24+j]));
	}
	for (j = 2; j < 21; ++j) fscanf(train, "%d,", &(stop_grid[21*24+j]));
	fscanf(train, "%d\n", &(stop_grid[21*24+21]));
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

int vote_pattern (int pattern, int count_0, int count_1, int delta) {
	double probability;
	if (count_0 + count_1 == 0) probability = 0;
	// for the patterns happen really less frequently, we assume the original center is definitely 0.
	else probability = (double)count_1 / (double)(count_0 + count_1);
	
	if (probability > 0.50) return 1;
	else return 0;
}

void reverse_step (int start_grid[][24], int stop_grid[][24], int vote_case[]) {
	int i, j, idx;
	for (i = 2; i < 22; ++i) {
		for (j = 2; j < 22; ++j) {	
			idx = get_pattern_idx(stop_grid, i, j);
			start_grid[i][j] = vote_case[idx];		
		}
	}
}

double difference_grid (int a_grid[][24], int another_grid[][24]) {
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

////////////////////////////////////////////////////////////////////////

unsigned int count_case_0[5][2097152], count_case_1[5][2097152]; // 2097152 = pow(2, 21)
int vote_case[5][2097152];

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
		read_train(train, &id, &delta, (int*)start_grid, (int*)stop_grid);
		vote_step(start_grid, stop_grid, count_case_0[delta-1], count_case_1[delta-1]);
	}
	
	// vote for statistical table
	for (i = 0; i < 5; ++i) {
		for (n = 0; n < 2097152; ++n) 
			vote_case[i][n] = vote_pattern(n, count_case_0[i][n], count_case_1[i][n], i);
	}
	
	//printf("%d %d\n", count_case_0[0][0], count_case_1[0][0]);
	
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
		read_train (train, &id, &delta, (int*)initial_grid, (int*)stop_grid);
		reverse_step (start_grid, stop_grid, vote_case[delta-1]);
		write_pred(pred, id, initial_grid, start_grid);
		difference_all += difference_grid(initial_grid, start_grid);
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

		reverse_step (start_grid, stop_grid, vote_case[delta-1]);
		write_submission(submission, id, start_grid);
	}
	
	fclose(test);
	fclose(submission);
	//*/
	
	return 0;
	// this is a super-simplicated algorithm which only does the pattern matching of
	// the train set initial/final results and do a probability vote. even the detail
	// of what conway rule is is not relavent. therefore, the training set score is 
	// like an lower limit score if we only care about the neighbor of the point in
	// some size (currently it is 20 neighbors plus itself). however, there's bias
	// between the training and the test set. If the training set comes from exact the
	// random procedure the kaggle website told us, we can simulate larger training
	// set, therefore, imporve the voting for the test set. the other way is to increase
	// the size of the matching pattern, but the vote_case table will go large really
	// quickly (which makes us quickly run out of the ram), and unless the training
	// set goes larger in the same speed, the vote_case table will be really sparse,
	// and the voting will go worse.
	//
	// training set score: 0.096463 (goes larger if the cutoff is not 0.5)
	// real score: 0.12873 (not so bad, a little bit better than simple prob reverse 0.12988)
}
