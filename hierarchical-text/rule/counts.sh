gcc label_counts.c -o label_counts
#./label_counts ../data/train.csv > label_counts.txt

gcc feature_counts.c -o feature_counts
./feature_counts ../data/train.csv > feature_counts.txt
