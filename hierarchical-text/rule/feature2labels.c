#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TRAIN_NROWS 2365440 // # of rows in input document
//#define TRAIN_NROWS 30000
#define EVAL_NROWS 2368
#define TEST_NROWS 5000

#define NLABELS 445729+1
#define NFEATURES 2085166+1 // unique # of feature tags, probably 2085166

#define TRAIN_NLABELDATA TRAIN_NROWS*20
#define TRAIN_NFEATUREDATA TRAIN_NROWS*100

#define NTARGET_MAX 100 // it should be large enough

#include "../csr.c"
#include "../read.c"

////////////////////////////////////////////////////////////////////////

int feature_in_row (int target_feature[], int n_target, csr *features, int row) {
	int i, j, n_get = 0;
	for (i = features->ptr[row]; i < features->ptr[row+1]; ++i) {
		for (j = 0; j < n_target; ++j) {
			if (features->idx[i] == target_feature[j]) ++n_get;
		}
	}
	if (n_get == n_target) return 1;
	else return 0;
}

void label_count_input (int label_count[], csr *labels, int row) {
	int i;
	for (i = labels->ptr[row]; i < labels->ptr[row+1]; ++i) {
		++label_count[labels->idx[i]];
	}
}

////////////////////////////////////////////////////////////////////////

int main (int argc, char *argv[]) {
	if (argc < 4) {
		printf( "Usage:\n  feature_vs_label [TRAIN FILE] [THRESHOLD] [LABELS...]\n\n");
		return 1;
	}

	//------------------------------------------------------------------

	csr *labels = csr_malloc(TRAIN_NROWS, TRAIN_NLABELDATA);
	csr *features = csr_malloc(TRAIN_NROWS, TRAIN_NFEATUREDATA);

	FILE *train;
	train = fopen(argv[1], "r");
	read_train(train, labels, features);
	fclose(train);

	int i, j;

	int n_target;
	n_target = argc - 3;

	double threshold, probability;
	threshold = atof(argv[2]);
	
	int target_features[NTARGET_MAX];
	for (i = 0; i < n_target; ++i) {
		target_features[i] = atoi(argv[i+3]);
	}

	int label_count[NLABELS];
	int all_count = 0;
	for (i = 0; i < NLABELS; ++i) label_count[i] = 0;
	
	for (i = 0; i < features->nrows; ++i) {
		if(feature_in_row(target_features, n_target, features, i)) {
			//printf("%d\n", labels->idx[labels->ptr[i]]);
			label_count_input(label_count, labels, i);
			++all_count;
		}
	}
	
	for (i = 0; i < NLABELS; ++i) {
		probability = (double)label_count[i]/all_count;
		if (probability >= threshold) {
			printf("feature ");
			for (j = 0; j < n_target; ++j) printf("%d ", target_features[j]); 
			printf("=> label %d prob %lf\n", i, probability);
		}
	}
	
	csr_free(labels);
	csr_free(features);
	return 0;
}
