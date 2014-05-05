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

