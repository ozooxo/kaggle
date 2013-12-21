## I use only the first part of the training set to form the set of matching tags.
## It doesn't seem changing the score a lot if not use exactly the whole training
## set, but it is much quicker. 
head -6000000 Train.csv > Sample.csv
## I use the last part of the training set for evalutiating my algorithm.
tail -2999989 Train.csv > Evaluation.csv

## Set up the MySQL database
mysql -u root < db_reset.sql

## Put the sample set into the database for future use.
python sample2db.py

## Doing statistics for the tags in the sample set. The methods I used include:
## (1) Find out the tags which are popular in the title. (2) Find the tags which
## always go together with some other tag using the apriori algorithm (to do
## that, I use Christian Borgelt's package from http://www.borgelt.net/apriori.html).
## (3) Find whether the most popular tags are in the body of the question or not.
## I only check a small amount of tags for the third part, because it's really time
## consuming to do that.
python tags_statistics.py
python tags_popular_statistics.py
sh tags_relations.sh
python tags_related_statistics.py
python tags_body_statistics.py

## I try to match my tags (in title, body, etc) with the evaluating set, check the
## rules for different tags, give every question some potential tags, and evaluate 
## the F-measure.
python evaluation.py

## Match the test set with the training set (I only try to match the first several
## characters of the title). If nobody matches, I use the algorithm in 
## "evaluation.py" to guess the tags.
python train2db.py
python test.py

## FIRST SUBMISSION ##############################################################
## 5000 popular tags
## 0.01 support for apriori relations
##--------------------------------------------------------------------------------
#Executed sample2db.py - Time: 94.9732420444  (F-measure: 0.525190716323)
#Executed tags_statistics.py - Time: 5.58588910103
#Executed tags_popular_statistics.py - Time: 68.5242230892
#./apriori - find frequent item sets with the apriori algorithm
#version 6.2 (2013.10.21)         (c) 1996-2013   Christian Borgelt
#reading tmp/tags.txt ... [25334 item(s), 249267 transaction(s)] done [0.14s].
#filtering, sorting and recoding items ... [7950 item(s)] done [0.01s].
#sorting and reducing transactions ... [148366/249267 transaction(s)] done [0.07s].
#building transaction tree ... [167715 node(s)] done [0.04s].
#checking subsets of size 1 2 done [0.60s].
#writing tmp/tags_relations1.txt ... [2892 rule(s)] done [0.33s].
#Executed tags_related_statistics.py - Time: 68.4228291512
#Executed evaluation.py - Time: 41.967952013  (F-measure: 0.354290439005)
#Executed test.py - Time: 1970.04095507
#In/out of the training set: 1167453/845884
##--------------------------------------------------------------------------------
## 1167453/(1167453+845884) + 845884/(1167453+845884)*0.354290439005 = 0.72871139
## Actual score 0.71969. A little bit lower, but matching is not so bad.

##################################################################################
## I corrected the "doesKeyInStr" function in "common_functions.py",
## so it is currently more consistent to what is used in countIncorrect.
## The main effect is that we'll no longer count mysql in 
## sentance "... mysqlsomething ..." except the only conditions
## [mysqled, mysqls, mysqling, mysql's]. Therefore, the F-measure for
## sample2db titleTags drops. However, I believe that most of the cases can
## be maked up by corrections of tags, if that "mysqlsomething" as a keyword
## does always induce the keyword "mysql" by rule. 
##
## I also counted the theoretical threshold for popular tags and 
## apriori support from "countYes" in title number. Currently:
## 4200 popular tags (threshold: count 5 events in sample)
## 0.002 support for apriori relations
##--------------------------------------------------------------------------------
#Executed sample2db.py - Time: 101.922752142  (F-measure: 0.467448438186)
#Executed tags_statistics.py - Time: 5.25429081917
#Executed tags_popular_statistics.py - Time: 115.949081898
#./apriori - find frequent item sets with the apriori algorithm
#version 6.2 (2013.10.21)         (c) 1996-2013   Christian Borgelt
#reading tmp/tags.txt ... [25334 item(s), 249267 transaction(s)] done [0.12s].
#filtering, sorting and recoding items ... [17698 item(s)] done [0.01s].
#sorting and reducing transactions ... [160214/249267 transaction(s)] done [0.11s].
#building transaction tree ... [188349 node(s)] done [0.04s].
#checking subsets of size 1 2 done [3.32s].
#writing tmp/tags_relations1.txt ... [14871 rule(s)] done [2.20s].
#Executed tags_related_statistics.py - Time: 309.11691308
#Executed evaluation.py - Time: 67.7658689022  (F-measure: 0.357991637252)

