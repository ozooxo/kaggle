#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <glib.h>

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
#define TEST_NFEATUREDATA TEST_NROWS*100

#define INT_SCALE 10000 // used to scale the "val" (INT) contribute in "csr train_feature"
#define PRED_THRESHOLD 10 // when calculating similar rows, ones with similarity more than
                          // 1/PRED_THRESHOLD of the maximal similarity will count.
#define LABEL_THRESHOLD 0.05 // probability for a label to be predicted stay

#include "../dict.c"
#include "../csr.c"
#include "../read.c"

////////////////////////////////////////////////////////////////////////

//void sort_csr_val(csr *mat, )

void scale_tfidf (csr *mat, csr *mat_attach, int ncols, double a, double b, double c) { 
	// ncols = max(mat->ncol), but it is costly the calculate that.
	// it is fine in here to choose any number which is larger than that.
	
	int nrow, i;
	
	int *frequency = malloc(ncols*sizeof(int));
	for (i = 0; i < ncols; ++i) frequency[i] = 0;
	
	for (nrow = 0; nrow < mat->nrows; ++nrow) {
		for (i = mat->ptr[nrow]; i < mat->ptr[nrow+1]; ++i) frequency[mat->idx[i]] += 1;
	}
	
	for (i = 0; i < ncols; ++i) frequency[i] = (int)((double)INT_SCALE/(a + b*(frequency[i]+1) + c*sqrt(frequency[i]+1)));

	for (i = 0; i < mat->ptr[mat->nrows]; ++i) mat->val[i] *= frequency[mat->idx[i]];
	if (mat_attach != NULL)
		for (i = 0; i < mat_attach->ptr[mat_attach->nrows]; ++i) mat_attach->val[i] *= frequency[mat_attach->idx[i]];

	//printf("%f\n\n", log(ncols+2));
	free(frequency);
}

void normalization (csr *mat) { 
	int nrow, i;
	long long absrow;
	for (nrow = 0; nrow < mat->nrows; ++nrow) {
		absrow = 0;
		for (i = mat->ptr[nrow]; i < mat->ptr[nrow+1]; ++i) 
			absrow += (long long)(mat->val[i]) * (long long)(mat->val[i]);
		absrow = (long long)sqrt(absrow);
		for (i = mat->ptr[nrow]; i < mat->ptr[nrow+1]; ++i) 
			mat->val[i] = INT_SCALE*mat->val[i]/absrow;
	}
}

csr *transpose (csr *mat, int ncols) { 
	// ncols = max(mat->ncol), but it is costly the calculate that.
	// it is fine in here to choose any number which is larger than that.
	
	int nrow, ncol, i;
	int ncolmax = 0;
	
	GSList **colidx = malloc(ncols*sizeof(GSList*));
	GSList **colval = malloc(ncols*sizeof(GSList*));
	for (i = 0; i < ncols; ++i) {
		colidx[i] = NULL;
		colval[i] = NULL;
	}
	
	for (nrow = 0; nrow < mat->nrows; ++nrow) {
		for (i = mat->ptr[nrow]; i < mat->ptr[nrow+1]; ++i) {
			ncol = mat->idx[i];
			ncolmax = (ncol > ncolmax ? ncol : ncolmax);
			colidx[ncol] = g_slist_prepend(colidx[ncol], GINT_TO_POINTER(nrow));
			colval[ncol] = g_slist_prepend(colval[ncol], GINT_TO_POINTER(mat->val[i]));
		}
	}

	csr *tr = csr_malloc(ncolmax+1, mat->ptr[mat->nrows]);
	tr->nrows = ncolmax+1;
	//printf("%d\n", ncolmax);

	GSList *p, *q;
	tr->ptr[0] = 0;
	i = 0;
	for (ncol = 0; ncol < (ncolmax + 1); ++ncol) {
		for (p = colidx[ncol], q = colval[ncol]; p != NULL; p = p->next, q = q->next) {
			tr->idx[i] = GPOINTER_TO_INT(p->data);
			tr->val[i] = GPOINTER_TO_INT(q->data);
			++i;
		}
		tr->ptr[ncol+1] = i;
		//printf("%d %d\n", ncol, tr->ptr[ncol+1]-tr->ptr[ncol]);
	}
	
	for (i = 0; i < ncols; ++i) {
		g_slist_free(colidx[i]);
		g_slist_free(colval[i]);
	}
	free(colidx);
	free(colval);
	
	return tr;
}

////////////////////////////////////////////////////////////////////////

void print_rowlabels (csr *labels, int nrow) {
	int i;
	for (i = labels->ptr[nrow]; i < labels->ptr[nrow+1]; ++i) 
		printf("%d ", labels->idx[i]);
	printf("\n");
}

void print_rowfeatures (csr *features, int nrow) {
	int i;
	for (i = features->ptr[nrow]; i < features->ptr[nrow+1]; ++i) 
		printf("%d:%d ", features->idx[i], features->val[i]);
	printf("\n");
}

////////////////////////////////////////////////////////////////////////

