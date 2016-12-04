# CV_final_GenderClassifier

compile gender.cpp with following command:

       g++ gender.cpp -o gender `pkg-config --cflags --libs opencv`

I failed to use a .jpg image from another dataset as test image.
And, the accuracy is awful, I wanna add more female samples to train the classifier.

I wanted to use CAS-PEAL to train it but it always give me some error like:

The matrix is not continuous, thus its number of rows can not be changed in function reshape...

or:

Image step is wrong (The matrix is not continuous, thus its number of rows can not be changed) in reshape, file line 802
 
