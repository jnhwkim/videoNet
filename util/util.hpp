#ifndef MY_UTIL_H_
#define MY_UTIL_H_

#define MAX_RETRY 10

enum E_TYPE {
	E_FILE,
};

class util {
public:
	static void randsample(int n, int k, int** list, int seed = -1);
	static bool exist(const std::string& name, E_TYPE TYPE = E_FILE);
};

#endif /* MY_UTIL_H_ */