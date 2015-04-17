#ifndef HN_H_
#define HN_H_

#include "HEdge.hpp"

class HN {
private:
	HEdge* _edges; // Hyper weights
	int _num_layers;
public:
	HN(int num_layers);
	~HN();
};

#endif /* HN_H_ */