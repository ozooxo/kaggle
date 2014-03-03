edge <- 20

showGrid <- function(lst, title) {
  image(1:edge, 1:edge, array(as.integer(lst), dim=c(edge,edge)), main=title, xlab="", ylab="", asp=1)
}

showTrain <- function(train, n) {
  par(mfrow=c(1,2))
  showGrid(train[n,][(3:(edge^2+2))], "start")
  showGrid(train[n,][((edge^2+3):(2*edge^2+2))], "stop")
}

#train <- read.csv("train.csv", nrows = 100)
#showTrain(train, 2)

showTrainPred <- function(train, pred, n) {
  par(mfrow=c(2,2))
  showGrid(train[n,][(3:(edge^2+2))], "start")
  showGrid(train[n,][((edge^2+3):(2*edge^2+2))], paste("stop -", train[n,2]))
  showGrid(pred[n,][(2:(edge^2+1))], paste("pred -", train[n,2]))
  showGrid(pred[n,][((edge^2+2):(2*edge^2+1))], "diff")
}

#pred <- read.csv("pred.csv", nrows = 100)
#showTrainPred(train, pred, 2)

showTestSubmission <- function(test, submission, n) {
  par(mfrow=c(1,2))
  showGrid(submission[n,][(2:(edge^2+1))], "submission")
  showGrid(test[n,][(3:(edge^2+2))], paste("test -", test[n,2]))
}

#test <- read.csv("test.csv", nrows = 100)
#submission <- read.csv("submission.csv", nrows = 100)
#showTestSubmission(test, submission, 1)