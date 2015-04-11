#include "opencv2/opencv.hpp"

using namespace cv;

int main(int, char**)
{  
    const char *filename = "/Users/Calvin/Documents/Projects/Pororo/Movies/pororo_3_1.avi";
    VideoCapture cap(filename); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    double count = cap.get(CV_CAP_PROP_FRAME_COUNT); //get the frame count
    cap.set(CV_CAP_PROP_POS_FRAMES,count-10000); //Set index to last frame

    Mat edges;
    namedWindow("edges",1);
    for(;;)
    {
        Mat frame;

        cap >> frame; // get a new frame from camera
    
        cvtColor(frame, edges, CV_BGR2GRAY);
        GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        Canny(edges, edges, 0, 30, 3);
        imshow("edges", edges);
        if(waitKey(30) >= 0) break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}