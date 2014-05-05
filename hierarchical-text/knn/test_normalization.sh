gcc `pkg-config --cflags glib-2.0` knn_test_normalization.c -lm `pkg-config --libs glib-2.0` -o knn_test_normalization

./knn_test_normalization train_train.csv train_eval.csv 0.0 0.0 1.0 > pred_statistics_slope05_approach602.csv

./knn_test_normalization train_train.csv train_eval.csv 0.400655 0.0006649 0.59868 > pred_statistics_slope03_approach602.csv
./knn_test_normalization train_train.csv train_eval.csv -0.400655 -0.0006649 1.40134 > pred_statistics_slope07_approach602.csv
./knn_test_normalization train_train.csv train_eval.csv -0.801330 -0.00132992 1.80267 > pred_statistics_slope09_approach602.csv

./knn_test_normalization train_train.csv train_eval.csv 0.0010984 0.0010984 0.99781 > pred_statistics_slope05_approach1000.csv
./knn_test_normalization train_train.csv train_eval.csv 0.00248018 0.00248018 0.99505 > pred_statistics_slope05_approach1500.csv
./knn_test_normalization train_train.csv train_eval.csv 0.00386196 0.00386196 0.99228 > pred_statistics_slope05_approach2000.csv
./knn_test_normalization train_train.csv train_eval.csv 0.0121526 0.0121526 0.97570 > pred_statistics_slope05_approach5000.csv
./knn_test_normalization train_train.csv train_eval.csv 0.0256705 0.0256705 0.94810 > pred_statistics_slope05_approach10000.csv
