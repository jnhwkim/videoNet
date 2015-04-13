#include "MediaReader.hpp"
#include <boost/regex.hpp>

using namespace cv;

MediaReader::MediaReader(const cv::string& filename) {
    // open a media file
    this->_filename = filename;
    this->_cap = VideoCapture(filename);
    if(!this->_cap.isOpened())  // check if we succeeded
        exit(1);
    // open a subtitle file corresponding a given media file
    const string& subtitlesfile = this->get_subtitles();
    this->_subtitleReader = new SubtitleReader(subtitlesfile);
}

MediaReader::~MediaReader() {
    this->_cap.release();
}

VideoCapture& MediaReader::operator>>(cv::Mat& image) {
    this->_cap >> image;
    return this->_cap;
}

void MediaReader::getFrame(unsigned time, cv::Mat& image) {
    double fps = this->_cap.get(CV_CAP_PROP_FPS);
    double frame = round(time * fps / 1000);
    this->_cap.set(CV_CAP_PROP_POS_FRAMES, frame);
    this->_cap >> (Mat&) image;
}

const Subtitle& MediaReader::getSubtitle(unsigned idx) const {
    return this->_subtitleReader->get(idx);
}

unsigned MediaReader::getSubtitleCount() const {
    return this->_subtitleReader->count();
}

const string& MediaReader::get_file_extension() {
    string& _ext = *new string();
    std::size_t ext_pos = this->_filename.rfind(".");
    _ext.append(this->_filename.begin() + ext_pos + 1, this->_filename.end());
    const string& ext = *new string(_ext);
    return ext;
}

const string MediaReader::get_subtitles() {
    boost::smatch m;
    boost::regex e ("^(\\S+)\\/(\\S+)\\.(\\S+)$");
    string subtitlesfile ("");
    if (boost::regex_search(this->_filename, m, e))
        subtitlesfile = string(m[1].first, m[1].second) + "/subtitles/" + string(m[2].first, m[2].second) + "." + DEFAULT_SUBTITLE_EXT;
    return subtitlesfile;
}

static const Vec3b bcolors[] =
{
    Vec3b(0,0,255),
    Vec3b(0,128,255),
    Vec3b(0,255,255),
    Vec3b(0,255,0),
    Vec3b(255,128,0),
    Vec3b(255,255,0),
    Vec3b(255,0,0),
    Vec3b(255,0,255),
    Vec3b(255,255,255)
};

int main(int, char**)
{  
    const string mediafile ("/Users/Calvin/Documents/Projects/Pororo/Movies/pororo_3_1.avi");

    MediaReader pororo = MediaReader(mediafile);
    unsigned count = pororo.getSubtitleCount();

    Mat edges;
    namedWindow("edges",1);
    for(int idx = 0; idx < 1000; idx++)
    {
        Mat frame;
        
        Subtitle subtitle = pororo.getSubtitle(idx);
        pororo.getFrame(subtitle.time, frame);

        // MSER
        vector<vector<Point> > contours;
        double t = (double)getTickCount();
        MSER()(frame, contours);
        t = (double)getTickCount() - t;
        printf( "MSER extracted %d contours in %g ms.\n", (int)contours.size(),
               t*1000./getTickFrequency() );

        // draw mser's with different colors
        for( int i = (int)contours.size()-1; i >= 0; i-- )
        {
            const vector<Point>& r = contours[i];
            for ( int j = 0; j < (int)r.size(); j++ )
            {
                cout << r.size() << endl;
                Point pt = r[j];
                frame.at<Vec3b>(pt) = bcolors[i%9];
            }
        }

        // print subtitles info
        cout << subtitle.time << "\t" << subtitle.text << endl;

        //cvtColor(frame, edges, CV_BGR2GRAY);
        GaussianBlur(frame, edges, Size(7,7), 1.5, 1.5);
        //Canny(edges, edges, 0, 30, 3);
        imshow("edges", edges);
        waitKey(0);
    }

    return 0;
}