#include <iostream>
#include <string>
#include <algorithm>
#include "FeatDic.hpp"

typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

FeatDic::FeatDic() {
}

FeatDic::~FeatDic() {
	this->clear();
}

word_map::const_iterator FeatDic::begin() {
	return this->_dic.begin();
}
word_map::const_iterator FeatDic::end() {
	return this->_dic.end();
}

WordInfo* FeatDic::get(std::string const& word) {
	return this->get(word, 0, 0);
}

// insert an word to a  and returns an index in the map.
WordInfo* FeatDic::get(std::string const& word, int tf, int df) {
	word_map::iterator iter = this->_dic.find(word);
	if (iter != this->_dic.end()) { // found
		WordInfo* info = &iter->second;
		info->tf += tf;
		info->df += df;
		return info;
	}
	else { // not found
		int id = this->_dic.size();
		std::pair<word_map::iterator, bool> ret;
		WordInfo info = {id, std::max(tf, 1), std::max(df, 1)}; // for new ones
		ret = this->_dic.insert(word_map::value_type(word, info));
		assert(ret.second); // check if inserting has no error
		assert(ret.first->second.id == id); // idx check
		return &ret.first->second;
	}
}

/**
 * parse a given string and build FeatDic.
 * `document` is a simply tokenizable string.
 * Notice: tolower, separator `WORD_SEPARATOR` preprocessing 
 * TODO: increase df per call
 */
void FeatDic::parse(std::string const& document) {
	// tolower
	std::string doc(document);
	std::transform(doc.begin(), doc.end(), doc.begin(), ::tolower);
  boost::char_separator<char> sep(WORD_SEPARATOR);
  tokenizer tok(doc, sep);
  for(tokenizer::iterator beg=tok.begin(); beg!=tok.end();++beg) {
  	this->get(*beg, 1, 0);
  }
}

void FeatDic::merge(FeatDic& dic) {
	size_t j = 0;
	for(word_map::const_iterator i = dic.begin(); i != dic.end(); ++i) {
		const char* word = i->first.c_str();
		const WordInfo* info = &i->second;
		this->get(word, info->tf, info->df);
	}
}

int FeatDic::size() const {
	return this->_dic.size();
}

void FeatDic::clear() {
	this->_dic.clear();
}

void FeatDic::print() const {
	printf("\n=== FeatDic ===\n");
	for(word_map::const_iterator i = this->_dic.begin(); i != this->_dic.end(); ++i) {
		const char* word = i->first.c_str();
		const WordInfo* info = &i->second;
		printf("%s\t=> id: %d, tf: %d, df: %d\n", word, info->id, info->tf, info->df);
	}
}

// int main(int argc, char** arg) {
// 	FeatDic dic;
// 	printf("%d\n", dic.get("Wow", 1, 0)->id); 		// 0
// 	printf("%d\n", dic.get("It's", 1, 0)->id); 		// 1
// 	printf("%d\n", dic.get("Pororo", 1, 0)->id); 	// 2
// 	printf("%d\n", dic.get("Pororo", 1, 0)->id); 	// 2
// 	printf("%d\n", dic.get("Pororo", 1, 0)->id); 	// 2
// 	printf("%d\n", dic.get("Its", 1, 0)->id); 		// 3
// 	dic.print();

// 	WordInfo* info;
// 	FeatDic dic2;
// 	dic2.parse("Wow It's Pororo! Pororororo!");
// 	dic2.print();

// 	dic.merge(dic2);
// 	dic2.clear();
// 	dic.print();
// }