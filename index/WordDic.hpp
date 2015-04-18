#ifndef WORD_DIC_H_
#define WORD_DIC_H_

#include <boost/unordered_map.hpp>

#define WORD_SEPARATOR	" -;|!."

struct WordInfo {
	int id, tf, df;
};

typedef boost::unordered_map<std::string, WordInfo> word_map;

class WordDic {
private:
	word_map _dic;
public:
	WordDic();
	~WordDic();
	word_map::const_iterator begin();
	word_map::const_iterator end();
	WordInfo* get(std::string const& word);
	WordInfo* get(std::string const& word, int tf, int df);
	void parse(std::string const& document);
	void merge(WordDic& dic);
	int size() const;
	void clear();
	void print() const;
};

#endif /* WORD_DIC_H_ */