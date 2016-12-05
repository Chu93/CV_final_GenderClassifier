# CV_final_GenderClassifier

compile gender.cpp with following command:

       g++ gender.cpp -o gender `pkg-config --cflags --libs opencv`
       
To run:

       ./gender att_faces/att.csv att_faces/female/s8/4.pgm

Theoratically, this program can resize training samples automatically and apply different .csv files and test images(just call them with correct path in command).

But when I wanted to use CAS-PEAL(Though with a drawback: it contains only Asian people's faces) to train it, it always give me some error like:

The matrix is not continuous, thus its number of rows can not be changed in function reshape...

or:

Image step is wrong (The matrix is not continuous, thus its number of rows can not be changed) in reshape, file line 802
 
