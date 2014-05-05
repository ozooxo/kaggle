# The data set is not uniform at all (see the plots and countings in read_train.py).
# It has 2365437 lines (so 2365436 entries) in all.

rm data/train_1-100.csv

i=0

while read line
do
	if [ $(($i%100)) -eq 0 ]
	then 
		echo $i
		echo $line >> data/train_1-100.csv
	fi
	i=$(($i + 1))
done < data/train.csv

echo $i
