#include "MediaReader.hpp"

using namespace cv;

MediaReader::MediaReader(const cv::string& filename) {
    this->_init(filename);
}
void MediaReader::_init(const cv::string& filename) {
    this->_cap = VideoCapture(filename);
    if(!this->_cap.isOpened())  // check if we succeeded
        exit(1);
}
VideoCapture& MediaReader::operator>>(cv::Mat& image) {
    this->_cap >> image;
    return this->_cap;
}

int main(int, char**)
{  
    const char *filename = "/Users/Calvin/Documents/Projects/Pororo/Movies/pororo_3_1.avi";

    MediaReader pororo = MediaReader(filename);

    //double count = cap.get(CV_CAP_PROP_FRAME_COUNT); //get the frame count
    //cap.set(CV_CAP_PROP_POS_FRAMES,count-10000); //Set index to last frame

    Mat edges;
    namedWindow("edges",1);
    for(;;)
    {
        Mat frame;

        pororo >> frame; // get a new frame from camera
    
        cvtColor(frame, edges, CV_BGR2GRAY);
        GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
        Canny(edges, edges, 0, 30, 3);
        imshow("edges", edges);
        if(waitKey(30) >= 0) break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}