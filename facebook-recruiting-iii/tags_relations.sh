mysql -u root < tags_output.sql
sudo mv /tmp/tags.txt tmp/tags.txt
./apriori -tr -s0.002 -c25 -m2 -n2 tmp/tags.txt tmp/tags_relations1.txt
#./apriori -tr -s0.01 -c25 -m3 -n3 tmp/tags.txt tmp/tags_relations2.txt
