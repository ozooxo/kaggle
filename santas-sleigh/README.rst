======================
Packing Santa's Sleigh
======================

This is my solution for kaggle compitation `Packing Santa's Sleigh`_. My final rank is 65th/362.

.. _Packing Santa's Sleigh: http://www.kaggle.com/c/packing-santas-sleigh

Data Properties
---------------

From some simple plotting of the distribution of the present data set, we can see that the present sizes are not random.

.. image:: https://raw.github.com/ozooxo/kaggle/master/santas-sleigh/pictures/Volumn_Distribution.png
   :height: 505 px
   :width: 919 px
   :scale: 100 %
   :alt: Volumn_Distribution.png
   :align: center

.. image:: https://raw.github.com/ozooxo/kaggle/master/santas-sleigh/pictures/Edge_Length_Distribution.png
   :height: 972 px
   :width: 559 px
   :scale: 100 %
   :alt: Edge_Length_Distribution.png
   :align: center

.. image:: https://raw.github.com/ozooxo/kaggle/master/santas-sleigh/pictures/Edge_Length_Distribution_presents2-3-4.png
   :height: 972 px
   :width: 560 px
   :scale: 100 %
   :alt: Edge_Length_Distribution_presents2-3-4.png
   :align: center

From smaller to larger, we classify all presents into four classes based on the above information. 

+----------------+---------------------------------------------------------+
| ``presents_1`` | ``(large edge) <= 10``                                  |
+----------------+---------------------------------------------------------+
| ``presents_2`` | ``11 <= (large edge) <= 70`` and ``(small edge) <= 64`` |
+----------------+---------------------------------------------------------+
| ``presents_3`` | ``(large edge) >= 71`` and ``(small edge) <= 64``       |
+----------------+---------------------------------------------------------+
| ``presents_4`` | ``(large edge) >= 71`` and ``(small edge) >= 65``       |
+----------------+---------------------------------------------------------+

From the "Faces Distribution plot", we can see that this classification makes sense.

.. image:: https://raw.github.com/ozooxo/kaggle/master/santas-sleigh/pictures/Faces_Distribution.png
   :height: 972 px
   :width: 564 px
   :scale: 100 %
   :alt: Faces_Distribution.png
   :align: center

Those classes has the following additional properties:

- "Volumn Distribution histogram plot" can be splitted quite good by ``presents_1``, ``2``, and ``4``.

- ``presents_3`` has a meaning from the "Faces Distribution graph".

- ``presents_1``, ``presents_2``, and ``presents_4`` all have the property that ``large edge`` aggregate to their maximum value, ``small edge`` aggregate to their mininum value, while ``median edge`` tend to stay in the middle.

  +----------------+---------------------+-------------------------+
  |                | ``max(large edge)`` | ``min(small edge)``     |
  +----------------+---------------------+-------------------------+
  | ``presents_1`` | ``10``              | ``2``                   |
  +----------------+---------------------+-------------------------+
  | ``presents_2`` | ``70``              | ``2`` (mostly ``>= 5``) |
  +----------------+---------------------+-------------------------+
  | ``presents_4`` | ``250``             | ``65`` (max ``247``)    |
  +----------------+---------------------+-------------------------+

  ``presents_3`` has really flat distribution with ``large edge``.

  +----------------+---------------------+-------------------------+-------------------------+
  |                | ``large edge``      | ``median edge``         | ``small edge``          |
  +----------------+---------------------+-------------------------+-------------------------+
  | ``presents_3`` | ``70`` ~ ``100``    | ``5`` ~ ``45``          | ``5`` ~ ``45``          |
  +----------------+---------------------+-------------------------+-------------------------+

- ``presents_4`` has ``(small edge)/(large edge) >= 0.2600`` and has a smooth distribution with an maximum of around ``0.4``. Notice that ``0.26`` is caused by ``65/250``, while ``0.4`` tells that for the presents in the small ``small edge`` limit, the most probable ``large edge`` has length ``65/0.4 = 162.5``.

.. image:: https://raw.github.com/ozooxo/kaggle/master/santas-sleigh/pictures/Shape_Distribution.png
   :height: 967 px
   :width: 1077 px
   :scale: 100 %
   :alt: Shape_Distribution.png
   :align: center

