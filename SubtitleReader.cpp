#include "SubtitleReader.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <boost/regex.hpp>
using namespace std;

SubtitleReader::SubtitleReader() {
}

SubtitleReader::SubtitleReader(const string& filename) : _filename(filename) {
	this->read(filename);
}

void SubtitleReader::read(const string& filename) {
	const string& ext = this->get_file_extension();
	if (0 == ext.compare("smi")) {
		this->_readSMI();
	} else {
		cerr << "Unsupported subtitle extension: " << ext << endl;
	}
}

void SubtitleReader::_readSMI() {
	boost::smatch m;
	boost::regex e ("<SYNC Start=(\\d+)><P Class=UNKNOWNCC>\\s*$");
	string line;
	ifstream myfile (this->_filename.c_str());
	bool flag = false;

	if (myfile.is_open()) {
		unsigned count = 0;
		while (getline(myfile, line)) {
			if (boost::regex_search(line, m, e)) {
				count++;
			}
		}
		this->_count = count;
		this->_subtitles = (Subtitle*) malloc(sizeof(Subtitle) * count);

		myfile.clear(); // clear the error flags
		myfile.seekg(0); // reset to beginning
		unsigned idx = 0;
		while (getline(myfile, line)) {
			if (boost::regex_search(line, m, e)) {
				flag = true;
				string s_time (m[1].first, m[1].second);
				this->_subtitles[idx].time = atoi(s_time.c_str());
				continue;
			} 
			if (flag) {
				string _processed;
				this->_preprocess(line, _processed);
				char* processed = (char*) malloc(sizeof(char) * _processed.length() + 1);
				strcpy(processed, _processed.c_str());
				this->_subtitles[idx++].text = processed;
				flag = false;
				if (idx >= this->count()) break;
			}
		}
		myfile.close();
	} else {
		cerr << "Can't open a subtitle! " << endl << this->_filename << endl;
	}
}

void SubtitleReader::_preprocess(const string& text, string& processed) {
	boost::regex e ("<br>");
	processed = boost::regex_replace(text, e, " ");
}

unsigned SubtitleReader::count() {
	return this->_count;
}

const Subtitle& SubtitleReader::get(unsigned idx) {
	return this->_subtitles[idx];
}

void SubtitleReader::print(const Subtitle& subtitle) {
	cout << subtitle.time << "\t" << (subtitle.text) << endl;
}

void SubtitleReader::print() {
	for (int i = 0; i < this->count(); i++) {
		const Subtitle& subtitle = this->_subtitles[i];
		this->print(subtitle);
	}
	cout << "Total #: " << this->count() << endl;
}

const string& SubtitleReader::get_file_extension() {
	string _ext;
	std::size_t ext_pos = this->_filename.rfind(".");
  _ext.append(this->_filename.begin() + ext_pos + 1, this->_filename.end());
  const string& ext = *new string(_ext);
	return ext;
}

// int main(int, char**)
// {  
// 	const string filename = "/Users/Calvin/Documents/Projects/Pororo/Movies/subtitle/pororo_1.smi";
// 	SubtitleReader sub(filename);
// 	//sub.print();
// 	const Subtitle& subtitle = sub.get(3);
// 	sub.print(subtitle);
// 	return 0;
// };