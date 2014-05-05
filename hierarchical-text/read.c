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

void read_test (FILE *test, csr *features) {
	
	while (fgetc(test) != '\n') ; // skip the first line -- "ID,Data"
	
	int nrows = 0, featureptr = 0;
	int buffer1, buffer2;
	
	features->ptr[0] = 0;
	fscanf(test, "%d", &buffer1);
	while (fgetc(test) != EOF) {
		fscanf(test, "%d", &buffer2);
		
		while (fscanf(test, "%d:%d", &buffer1, &buffer2) == 2) {
			features->idx[featureptr] = buffer1;
			features->val[featureptr] = buffer2;
			++featureptr;
		}
		features->ptr[nrows+1] = featureptr;
		
		++nrows;
		//if (nrows > 3) break;
	}
	features->nrows = nrows;
}
