import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import LabelEncoder
from sklearn.neighbors import KNeighborsClassifier
from sklearn.svm import SVC
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import accuracy_score


file_path = '/Users/songmingyang/Desktop/Dataset/Sub_Dataset/FDB1_Encode.txt'
df = pd.read_csv(file_path, sep='\t')


print(df.head())


def bin_str_to_vector(bin_str):
    if isinstance(bin_str, str):
        bin_str = bin_str.strip('"')
        if all(bit in '01' for bit in bin_str):
            return [int(bit) for bit in bin_str]
        else:
            return []
    return []


df['reviewText'] = df['reviewText'].apply(bin_str_to_vector)
df['category'] = df['category'].apply(bin_str_to_vector)


max_len = max(df['reviewText'].apply(len).max(), df['category'].apply(len).max())

def pad_binary_str(bin_list, max_len):
    return bin_list + [0] * (max_len - len(bin_list))
    
df['reviewText'] = df['reviewText'].apply(lambda x: pad_binary_str(x, max_len))
df['category'] = df['category'].apply(lambda x: pad_binary_str(x, max_len))


X = df[['reviewText', 'category']]
y = df['overall']


import numpy as np
X = np.array([x[0] + x[1] for x in zip(X['reviewText'], X['category'])])
y = np.array(y)

X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.4, random_state=42)


from sklearn.preprocessing import StandardScaler
scaler = StandardScaler()
X_train = scaler.fit_transform(X_train)
X_test = scaler.transform(X_test)


# KNN
knn = KNeighborsClassifier(n_neighbors=3)
knn.fit(X_train, y_train)
knn_predictions = knn.predict(X_test)
knn_accuracy = accuracy_score(y_test, knn_predictions)

# SVM
svm = SVC(kernel='linear')
svm.fit(X_train, y_train)
svm_predictions = svm.predict(X_test)
svm_accuracy = accuracy_score(y_test, svm_predictions)


lr = LogisticRegression(max_iter=1000)
lr.fit(X_train, y_train)
lr_predictions = lr.predict(X_test)
lr_accuracy = accuracy_score(y_test, lr_predictions)


print(f"KNN Accuracy: {knn_accuracy}")
print(f"SVM Accuracy: {svm_accuracy}")
print(f"Logistic Regression Accuracy: {lr_accuracy}")
