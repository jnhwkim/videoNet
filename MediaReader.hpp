#ifndef MEDIA_READER_H_
#define MEDIA_READER_H_

#include "opencv2/opencv.hpp"
#include "SubtitleReader.hpp"

#define DEFAULT_SUBTITLE_EXT "smi"

class MediaReader {
private:
	string _filename;
	SubtitleReader* _subtitleReader;
	cv::VideoCapture _cap;
public:
	~MediaReader();
	MediaReader(const cv::string& filename);
	cv::VideoCapture& operator>>(cv::Mat& image);
	void getFrame(int idx, cv::Mat& image);
	const Subtitle& getSubtitle(int idx) const;
	int getSubtitleCount() const;
	const string& get_file_extension();
	const string get_subtitles();
};

#endif /* MEDIA_READER_H_ */