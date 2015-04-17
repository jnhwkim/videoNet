#include "WordDic.hpp"

WordDic::WordDic() {
}

WordDic::~WordDic() {
 	free(this->_dic);
}

int WordDic::get(std::string const& word) {
	
}