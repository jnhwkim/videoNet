#ifndef MY_UTIL_H_
#define MY_UTIL_H_

#define MAX_RETRY 10

class util {
public:
	static void randsample(int n, int k, int** list, int seed = -1);
};

#endif /* MY_UTIL_H_ */