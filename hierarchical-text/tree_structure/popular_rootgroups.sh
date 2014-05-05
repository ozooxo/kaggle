# find popular root groups
#./apriori -ts -m2 -s0.06 data/roottable_entries.txt data/popular_rootgroups_tmp.txt
./apriori -ts -m2 -s0.02 data/roottable_entries.txt data/popular_rootgroups.txt
python merge_forest.py