//void pred_row_label (csr *features, int nrow, csr *train_labels, csr *tr_train_features) {
void pred_row_label (csr *features, int nrow, csr *train_labels, csr *tr_train_features, csr *test_labels) {
	
	int i, j, k, l;
	
	dict *similarity = malloc((train_labels->nrows)*sizeof(dict));
	for (i = 0; i < train_labels->nrows; ++i) {
		similarity[i].idx = i;
		similarity[i].val = 0;
	}
	
	for (i = features->ptr[nrow]; i < features->ptr[nrow+1]; ++i)  {
		k = features->idx[i];
		for (j = tr_train_features->ptr[k]; j < tr_train_features->ptr[k+1]; ++j) {
			// scaled cos similarity
			similarity[tr_train_features->idx[j]].val += tr_train_features->val[j] * features->val[i];
		}
	}
	
	qsort(similarity, train_labels->nrows, sizeof(dict), compare_dict);
	
	int similarity_maxval = similarity[0].val;
	//for (i = 0; i < 10; ++i)//similarity[i].val > similarity_maxval/PRED_THRESHOLD; ++i) 
	//	printf("%d %d\n", similarity[i].idx, similarity[i].val);
	//printf("\n");
	
	dict *labels = malloc(NLABELS*sizeof(dict));
	for (i = 0; i < NLABELS; ++i) {
		labels[i].idx = i;
		labels[i].val = 0;
	}
	
	long long similarity_sumval = 0;
	for (i = 0; i < 3 || similarity[i].val > similarity_maxval/PRED_THRESHOLD; ++i) {
		k = similarity[i].idx;
		similarity_sumval += similarity[i].val;
		for (j = train_labels->ptr[k]; j < train_labels->ptr[k+1]; ++j) {
			//if ((train_labels->idx[j] > 445727) || (train_labels->idx[j] < 2)) printf("--%d\n", train_labels->idx[j]);
			labels[train_labels->idx[j]].val += similarity[i].val;
		}
	}
	
	qsort(labels, NLABELS, sizeof(dict), compare_dict);
	
	//for (i = 0; i < 10 || 1.0L*labels[i].val/similarity_sumval > LABEL_THRESHOLD; ++i)
	//	printf("%d:%d ", labels[i].idx, labels[i].val);
	
	for (i = test_labels->ptr[nrow]; i < test_labels->ptr[nrow+1]; ++i) {
		j = test_labels->idx[i];
		printf("%d:%d ", j, find_dict_rank(j, labels, 100));//NLABELS));
	}
	
	free(similarity);
	free(labels);
}

////////////////////////////////////////////////////////////////////////

int main (int argc, char *argv[]) {
	if (argc != 6) {
		printf( "Usage:\n  knn [TRAIN FILE] [EVAL or TEST FILE] [PARAS ...]\n\n");
		return 1;
	}
	
	int i;

	//------------------------------------------------------------------

	csr *train_labels = csr_malloc(TRAIN_NROWS, TRAIN_NLABELDATA);
	csr *train_features = csr_malloc(TRAIN_NROWS, TRAIN_NFEATUREDATA);

	FILE *train;
	train = fopen(argv[1], "r");
	read_train(train, train_labels, train_features);
	fclose(train);
	
	//printf("finish reading train data!\n");
	
	//print_rowlabels(train_labels, labels->nrows-1);
	//print_rowfeatures(train_features, labels->nrows-1);

	//print_rowfeatures(tr_features, 1062588);
	
	//print_rowlabels(train_labels, 1);
	//print_rowfeatures(train_features, 0);
	
	//------------------------------------------------------------------

	csr *eval_labels = csr_malloc(EVAL_NROWS, EVAL_NLABELDATA);
	csr *eval_features = csr_malloc(EVAL_NROWS, EVAL_NFEATUREDATA);

	FILE *eval;
	eval = fopen(argv[2], "r");
	read_train(eval, eval_labels, eval_features);
	fclose(eval);
	
	double a, b, c, d;
	a = atof(argv[3]);
	b = atof(argv[4]);
	c = atof(argv[5]);
	
	scale_tfidf(train_features, eval_features, NFEATURES, a, b, c);
	normalization(train_features);
	normalization(eval_features);
	csr *tr_train_features = transpose(train_features, NFEATURES);
	//transpose(train_labels, NLABELS);
	//printf("pass tr\n");
	csr_free(train_features);
	
	//pred_row_label(eval_features, 0, train_labels, tr_train_features);
	//pred_row_label(eval_features, 0, train_labels, tr_train_features, eval_labels);
	//printf("\n");
	//pred_row_label(eval_features, 1, train_labels, tr_train_features, eval_labels);
	//printf("\n");
	//pred_row_label(eval_features, 2, train_labels, tr_train_features, eval_labels);
	//printf("\n");

 	printf("Id,Tag:Rank\n");
	for (i = 0; i < eval_features->nrows; ++i) {
		printf("%d,", i+1);
		pred_row_label(eval_features, i, train_labels, tr_train_features, eval_labels);
		printf("\n");
	}
	
	csr_free(eval_labels);
	csr_free(eval_features);
	
	//------------------------------------------------------------------
/*	
	csr *test_features = csr_malloc(TEST_NROWS, TEST_NFEATUREDATA);
	
	FILE *test;
	test = fopen(argv[2], "r");
	fclose(test);
	
	read_test(test, test_features);
	//print_rowfeatures(test_features, 1);
	//print_rowfeatures(test_features, test_features->nrows-1);
	
*/
	csr_free(train_labels);
	csr_free(tr_train_features);
	return 0;
}
