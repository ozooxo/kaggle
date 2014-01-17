=======================
Facebook Recruiting III
=======================

This is my solution for kaggle compitation `Facebook Recruiting III - Keyword Extraction`_. My final rank is 69th/367.

.. _Facebook Recruiting III - Keyword Extraction: http://www.kaggle.com/c/facebook-recruiting-iii-keyword-extraction

Algorithm
=========

I use only the first part of the training set to form the set of matching tags. It doesn't seem changing the score a lot if not use exactly the whole training set, but it is much quicker. 

::

    $ head -6000000 Train.csv > Sample.csv

I use the last part of the training set for evalutiating my algorithm.

::

    $ tail -2999989 Train.csv > Evaluation.csv

Set up the MySQL database

::

    $ mysql -u root < db_reset.sql

Put the sample set into the database for future use.

::

    $ python sample2db.py

Doing statistics for the tags in the sample set. The methods I used include: (1) Find out the tags which are popular in the title. (2) Find the tags which always go together with some other tag using the apriori algorithm (to do that, I use Christian Borgelt's package from http://www.borgelt.net/apriori.html). (3) Find whether the most popular tags are in the body of the question or not. I only check a small amount of tags for the third part, because it's really time consuming to do that.

::

    $ python tags_statistics.py
    $ python tags_popular_statistics.py
    $ sh tags_relations.sh
    $ python tags_related_statistics.py
    $ python tags_body_statistics.py

I try to match my tags (in title, body, etc) with the evaluating set, check the rules for different tags, give every question some potential tags, and evaluate the F-measure.

::

    $ python evaluation.py

Match the test set with the training set (I only try to match the first several characters of the title). If nobody matches, I use the algorithm in "evaluation.py" to guess the tags.

::

    $ python train2db.py
    $ python test.py
