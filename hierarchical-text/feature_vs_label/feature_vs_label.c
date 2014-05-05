#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TRAIN_NROWS 2365440 // # of rows in input document
//#define TRAIN_NROWS 30000
#define EVAL_NROWS 2368
#define TEST_NROWS 5000

#define NLABELS 445729*3  // unique # of label tags (malloc error in "pred_row_label" if only 445729+1; I don't know why)
#define NFEATURES 2085166+1 // unique # of feature tags, probably 2085166

#define TRAIN_NLABELDATA TRAIN_NROWS*20
#define TRAIN_NFEATUREDATA TRAIN_NROWS*100
#define EVAL_NLABELDATA EVAL_NROWS*20
#define EVAL_NFEATUREDATA EVAL_NROWS*100
#define TEST_NFEATUREDATA EVAL_NROWS*100

#define INT_SCALE 10000 // used to scale the "val" (INT) contribute in "csr train_feature"
#define PRED_THRESHOLD 10 // when calculating similar rows, ones with similarity more than
                          // 1/PRED_THRESHOLD of the maximal similarity will count.
#define LABEL_THRESHOLD 0.05 // probability for a label to be predicted stay

////////////////////////////////////////////////////////////////////////

typedef struct {
	int nrows;
	int *ptr;
	int *idx;
	int *val;
	// for a g_slist, int data will just be casted to a pointer, 
	// while float/double can only be pointed out to a malloc.
	// so we'll just use int val.
} csr;

csr *csr_malloc(int nrows, int ndata) {
	csr *data = malloc(sizeof(csr));
	data->ptr = malloc(nrows*sizeof(int));
	data->idx = malloc(ndata*sizeof(int)); 
	data->val = malloc(ndata*sizeof(int)); 
}

void csr_free (csr *mat) {
	free(mat->ptr);
	free(mat->idx);
	free(mat->val);
	free(mat);
}

////////////////////////////////////////////////////////////////////////

void read_train (FILE *train, csr *labels, csr *features) {
	
	while (fgetc(train) != '\n') ; // skip the first line -- "Data"
	
	int nrows = 0, labelptr = 0, featureptr = 0;
	int buffer1, buffer2;
	
	labels->ptr[0] = 0;
	features->ptr[0] = 0;
	fscanf(train, "%d,", &buffer1);
	while (fgetc(train) != EOF) {
		labels->idx[labelptr] = buffer1;
		++labelptr;			
		while (fscanf(train, "%d,", &buffer1) == 1) {
			labels->idx[labelptr] = buffer1;
			++labelptr;
		}
		--labelptr;
		labels->ptr[nrows+1] = labelptr;
		
		fscanf(train, ":%d", &buffer2);
		features->idx[featureptr] = buffer1;
		features->val[featureptr] = buffer2;
		++featureptr;
		while (fscanf(train, "%d:%d", &buffer1, &buffer2) == 2) {
			features->idx[featureptr] = buffer1;
			features->val[featureptr] = buffer2;
			++featureptr;	
		}
		features->ptr[nrows+1] = featureptr;
		
		++nrows;
		//if (nrows > 30000) break;
	}
	labels->nrows = nrows;
	features->nrows = nrows;
}

////////////////////////////////////////////////////////////////////////

int main (int argc, char *argv[]) {
	if (argc != 2) {
		printf( "Usage:\n  feature_vs_label [TRAIN FILE]\n\n");
		return 1;
	}

	//------------------------------------------------------------------

	csr *labels = csr_malloc(TRAIN_NROWS, TRAIN_NLABELDATA);
	csr *features = csr_malloc(TRAIN_NROWS, TRAIN_NFEATUREDATA);

	FILE *train;
	train = fopen(argv[1], "r");
	read_train(train, labels, features);
	fclose(train);

	int i;
	for (i = 0; i < labels->nrows; ++i) 
		printf("%d %d\n", features->ptr[i+1]-features->ptr[i], labels->ptr[i+1]-labels->ptr[i]);
	
	csr_free(labels);
	csr_free(features);
	return 0;
}
