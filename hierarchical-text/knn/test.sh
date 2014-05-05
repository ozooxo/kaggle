gcc `pkg-config --cflags glib-2.0` knn.c -lm `pkg-config --libs glib-2.0` -o knn

#./knn ../data/train_train.csv ../data/train_eval.csv > ../evaluation/pred_statistics_knn.csv
./knn ../data/train_train.csv ../data/train_eval.csv > ../evaluation/pred_votevalue_knn.csv
###./knn ../data/train.csv ../data/test.csv > ../prediction/pred_votevalue_knn.csv
