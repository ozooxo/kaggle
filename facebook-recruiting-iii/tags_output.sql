USE facebook3;
SELECT tags FROM sample INTO OUTFILE '/tmp/tags.txt'

# local filepath doesn't work,
# so I can only use sudo mv to move the file :-(
