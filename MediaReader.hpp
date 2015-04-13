#ifndef MEDIA_READER_H_
#define MEDIA_READER_H_

#include "opencv2/opencv.hpp"

class MediaReader {
private:
	unsigned int curFrame;
	//SubtitleReader *subtitle;
	cv::VideoCapture _cap;
	void _init(const cv::string& filename);
public:
	MediaReader(const cv::string& filename);
	cv::VideoCapture& operator>>(cv::Mat& image);
};

#endif /* MEDIA_READER_H_ */