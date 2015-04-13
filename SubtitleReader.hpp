#ifndef SUBTITLE_READER_H_
#define SUBTITLE_READER_H_

#include <string>
using namespace std;

struct Subtitle {
	int time;
	string text;
};

class SubtitleReader {
private:
	string _filename;
	Subtitle* _subtitles;
	unsigned _count;
	void _readSMI();
	void _preprocess(const string& text, string& processed);
public:
	SubtitleReader(const string& filename);
	string& get_file_extension();
	void read(const string& filename);
	void print();
};

#endif /* SUBTITLE_READER_H_ */