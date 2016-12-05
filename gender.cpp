#include <iostream>
#include <sstream>
#include <fstream>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

#define WIDTH 92
#define HEIGHT 112

void read_csv(string& fileName,vector<Mat>& images,vector<int>& labels,char separator = ';')
{
    ifstream file(fileName.c_str(),ifstream::in);    //Open the file
    String line,path,label;
    while (getline(file,line))	//Read a line
    {
        stringstream lines(line);	// Read a line
        getline(lines,path,separator);  // Path + Separator(;)
        getline(lines,label);	// Then Label
        if (!path.empty()&&!label.empty())  // If succeed
        {
            //images.push_back(imread(path,1));        //read sample(color)
			images.push_back(imread(path,0));		//read sample(gray)
            labels.push_back(atoi(label.c_str()));   //read label
        }
    }
}

int resizeSamples(string PATH, string Prefix,int START,int END)
{
    Mat image;
	string ImgName;
	
	int n;
	string s;
	
	for(n=START;n<=END;n++)
	{
		stringstream num;
		num<<n;
		s=num.str();
			
		ImgName=PATH+Prefix+s+".bmp";
		image = imread(ImgName,CV_LOAD_IMAGE_COLOR);

		Mat newImg;
		if(! image.data )                              // Check for invalid input
		{
			cout <<  "Could not open or find face_" << s << ".bmp" << std::endl ;
		}
		else
		{
			//namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
			//imshow( "Display window", image );                   // Show our image inside it.
			resize(image,newImg,Size(WIDTH,HEIGHT));
			imwrite(ImgName,newImg);
			cout << "Resize face_" << s << ".bmp to " << newImg.cols << "x" << newImg.rows << endl;
		}

	}

    waitKey(0);                                          // Wait for a keystroke in the window
    return 0;
}


void detection( Mat& img, CascadeClassifier& cascade, CascadeClassifier& nestedCascade, double scale, bool tryflip )
{	
	int i = 0;

	vector<Rect> faces, faces2;	    //vector to store faces info
	Mat gray, smallImg( cvRound (img.rows/scale), cvRound(img.cols/scale), CV_8UC1 );
	
	//Haar Classifier is trained with gray scale image, turn img into gray scale
	cvtColor( img, gray, CV_BGR2GRAY );

	//enhance images brightness and contrast
	resize( gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR );
	equalizeHist( smallImg, smallImg );	

	cascade.detectMultiScale( smallImg, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE,Size(30, 30));
		
    //detect again to enhance accurancy
    if( tryflip )
    {
        flip(smallImg, smallImg, 1);
		cascade.detectMultiScale( smallImg, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE,Size(30, 30));

        for( vector<Rect>::const_iterator r = faces2.begin(); r != faces2.end(); r++ )
        {
            faces.push_back(Rect(smallImg.cols - r->x - r->width, r->y, r->width, r->height));
        }
    }

 
    for( vector<Rect>::const_iterator r = faces.begin(); r != faces.end(); r++ )
    {
        Mat smallImgROI;
        vector<Rect> nestedObjects;
        Point center;
		Scalar color = CV_RGB(0,0,255);
        int radius;

        double aspect_ratio = (double)r->width/r->height;
        if( 0.75 < aspect_ratio && aspect_ratio < 1.3 )
        {
            //resize image
            center.x = cvRound((r->x + r->width*0.5)*scale);
            center.y = cvRound((r->y + r->height*0.5)*scale);
            radius = cvRound((r->width + r->height)*0.25*scale);
            circle( img, center, radius, color, 3, 8, 0 );
        }
        else
            rectangle( img, cvPoint(cvRound(r->x*scale), cvRound(r->y*scale)),
                       cvPoint(cvRound((r->x + r->width-1)*scale), cvRound((r->y + r->height-1)*scale)),
                       color, 3, 8, 0);
        if( nestedCascade.empty() )
            continue;
        smallImgROI = smallImg(*r);
    }

}


