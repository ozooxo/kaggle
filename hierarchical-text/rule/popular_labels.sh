gcc feature2labels.c -o feature2labels

python popular_features.py
python popular_labels.py

export IFS=" "

while read feature probability
	do
	./feature2labels ../data/train_1-100.csv 0.2 $feature
done < popular_features.txt
