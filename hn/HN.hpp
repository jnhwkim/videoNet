#ifndef HN_H_
#define HN_H_

class HN {
private:
	HEdge* _edges; // Hyper weights
	unsigned _count;
	int _num_layers;
public:
	HN(int num_layers);
};

#endif /* HN_H_ */