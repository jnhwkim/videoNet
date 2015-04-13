#ifndef SUBTITLE_READER_H_
#define SUBTITLE_READER_H_

#include <string>
using namespace std;

struct Subtitle {
	int time;
	char* text;
};

class SubtitleReader {
private:
	const string _filename;
	Subtitle* _subtitles;
	unsigned _count;
	void _readSMI();
	void _preprocess(const string& text, string& processed);
public:
	SubtitleReader();
	SubtitleReader(const string& filename);
	const string& get_file_extension();
	void read(const string& filename);
	unsigned count();
	const Subtitle& get(unsigned idx);
	void print();
	void print(const Subtitle& subtitle);
};

#endif /* SUBTITLE_READER_H_ */