#### SECOND SUBMISSION ###########################################################
## 4200 popular tags (threshold: count 5 events in sample)
## 0.002 support for apriori relations
## 300 popular tags in body 
## threshold: 10 events of body-tag in sample
##--------------------------------------------------------------------------------
#Executed tags_body_statistics.py - Time: 123.978820801
#Executed evaluation.py - Time: 38.5010130405  (F-measure: 0.394526224886)
#Tag length distribution in evaluation.py: [0, 1657, 2577, 2204, 1313, 1800, 
#271, 116, 38, 15, 6, 1, 2, 0, 0, 0, 0, 0, 0, 0]
#Executed test.py - Time: 3740.35679984
#In/out of the training set: 1167453/845884
##--------------------------------------------------------------------------------
## 1167453/(1167453+845884) + 845884/(1167453+845884)*0.394526224886 = 0.74561
## Actual score 0.73711. A little bit lower. Try to match the reason.
## Can it raise because the kaggle calculator cannot handle questions with more
## than 5 tags?
## 1167453/(1167453+845884) + 845884/(1167453+845884)*(10000-271-116-38-15-6-1-2)/10000*0.394526224886 = 0.73817
## That probably is the reason.

#### THIRD SUBMISSION ############################################################
## If cut down the questions with more than 6 tags.
#Executed evaluation.py - Time: 38.8478548527  (F-measure: 0.394488253968)
#Tag length distribution in evaluation.py: [0, 1657, 2576, 2211, 1431, 2125, 
#0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
#Executed test.py - Time: 4884.83438015
#In/out of the training set: 1167453/845884
##--------------------------------------------------------------------------------
## kaggle gives 0.73710, so that's not the reason.

#### FOURTH SUBMISSION ###########################################################
## Add some special synonyms (e.g., "rails"->"ruby-on-rails")
## When matching body tags, replace "-" with " " (e.g., "ruby-on-rails"->"ruby on rails")
##--------------------------------------------------------------------------------
## 6000 popular tags (threshold: count 3 events in sample)
## 0.002 support for apriori relations
## 500 popular tags in body 
## threshold: 50 events of body-tag in sample
## (if lower it, there are to many and "evaluation.py" will be too slower)
##--------------------------------------------------------------------------------
#Executed train2db.py - Time: 1329.056283
#Executed sample2db.py - Time: 96.8032650948  (F-measure: 0.466230457543)
#Executed tags_statistics.py - Time: 5.62893390656
#Executed tags_popular_statistics.py - Time: 140.073144197
#./apriori - find frequent item sets with the apriori algorithm
#version 6.2 (2013.10.21)         (c) 1996-2013   Christian Borgelt
#reading tmp/tags.txt ... [25334 item(s), 249267 transaction(s)] done [0.15s].
#filtering, sorting and recoding items ... [17698 item(s)] done [0.01s].
#sorting and reducing transactions ... [160214/249267 transaction(s)] done [0.12s].
#building transaction tree ... [188349 node(s)] done [0.04s].
#checking subsets of size 1 2 done [3.68s].
#writing tmp/tags_relations1.txt ... [14871 rule(s)] done [2.69s].
#Executed tags_related_statistics.py - Time: 179.805109978
#Executed tags_body_statistics.py - Time: 193.845203876
#Executed evaluation.py - Time: 52.1878931522  (F-measure: 0.402955238095)
#Tag length distribution in evaluation.py: [0, 1553, 2548, 2183, 1382, 1783, 323, 145, 49, 22, 8, 2, 1, 0, 1, 0, 0, 0, 0, 0]
#Executed test.py - Time: 4464.16883397
#In/out of the training set: 1167388/845949
##--------------------------------------------------------------------------------
## 1167388/(1167388+845949) + 845949/(1167388+845949)*0.402955238095 = 0.74914
## Actual score 0.73981. The actually imporvement is tiny compare to Submission 2.

##################################################################################
## TODO
## better default tags -- ranking of wrong tags in evaluation.py?
## detect language for <code> blocks
