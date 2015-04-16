#ifndef HEdge_H_
#define HEdge_H_

/**
 * f(H W x) = a
 * H : indicator matrix representing the connection between vertices.
 * W : diagonal matrix whose diagonals are weights of hyperedges x
 * x : representation of the lower hyperedges
 * a : activations of the upper hyperedges
 * f : activation function
 **/
class HEdge {
private:
	// CRS formatted sparse matrix H
	unsigned* _i;		// size N + 1
	unsigned* _j;		// size nz 
	bool* 	  _val; // size nz
	// hyperedge weight vector
	float* 	  _w;   // size N
	unsigned  _nz;
public:
	unsigned M, N;
	HEdge(unsigned* i, unsigned* j, bool* val);
	void merge(HEdge& hedge);
};

#endif /* HEdge_H_ */