#include "MediaReader.hpp"
#include "util/util.hpp"
#include <boost/regex.hpp>
#include <algorithm>
#include <opencv2/nonfree/features2d.hpp>

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

void MediaReader::get_frame(int time, cv::Mat& image) {
    double fps = this->_cap.get(CV_CAP_PROP_FPS);
    double frame = floor(time * fps / 1000) - 2; // align with previous setting
    this->_cap.set(CV_CAP_PROP_POS_FRAMES, frame);
    this->_cap >> (Mat&) image;
}

const Subtitle& MediaReader::get_subtitle(int idx) const {
    return this->_subtitleReader->get(idx);
}

int MediaReader::get_subtitle_count() const {
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

// sample to find k-mean descriptors
void MediaReader::kmean_descriptor(
    DetectorType TYPE, int K, int samples_per_frame, Mat& centroids) {
    
    int idx_limit = this->get_subtitle_count();
    int** selected_idx = (int **) malloc(sizeof(int*));
    *selected_idx = (int *) malloc(sizeof(int) * samples_per_frame);
    Mat samples, labels;

    printf("Sampling %d descriptors for each frame ...\n", samples_per_frame);
    for(int idx = 0; idx < idx_limit; ++idx)
    {
        printf("."); if (0 == (idx + 1) % 20) printf(" (%d/%d)\n", idx + 1, idx_limit);
        cout.flush();

        Mat frame, resized, descriptors;
        std::vector<cv::KeyPoint> keypoints;

        Subtitle subtitle = this->get_subtitle(idx);
        this->get_frame(subtitle.time, frame);
        cv::resize(frame, resized, Size(), 0.25, 0.25, INTER_CUBIC);

        switch(TYPE) {
        case TYPE_SIFT:
            cv::SiftFeatureDetector detector;
            detector.detect(resized, keypoints);

            SiftDescriptorExtractor extractor;
            extractor.compute(resized, keypoints, descriptors);
        }

        int k1 = min(samples_per_frame, descriptors.size().height);
        
        assert(descriptors.size().height >= k1);
        util::randsample(descriptors.size().height, k1, selected_idx, idx);
        for (int i = 0; i < k1; i++) {
            cout << *(*selected_idx + i) << "\t";
        }
        cout << endl;

        for (int j = 0; j < k1; j++) {
            if (0 == j && 0 == idx) {
                samples = Mat(idx_limit * k1, descriptors.size().width, descriptors.type());
            }
            // Matrix `descriptors` will be destroyed after retruning.
            descriptors.row(*(*selected_idx + j)).copyTo(samples.row(idx*k1+j));
        }
    }
    printf("$\n");
    namedWindow("samples",9);
    imshow("samples", samples);

    printf("K-means clustering ...\n");
    kmeans(samples, K, labels, TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0), 3, KMEANS_PP_CENTERS, centroids);
    free(*selected_idx);
    free(selected_idx);
}

void MediaReader::good_matches(cv::Mat& descriptors, cv::Mat& centroids, cv::vector< DMatch >& good_matches) {
    
    //-- Matching descriptor vectors using FLANN matcher
    cv::FlannBasedMatcher matcher;
    std::vector< DMatch > matches;
    matcher.match( descriptors, centroids, matches );

    double max_dist = 0; double min_dist = 10000000;
    double mean_dist = 0;

    //-- Quick calculation of max and min distances between keypoints
    for( int i = 0; i < descriptors.rows; i++ ) { 
        double dist = matches[i].distance;
        if( dist < min_dist ) min_dist = dist;
        if( dist > max_dist ) max_dist = dist;
        mean_dist = (mean_dist * i + dist) / (i + 1); // incremental mean
    }

    printf("-- Max dist : %f \n", max_dist );
    printf("-- Min dist : %f \n", min_dist );

    //-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist,
    //-- or a small arbitary value ( 0.02 ) in the event that min_dist is very
    //-- small)
    //-- PS.- radiusMatch can also be used here.
    for( int i = 0; i < descriptors.rows; i++ ) { 
        if ( matches[i].distance <= mean_dist ) { 
            good_matches.push_back( matches[i]); 
        }
    }
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
    const std::string CENTROIDS_FILENAME("centroids.yml");

    MediaReader pororo = MediaReader(mediafile);
    int count = pororo.get_subtitle_count();

    DetectorType TYPE = TYPE_SIFT;
    int K = 100;
    int samples_per_frame = 10;
    Mat centroids;

    // 1. Get K descriptors to index vertices.
    if (util::exist(CENTROIDS_FILENAME)) {
        cout << "Loading centroids ..." << endl;
        FileStorage fs(CENTROIDS_FILENAME, FileStorage::READ);
        fs["centroids"] >> centroids;
    } else {
        pororo.kmean_descriptor(TYPE, K, samples_per_frame, centroids);
        FileStorage fs(CENTROIDS_FILENAME, FileStorage::WRITE);
        fs << "centroids" << centroids;
        fs.release();
    }

    Mat edges;
    namedWindow("edges",1);
    for(int idx = 0; idx < 1000; idx++)
    {
        Mat frame, resized, descriptors;
        std::vector<cv::KeyPoint> _keypoints, keypoints;
        
        Subtitle subtitle = pororo.get_subtitle(idx);
        pororo.get_frame(subtitle.time, frame);
        cv::resize(frame, resized, Size(), 0.25, 0.25, INTER_CUBIC);

        // 2. Extract features and get descriptors.
        switch(TYPE) {
        case TYPE_SIFT:
            cv::SiftFeatureDetector detector;
            detector.detect(resized, _keypoints);

            SiftDescriptorExtractor extractor;
            extractor.compute(resized, _keypoints, descriptors);
        }

        // 3. Get good matches for K centroids.
        std::vector< DMatch > good_matches;
        MediaReader::good_matches(descriptors, centroids, good_matches);

        for (int i = 0; i < good_matches.size(); i++) {
            int q_idx = good_matches.at(i).queryIdx;
            int t_idx = good_matches.at(i).trainIdx;
            cout << q_idx << "\t" << t_idx << endl;
            keypoints.push_back(_keypoints.at(q_idx));
        }

        printf("Good %lu / %lu\n", _keypoints.size(), keypoints.size());

        // Add results to image and save.
        cv::drawKeypoints(resized, keypoints, edges);

        // MSER
        // vector<vector<Point> > contours;
        // double t = (double)getTickCount();
        // MSER()(frame, contours);
        // t = (double)getTickCount() - t;
        // printf( "MSER extracted %d contours in %g ms.\n", (int)contours.size(),
        //        t*1000./getTickFrequency() );

        // // draw mser's with different colors
        // for( int i = (int)contours.size()-1; i >= 0; i-- )
        // {
        //     const vector<Point>& r = contours[i];
        //     for ( int j = 0; j < (int)r.size(); j++ )
        //     {
        //         cout << r.size() << endl;
        //         Point pt = r[j];
        //         frame.at<Vec3b>(pt) = bcolors[i%9];
        //     }
        // }

        // print subtitles info
        cout << subtitle.time << "\t" << subtitle.text << endl;

        //cvtColor(frame, edges, CV_BGR2GRAY);
        //GaussianBlur(frame, edges, Size(7,7), 1.5, 1.5);
        //Canny(edges, edges, 0, 30, 3);
        imshow("edges", edges);
        waitKey(0);
    }

    return 0;
}