- ``presents_1`` has ``(small edge)/(large edge) >= 0.2000`` (caused by ``2/10``), but it doesn't have any good distribution.

- The pattern in "Sharp Distribution plot" of ``presents_1``, ``2``, and ``3`` are not interesting, because it is just caused by the integer edges (so rational number for their ratio). This effect is always more significant when the package is smaller. The gaps near the pattern lines are also caused by the same reason. The two boundary lines on top and right are caused by the case when two edges have the same length, so they are also not special cases.

- There are the sum of volumn of all presents in different class. So the mean job is to organize ``presents_4``.

  +----------------+------------+
  | ``presents_1`` | 7.5821e+7  |
  +----------------+------------+
  | ``presents_2`` | 1.9941e+10 |
  +----------------+------------+
  | ``presents_3`` | 1.6338e+09 |
  +----------------+------------+
  | ``presents_4`` | 7.8438e+11 |
  +----------------+------------+

- From the "Timestream Distribution histogram plot", ``presents_3`` only stays in id region ``[13, 399480]``, while ``presents_4` only stays in id region ``[2, 698904]``. I have double checked that for different regions of time, the distribution of presents in a defined class have roughly the same property; so there's no reason to futher classify presents by their time.

.. image:: https://raw.github.com/ozooxo/kaggle/master/santas-sleigh/pictures/Timestream_distribution.png
   :height: 495 px
   :width: 724 px
   :scale: 100 %
   :alt: Timestream_distribution.png
   :align: center

Algorithm
---------

My final algorithm has been shown in ``Packing_8-organize-boundary.py``.

By the `evaluation metric`_ of the compitation, I believe that the penalty for ordering is always too large, so there's no reason to disrupt the order. In addition, since packages with the same top edge will be resorted, packing in layers with the same top edge will get no penalty. This configuration is definitely quite unnatural and counterintuitive (in a real world with gravity), but let's just assume that Santa has special frames to hold his presents like that.

.. _evaluation metric: http://www.kaggle.com/c/packing-santas-sleigh/details/evaluation

Since the small presents are always much smaller than the larger ones, the key point of this project is how to pack up the large presents without disrupt the order. After that, it is always easy to pack small presents properly in the gaps. Therefore, I focus on packing ``presents_4`` in the region ``[2, 698904]``. For region ``[698905, 1000000]``, we can imitate the way of packing ``presents_4`` to ``presents_2``; also, the contribution to the score for this region is much smaller, therefore any packing methods doesn't affect much.

Since the ``large edge`` of ``presents_4`` aggregates to ``250``, we use the ``large edge`` as the z-direction of the sleigh. Then we order the ``median edge`` from large to small and pack alternately from left to right or from right to left. We handle the gaps by some recursive process. When one layer is finished, we move on to the next layer which is roughly ``250`` lower. This process, as shown in ``Packing_4-by-layer-triangle.py``, can get us a score of ``2134052``. The packing of the first ``226`` presents are shown as below.

.. image:: https://raw.github.com/ozooxo/kaggle/master/santas-sleigh/pictures/Packing_4-by-layer-triangle.png
   :height: 529 px
   :width: 502 px
   :scale: 100 %
   :align: center

Then we try to recycled the top gap above ``presents_1``, ``presents_2``, and ``presents_3`` to pack some more presents. To achieve that, we concentrate the the top-left corner of a layer by some minor changes of the layer packing strategies. The final process, as shown in ``Packing_8-organize-boundary.py``, give us a score of ``2072730``. For example, for the very first presents, we packed ``225`` presents in the layer, and then packed ``4`` more presents in the recycled top corner.

.. image:: https://raw.github.com/ozooxo/kaggle/master/santas-sleigh/pictures/Packing_8-organize-boundary_layer.png
   :height: 529 px
   :width: 502 px
   :scale: 100 %
   :align: center

.. image:: https://raw.github.com/ozooxo/kaggle/master/santas-sleigh/pictures/Packing_8-organize-boundary_top.png
   :height: 529 px
   :width: 502 px
   :scale: 100 %
   :align: center
