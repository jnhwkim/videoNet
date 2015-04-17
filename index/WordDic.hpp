#ifndef WORD_DIC_H_
#define WORD_DIC_H_

#include <boost/unordered_map.hpp>

class WordDic {
private:
	boost::unordered_map<std::string, int> _dic;
public:
	WordDic();
	~WordDic();
	int get(std::string const& word);
};

#endif /* WORD_DIC_H_ */