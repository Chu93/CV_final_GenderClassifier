/****************************************************************************/
/*Function：Human Pose Estimation for 1 pic
/* 
/* Chuyi Liu
/****************************************************************************/

#include <cstdio>  
#include <cstdlib>  
#include <cstring>
#include <iostream>
#include <ctime>

#include <opencv2/opencv.hpp> 
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/contrib/contrib.hpp>

using namespace cv;
using namespace std;

void showfiles();

void showfiles()
{
	string path="\\home\\chuyi\\HPE";
	Directory dir;
	vector<string> fileNames = dir.GetListFiles(path,"*.jpg",false);
	
	for(int i = 0; i < fileNames.size(); i++)  
	{  
		//get image name  
		string fileName = fileNames[i];  
		string fileFullName = path + fileName;  
		cout<<"File name:"<<fileName<<endl;  
		cout<<"Full path:"<<fileFullName<<endl;  
	  
		//load image  
		IplImage* srcImg = cvLoadImage(fileFullName.c_str(), -1);  
		cvShowImage("src", srcImg);  
		cvWaitKey(0);  
	}  
}


vector<Rect> detectFaces(Mat img_gray){
	CascadeClassifier faces_cascade;
	faces_cascade.load("haarcascade_frontalface_alt.xml");
	vector<Rect> faces;
	faces_cascade.detectMultiScale(img_gray,faces,1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
	return faces;
}


void drawFaces(Mat img,vector<Rect> faces){
	namedWindow("faces");
	for(size_t i=0;i<faces.size();i++){

		Point center(faces[i].x + faces[i].width/2,faces[i].y + faces[i].height/2);
		ellipse(img,center,Size(faces[i].width/2,faces[i].height/1.5),0,0,360,Scalar(0,255,0),2,8,0);
	}
	imshow("faces",img);
}


void saveFaces(Mat img,Mat img_gray){
	vector<Rect> faces = detectFaces(img_gray);
	for(size_t i=0; i<faces.size();i++){
		stringstream buffer;
		buffer<<i;
		string saveName = "face"+ buffer.str() + ".jpg";
		Rect roi = faces[i];
		imwrite(saveName,img(roi));
	}
}



int main(){

	showfiles();

	Mat img = imread("1.jpg");
	Mat img_gray;
	cvtColor(img,img_gray,COLOR_BGR2GRAY );
	equalizeHist(img_gray,img_gray);

	vector<Rect> faces = detectFaces(img_gray);
	saveFaces(img,img_gray);
	drawFaces(img,faces);

	waitKey(0);
	return 0;
}

