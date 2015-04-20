#include <cassert>
#include <ctime>
#include <iostream>
#include <boost/random.hpp>
#include "util.hpp"
using namespace std;

void util::randsample(int n, int k, int** list, int seed) {
	assert(n >= k);
	bool chk[n]; for (int i = 0; i < n; i++) chk[i] = 0;
	std::time_t now = std::time(0);
	if (0 > seed) seed = (int) now;
  for (size_t i = 0; i < k; i++) {
  	boost::mt19937 gen(static_cast<uint32_t>(seed + i));
  	boost::uniform_int<> range(0, n - 1 - i);
  	boost::variate_generator<boost::mt19937, boost::uniform_int<> > 
  	dice( gen, range );
  	int x = dice();
  	int retry = 0;
  	while (chk[x]) {
  		if (retry < MAX_RETRY) {
  			x = dice();
  			retry++;
  		} else {
  			x = (x + 1) % n;
  		}
  	}
  	chk[x] = 1;
  	*(*list + i) = x;
  }
}

// int main(int argc, char** arg) {
// 	int N = 10;
// 	int* list = (int *) malloc(sizeof(int) * N);
// 	util::randsample(11, N, &list);
// 	for (int* p = list; p < list + N; p++) {
// 		printf("%d\n", *p);
// 	}
// }
