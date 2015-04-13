#include "SubtitleReader.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
using namespace std;

SubtitleReader::SubtitleReader(const string& filename) {
	this->read(filename);
}
void SubtitleReader::read(const string& filename) {
	this->_filename = filename;
	string& ext = this->get_file_extension();
	if (0 == ext.compare("smi")) {
		this->_readSMI();
	} else {
		cerr << "Unsupported subtitle extension: " << ext << endl;
	}
}

void SubtitleReader::_readSMI() {
	smatch m;
	regex e ("<SYNC Start=(\\d+)><P Class=UNKNOWNCC>\\s*$");
	string line;
	ifstream myfile (this->_filename);
	bool flag = false;

	if (myfile.is_open()) {
		unsigned count = 0;
		while (getline(myfile, line)) {
			regex_search(line, m, e);
			if (0 != m.length()) {
				count++;
			}
		}
		this->_count = count;
		myfile.clear(); // clear the error flags
		myfile.seekg(0); // reset to beginning
		this->_subtitles = (Subtitle*) malloc(sizeof(Subtitle) * count);

		unsigned idx = 0;
		while (getline(myfile, line)) {
			regex_search(line, m, e);
			if (0 != m.length()) {
				flag = true;
				this->_subtitles[idx].time = stoi(m[1]);
				continue;
			} 
			if (flag) {
				this->_preprocess(line, this->_subtitles[idx++].text);
				flag = false;
				if (idx >= this->_count) break;
			}
		}
		myfile.close();
	}
}

void SubtitleReader::_preprocess(const string& text, string& processed) {
	regex e ("<br>");
	processed = regex_replace(text, e, " ");
}

void SubtitleReader::print() {
	for (int i = 0; i < this->_count; i++) {
		Subtitle subtitle = this->_subtitles[i];
		cout << subtitle.time << "\t" << subtitle.text << endl;
	}
}

string& SubtitleReader::get_file_extension() {
	string& ext = *new string();
	std::size_t ext_pos = this->_filename.rfind(".");
  ext.append(this->_filename.begin() + ext_pos + 1, this->_filename.end());
	return ext;
}

int main(int, char**)
{  
	string filename ("/Users/Calvin/Documents/Projects/Pororo/Movies/subtitle/pororo_1.smi");
	SubtitleReader sub(filename);
	sub.print();
	return 0;
};