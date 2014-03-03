=============================
Conway's Reverse Game of Life
=============================

This is my solution for kaggle compitation `Conway's Reverse Game of Life`_. My final rank is 10th/145.

.. _Conway's Reverse Game of Life: http://www.kaggle.com/c/conway-s-reverse-game-of-life

Algorithm
=========

I am using a pure statistical method to solve this problem. Specifically speaking, I traverse all the different patterns in the stopping board, and count whether the center grid of that pattern in the starting board is alive or dead. The statistics is done seperately for different ``delta``. If there are more alive grids then the dead grids, I predict the reverse grid to be alive; otherwise dead. While I can just use the training set to do the counting and statistics, it is too small to get stable statistical result; therefore, I created my own training set by the random process described by `this URL`_.

.. _this URL: http://www.kaggle.com/c/conway-s-reverse-game-of-life/data

So the two bottlenecks for this algorithm is RAM and CPU clock rate. RAM decides how large the pattern I can test and exam, and CPU clock rate decides how large my training set is, therefore how stable my statistics is towarding the converging value.

Each grid pattern can be written as a binary string (e.g., 0 for alive and 1 for dead), so we can use an INT (32-bits) array to count the different cases. I have a 8G RAM in my desktop; however, I am using a 32-bit ``gcc`` for which the pointer address can only support 4G. Therefore, in a naive way the maximal pattern size is

::

	log((4*(2^30) / 4) / (5*2)) / log(2) = 26.6

that is, roughly the ``5*5`` grid pattern. Actually, even that can give us quite good result (roughly ``0.1204`` for training set, ``0.1179`` for public test set).

I construacted some ways to further increase the pattern set.

One way to do it is as follows. As we only care about the possible patterns in the test set, so the number of patters we need to count and vote has a maximum of ``50000*20*20/5 = 4000000`` for each ``delta``; however, as there are many duplicated patterns (such as the all dead one) which dominate the statistics, the actually unique pattern number is roughly ``1000000``. Then, we can use a binary table to record whether any pattern need to be counted or not, and a integer voting table to record the counts of (hashed) patterns.

Suppose we have 2G space to record the pattern existence, the pattern size should be

::

    log(2*(2^30)*8 / 5) / log(2) = 31.6

And to use the another 2G for the voting table, the filling rate is roughly

::

    (2*5*1000000) / (2*(2^30) / 4) = 0.019

Therefore, for a proper designed hash function, the crashing of voting tables between different patterns can be in general ignored.

The final pattern I am using is 29-bit. It is shown as below. The grids which have the same number will be count alive if any grid in it is alive (a.k.a OR operation).

== == == == == == ==
      01 01 01      
   02 03 04 05 06   
07 08 09 10 11 12 13
07 14 15 16 17 18 13
07 19 20 21 22 23 13
   24 25 26 27 28   
      29 29 29
== == == == == == ==

I didn't use any parallel computing algorithms to speed up my calculation. The final running time is roughly 2 days in my 300 USD desktop. The running file is ``conway_d-pattern-matching_combine37.c``, it has a public score of ``0.11488`` and a private score of ``0.11584``.

Alternative Approaches
======================

Parallel computing
------------------

I should definitely try that in this problem.

Symmetries
----------

I should count the parity and the rotation symmetries of the pattern. In that case, I can win 3 more bits of my pattern. However, I didn't find out any easy-to-use hash function to do that.

Boundaries
----------

I should make boundary grid special while matching the pattern. However, because of limited RAM size, and a lack of symmetry-related function, I didn't do that. However, boundary may actually predicted batter than center grids, because they are more dominated by dead grids.

Spedial patterns
----------------

There are several special patterns (block, beehive, loaf, boat ...) exist in conway's games. They can always be reserved in this statistical approach.

Reverse step-by-step
--------------------

I formerly did some statistics based on single step, and reverse for multiple steps. Actually, by carefully give a the voting bias to the dead grids, the prediction is better than my final approach (see e.g. ``conway_4-reverse_21_more-careful-cutoff.c``). That's probably because this method can actually consider larger regions than the pattern size. However, it doesn't give good enough performance when the pattern size / training set size go larger (probably that's just because of code bugs), and this method cannot be used together with the test set pattern existance table.

Back-and-force check
--------------------

We can let the pattern go back-and-force to check for prediction accurancies. However, my current approach based on this method doesn't help much.

Larger pattern
--------------

Based on my current algorithm, my pattern can has a maximum of 31-bits, but I only use 29-bits. It can be imaging that a larger pattern can cause a higher accurancy; however, it may need us to have a much larger training set to converge the predicted score.

Better design for pattern combination
-------------------------------------

I currently use 3-grids OR for faraway grid, which gives an improvement of ``0.0005``. It can be imaging that a better pattern combination can help more; however, I have no theoretical idea of how to design that, and a brute-force parameter space test is too time consuming.

Hash pattern before existance table
-----------------------------------

It should be reasonable to hash the existance table before we make the existance table, because some patterns are just theoretical impossible for conway simulations (a forum article said the existance possibility goes down to roughly 3% for a 5x5 table in 5th step). However, I tried to go that way, but my score goes worse.
