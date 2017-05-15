import argparse
from patsy import dmatrices
from sklearn.linear_model import LogisticRegression
#from sklearn.cross_validation import train_test_split
from sklearn import metrics
#from sklearn.cross_validation import cross_val_score

import pandas as pd
import sys
import numpy as np

COLUMNS = ["subjectBound", "objectBound", "numberOfResults", "costOfComputing", "numberOfRules", "numberOfQueries", "numberOfUniqueRules", "algorithm"]

def perf_measures(yActual, yPredicted):
    TP = 0
    FP = 0
    FN = 0
    TN = 0

    if (len(yActual) != len(yPredicted)):
        print("FATAL", len(yActual) , " ! = " , len(yPredicted))
        return 1,1,1,1
    for i in range(len(yPredicted)):
        if yActual[i] == yPredicted[i]:
            TP += 1
            TN += 1
        else:
            FN += 1
            FP += 1
    return float(TP), float(FP), float(TN), float(FN)

def train_and_eval(train_file, test_file):
    df_train = pd.read_csv(
      train_file,
      names=COLUMNS,
      skipinitialspace=True,
      engine="python")
    df_test = pd.read_csv(
      test_file,
      names=COLUMNS,
      skipinitialspace=True,
      engine="python")

    # remove NaN elements
    df_train = df_train.dropna(how='any', axis=0)
    df_test = df_test.dropna(how='any', axis=0)

    y,X = dmatrices ('algorithm ~ subjectBound + objectBound + numberOfResults + \
    numberOfRules + numberOfQueries + numberOfUniqueRules', df_train, return_type = "dataframe")
    y = np.ravel(y)
    model = LogisticRegression()
    model = model.fit(X, y)

    yTest, xTest = dmatrices ('algorithm ~ subjectBound + objectBound + numberOfResults + \
    numberOfRules + numberOfQueries + numberOfUniqueRules', df_test, return_type = "dataframe")
    # check the accuracy on the training set

    predicted = model.predict(xTest)

    TP, FP, TN, FN = perf_measures(list(yTest.values), list(predicted))

    precision = TP / (TP + FP)
    recall = TP / (TP + FN)
    accuracy = TP + TN / (TP + TN + FP + FN)
    #print (metrics.accuracy_score(yTest, predicted))
    print ("Precision = ", precision)
    print("Recall = ", recall)
    print("Accuracy = ", accuracy)

def parse_args():
    parser = argparse.ArgumentParser(description = "Simple linear model")
    parser.add_argument('--train_file', type=str, required=True, help='Training data csv file')
    parser.add_argument('--test_file', type=str, required=True, help='Test data csv file')
    return parser.parse_args()

args = parse_args()
train_and_eval(args.train_file, args.test_file)