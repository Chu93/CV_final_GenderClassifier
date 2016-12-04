#include <iostream>
#include <sstream>
#include <fstream>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

void read_csv(string& fileName,vector<Mat>& images,vector<int>& labels,char separator = ';')
{
    ifstream file(fileName.c_str(),ifstream::in);    //Open the file
    String line,path,label;
    while (getline(file,line))	//Read a line
    {
        stringstream lines(line);		// Read a line
        getline(lines,path,separator);  // Path + Separator(;)
        getline(lines,label);			// Then Label
        if (!path.empty()&&!label.empty())  // If succeed
        {
            images.push_back(imread(path,1));        //read sample(color)
			//images.push_back(imread(path,0));		//read sample(gray)
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
			resize(image,newImg,Size(96,112));
			imwrite(ImgName,newImg);
			cout << "Resize face_" << s << ".bmp to " << newImg.cols << "x" << newImg.rows << endl;
		}

	}

    waitKey(0);                                          // Wait for a keystroke in the window
    return 0;
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
	//height info of images
	//int height = images[0].rows;

    // LBPHFaceRecognizer
	// Need gray scale images
    //Ptr<FaceRecognizer> model = createLBPHFaceRecognizer();
    //model -> train(images,labels);
    //model -> save("XML/LBPfaces.xml");
	
	
	// FisherFaceRecognizer
    //Ptr<FaceRecognizer> model = createFisherFaceRecognizer();
    //model -> train(images,labels);
    //model -> save("XML/Fisherfaces.xml");


	// EigenFaceRecognizer
    Ptr<FaceRecognizer> model = createEigenFaceRecognizer(80,10);
    model -> train(images,labels);
    model -> save("XML/Eigenfaces.xml");
	
	/*------------------------------------------------------------
	
	Detection Part:
	
	--------------------------------------------------------------*/
	
	Mat testSample = images[images.size()-1];
	int testLabel = labels[labels.size()-1];
	images.pop_back();
	labels.pop_back();
	
	int predictLabel = model -> predict(testSample);
	
	string result_message = format("\n\nPredicted GENDER class = %d", predictLabel);
	cout << result_message << endl;
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

    return 0;
}