int main(int argc, char* argv[])
{
	// Prepare a Haar face classifier
	CascadeClassifier haarface,nest;
	haarface.load("haarcascade_frontalface_alt.xml");
	
    //string csvPath = "CAS-PEAL/at.txt";
	//string csvPath = "att_faces/att.csv";
	string csvPath = argv[1];
    vector<Mat> images;
    vector<int> labels;
	
	// Read in the data
	// Report if no valid filename
	try{
	    read_csv(csvPath,images,labels);
	} catch (Exception& e){
        cerr << "Error opening file \"" << csvPath << "\". Reason: " << e.msg << endl;
        exit(1);		
	}
	
	// Quit if there are not enough images for this demo.
    if(images.size() <= 1) {
        string error_message = "At least 2 images to work.";
        CV_Error(CV_StsError, error_message);
    }

    cout << "Images: " << images.size() << endl;
    cout << "Labels: " << labels.size() << endl;
	
	/*------------------------------------------------------------
	
	Resize sample images:
	
		Got an error that said src pictures are not of the same size.
		So I write another function to resize them.
		But still got error, anyway it can be useful for dealing with src imgs
		src filenames must be well arranged.
		
	/*------------------------------------------------------------
	
	//string PATH1 = "CAS-PEAL/female/";
	//string Prefix1 = "face_";
	//int START1 = 1;
	//int END1 = 400;
	//resizeSamples(PATH1,Prefix1,START1,END1);
	
	//string PATH2 = "CAS-PEAL/male/";
	//string Prefix2 = "face_";
	//int START2 = 401;
	//int END2 = 800;
	//resizeSamples(PATH2,Prefix2,START2,END2);
	
	--------------------------------------------------------------*/
	
	/*------------------------------------------------------------
	
	Training with FaceRecognizer class of OpenCV:
	
	--------------------------------------------------------------*/		
	Mat testSample = images[images.size()-1];
	int testLabel = labels[labels.size()-1];
	images.pop_back();
	labels.pop_back();	
	//height info of images
	//int height = images[0].rows;

    // LBPHFaceRecognizer
	// Need gray scale images
    Ptr<FaceRecognizer> model = createLBPHFaceRecognizer();
    model -> train(images,labels);
    model -> save("XML/LBPfaces.xml");	

	
	// FisherFaceRecognizer
    //Ptr<FaceRecognizer> model = createFisherFaceRecognizer();
    //model -> train(images,labels);
    //model -> save("XML/Fisherfaces.xml");
	//model -> load("XML/Fisherfaces.xml");

	// EigenFaceRecognizer
    //Ptr<FaceRecognizer> model = createEigenFaceRecognizer(80,10);
    //model -> train(images,labels);
    //model -> save("XML/Eigenfaces.xml");
	//model -> load("XML/Eigenfaces.xml");
	
	/*------------------------------------------------------------
	
	Detection Part:
	
	--------------------------------------------------------------*/
	
	Mat testImg;
	string testFile = argv[2];
	testImg = imread(testFile,0);
	//testImg = imread("att_faces/male/s4/1.pgm",0);
	
	int predictLabel = model -> predict(testImg);
	
	string result = format("\n\nPredicted GENDER class = %d", predictLabel);
	cout << result << endl;
	cout<<"Predicted Gender is ";
	if(predictLabel==1)
	{
		cout<<"Female"<<endl;
	}
	else
	{
		cout<<"Male"<<endl;
	}
	
	/*-------------------------------------------------------------*/
	
	/*------------------------------------------------------------
	
	Detecting via camera:
	
	--------------------------------------------------------------
	
	VideoCapture cap(0);
	if(!cap.isOpened())
	{
		cout<<"Cannot open Camera"<<endl;
		return 1;
	}
	
	Mat frame;
	cap >> frame;
	
	vector<Rect> recs;
	Mat test(WIDTH,HEIGHT,CV_8UC1);
	Mat gray;
	int x,y;
	
	while(1)
	{
		if(!cap.read(frame))
			break;
		
		detection(frame,haarface,nest,2,0);
		imshow("face",frame);
		
		
		for(int i=0;i<recs.size();i++)
		{
			rectangle(frame,recs[i],Scalar(0,0,255));
			x = recs[i].x + recs[i].width/2;
			y = recs[i].y + recs[i].height/2;
			
			Mat ROI = frame(recs[i]);
			cvtColor(ROI,gray,CV_BGR2GRAY);
			resize(gray,test,Size(WIDTH,HEIGHT));// Must be the same as training images
			
			int result = model -> predict(test);
			
			switch(result)
			{
				case 0:
					cout << "Male." << endl; break;
				case 1:
					cout << "Female." << endl; break;
			}
		}
		
				
		imshow("Result",frame);	
		if(waitKey(30)>=0)
			break;
	}
	cap.release();
	
	/*-------------------------------------------------------------*/

    return 0;
}