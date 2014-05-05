typedef struct {
	int idx;
	int val;
} dict;

int compare_dict (const void *a, const void *b) {
	// large to small val
	return (((dict*)b)->val - ((dict*)a)->val);
}

int find_dict_rank (int idx, dict *dicts, int n) {
	int i;
	for (i = 0; i < n; ++i) {
		if (idx == dicts[i].idx) return i+1;
	}
	return 0;
}
