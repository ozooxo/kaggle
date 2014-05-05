rm data/test_small.csv

i=0

while read line
do
	if [ $(($i%100)) -eq 0 ]
	then 
		echo $i
		echo $line >> data/test_small.csv
	fi
	i=$(($i + 1))
done < data/test.csv

echo $i
