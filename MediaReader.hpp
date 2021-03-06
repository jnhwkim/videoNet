#ifndef MEDIA_READER_H_
#define MEDIA_READER_H_

#include "opencv2/opencv.hpp"
#include "SubtitleReader.hpp"

#define DEFAULT_SUBTITLE_EXT "smi"

enum DetectorType {
	TYPE_SIFT,
};

class MediaReader {
private:
	string _filename;
	SubtitleReader* _subtitleReader;
	cv::VideoCapture _cap;
public:
	~MediaReader();
	MediaReader(const cv::string& filename);
	cv::VideoCapture& operator>>(cv::Mat& image);
	void get_frame(int idx, cv::Mat& image);
	const Subtitle& get_subtitle(int idx) const;
	int get_subtitle_count() const;
	const string& get_file_extension();
	const string get_subtitles();
	void kmean_descriptor(DetectorType TYPE, int K, int samples_per_frame, cv::Mat& centroids);
	static void good_matches(cv::Mat& descriptors, cv::Mat& centroids, cv::vector<cv::DMatch>& good_matches);
};

#endif /* MEDIA_READER_H_ */