factor=12

rm toys_rev2_sample$factor.csv
echo "ToyId,Arrival_time,Duration" >> toys_rev2_sample$factor.csv

i=0

while read line
do
	if [ $(($i%$factor-5)) -eq 0 ]
	then 
		echo $i
		echo $line >> toys_rev2_sample$factor.csv
	fi
	i=$(($i + 1))
done < toys_rev2.csv

echo $i
