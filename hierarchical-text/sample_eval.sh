rm data/train_train.csv
rm data/train_eval.csv

echo "Data" >> data/train_eval.csv

i=0

while read line
do
	if [ $(($i%1000-50)) -eq 0 ]
	then 
		echo $i
		echo $line >> data/train_eval.csv
	else
		echo $line >> data/train_train.csv
	fi
	i=$(($i + 1))
done < data/train.csv

echo $i
