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
			
		ImgName=PATH+Prefix+s+".pgm";
		image = imread(ImgName,CV_LOAD_IMAGE_COLOR);

		Mat newImg;
		if(! image.data )                              // Check for invalid input
		{
			cout <<  "Could not open or find face_" << s << ".pgm" << std::endl ;
		}
		else
		{
			//namedWindow( "Display window", WINDOW_AUTOSIZE );		// Create a window for display.
			//imshow( "Display window", image );	// Show our image inside it.
			resize(image,newImg,Size(WIDTH,HEIGHT));
			imwrite(ImgName,newImg);
			cout << "Resize face_" << s << ".bmp to " << newImg.cols << "x" << newImg.rows << endl;
		}

	}

    waitKey(0);                                          // Wait for a keystroke in the window
    return 0;
}

// To enhance accuracy, I add faces2 for re-checking the detected faces
// Successfully prevent the face detector from misrecognizing hands and clocks as faces
vector<Rect> detectFaces(Mat img_gray, bool tryflip){
	CascadeClassifier faces_cascade;
	faces_cascade.load("haarcascade_frontalface_alt.xml");
	
	vector<Rect> faces,faces2;
	faces_cascade.detectMultiScale(img_gray,faces,1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30));
	
	// Detect again to enhance accuracy
	if (tryflip)
	{
		flip(img_gray,img_gray,1);	// Flip it then detect again
		faces_cascade.detectMultiScale(img_gray,faces,1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30));
		for(vector<Rect>::const_iterator r = faces2.begin(); r != faces2.end(); r++)
		{
            faces.push_back(Rect(img_gray.cols - r->x - r->width, r->y, r->width, r->height));
		}
	}
	
	return faces;
}


void drawFaces(Mat img,vector<Rect> faces){
	namedWindow("faces");
	for(size_t i=0;i<faces.size();i++){
		
		// To draw a ellipse, which looks better as a face detector
		//Point center(faces[i].x + faces[i].width/2,faces[i].y + faces[i].height/2);
		//ellipse(img,center,Size(faces[i].width/2,faces[i].height/1.5),0,0,360,Scalar(0,255,0),2,8,0);
		
		// To draw a rectangle, which is more suitable for our purpose
		Point pt1(faces[i].x, faces[i].y);
		Point pt2(faces[i].x + faces[i].width, faces[i].y + faces[i].height);
		rectangle(img,pt1,pt2,Scalar(0,0,255),1,8,0);
		
		//putText(img,"TEST FONT",pt1,CV_FONT_HERSHEY_TRIPLEX, 0.7f ,Scalar(0,255,0),1.5,8,0);
	}
	imshow("faces",img);
}

	
void saveFaces(Mat img,Mat img_gray){
	vector<Rect> faces = detectFaces(img_gray,1);
	for(size_t i=0; i<faces.size();i++){
		stringstream buffer;
		buffer<<i;
		string saveName = "face_"+ buffer.str() + ".jpg";
		Rect roi = faces[i];
		imwrite(saveName,img(roi));
	}
}
	

int main(int argc, char* argv[])
{	
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

    cout << "Got " << images.size() << " sample images." << endl;
	
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
	
	--------------------------------------------------------------	*/	

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
	
	Detection Part: a face image
	
	-------------------------------------------------------------*/
	
	Mat testImg;
	string testFile = argv[2];
	testImg = imread(testFile,0);
	//testImg = imread("att_faces/male/s4/1.pgm",0);
	
	if((testImg.rows != HEIGHT)||(testImg.cols != WIDTH))
	{
		Mat newtestImg;
		resize(testImg,newtestImg,Size(WIDTH,HEIGHT));
		testImg = newtestImg;
	}
	
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
	
	/*------------------------------------------------------------
	
	Detection Part - 2: an image of more than one people
	
	--------------------------------------------------------------
	
	Mat testImg, testImg_gray;
	if(!argv[2])
	{
		cout << "Need a test image!" << endl;
		exit(1);
	}
	
	string testFile = argv[2];
	testImg = imread(testFile);
	testImg_gray = imread(testFile,0);
	
	// Need grayscale images to test
	//cvtColor(testImg,testImg_gray,COLOR_BGR2GRAY);		// Turn into grayscale image
	//equalizeHist(testImg_gray,testImg_gray);
		
	vector<Rect> faces = detectFaces(testImg_gray,1);
	saveFaces(testImg,testImg_gray);
	
	//namedWindow("faces");
	Mat testArea,testArea_resized;

	for(size_t i=0; i < faces.size();i++)
	{
		//
		if((testImg.cols != WIDTH) || (testImg.rows != HEIGHT))
		{
			Rect roi = faces[i];
			testArea = testImg_gray(roi);
			resize(testArea,testArea_resized,Size(WIDTH,HEIGHT));
		}
		else
		{
			Mat testArea_resized = testImg_gray;
		}
		
		// Predict!
		int predictLabel = model -> predict(testArea_resized);
	
		// Draw the ROI and label
		Point pt1(faces[i].x, faces[i].y);
		Point pt2(faces[i].x + faces[i].width, faces[i].y + faces[i].height);
		rectangle(testImg,pt1,pt2,Scalar(0,0,255),1,8,0);
		
		// Show result
		string result = format("\n\nPredicted Label = %d", predictLabel);
		cout << result << endl;
		cout<<"Predicted Gender is ";
		if(predictLabel==1)
		{
			cout<<"Female"<<endl;
			putText(testImg,"Female",pt1,CV_FONT_HERSHEY_TRIPLEX, 0.7f ,Scalar(0,255,0),1.5,8,0);	
		}
		else
		{
			cout<<"Male"<<endl;
			putText(testImg,"Male",pt1,CV_FONT_HERSHEY_TRIPLEX, 0.7f ,Scalar(0,255,0),1.5,8,0);	
		}
		
	}
	imshow("faces",testImg);
	waitKey(0);

	-------------------------------------------------------------*/
	
    return 0;
}
