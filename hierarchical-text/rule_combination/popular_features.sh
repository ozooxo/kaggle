gcc features.c -o features

grep ' '$1',\|^'$1',\| '$1' \|^'$1' ' ../data/train_1-100.csv > data/train_$1.csv
./features data/train_$1.csv > data/features_$1.csv

./apriori -ts -s10 data/features_$1.csv data/popular_features_$1.csv
