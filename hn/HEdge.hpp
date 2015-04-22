#ifndef HEdge_H_
#define HEdge_H_

#include "../index/WordDic.hpp"
#include "../index/FeatDic.hpp"

/**
 * f(H W x) = a
 * H : indicator matrix representing the connection between vertices.
 * W : diagonal matrix whose diagonals are weights of hyperedges x
 * x : representation of the lower hyperedges
 * a : activations of the upper hyperedges
 * f : activation function
 **/

#define NOT_FOUND 		-1
#define DECAY_FACTOR 	0.9
#define EDGE_BUFFER		100
#define MAX_ORDER			8

enum HE_WEIGHT_UPDATE {
 	HE_WEIGHT_UPDATE_ADD,
 	HE_WEIGHT_UPDATE_REPLACE,
 	HE_WEIGHT_UPDATE_DECAY,
};

enum HE_SAMPLING {
	HE_SAMPLING_RANDOM,
	HE_SAMPLING_GREEDY,
	HE_SAMPLING_SERENDIPITY,
};

class HEdge {
private:
	// CRS formatted sparse matrix H
	int* _i;		// size M + 1
	int* _j;		// size nz 
	bool* _val; // size nz
	float* _w;	// size M, hyperedge weight vector
	// edge buffer
	int* _i_b;		// size `EDGE_BUFFER` + 1
	int* _j_b;		// size `EDGE_BUFFER` x `MAX_ORDER`
	bool* _val_b; // size `EDGE_BUFFER` x `MAX_ORDER`
	float* _w_b;  // size `EDGE_BUFFER`
	int _count_b; // buffer count
	int _find(HEdge& edge, int idx);
	void _update_weight(HE_WEIGHT_UPDATE RULE, int idx, float weight);
public:
	int M; // # of hyperedges
	int nz; // # of connections
	HEdge();
	~HEdge();
	HEdge(int M, int nz, int* i, int* j, bool* val);
	HEdge(int M, int nz, int* i, int* j, bool* val, float* w);
	void init();
	void init(int M, int nz, int* i, int* j, bool* val);
	void get_edge(int idx, int** j_ptr, bool** val_ptr, int* degree) const;
	int get_edge_order(int idx) const;
	float get_weight(int idx) const;
	void merge(HE_WEIGHT_UPDATE RULE, HEdge& edge);
	int N() const; // N, # of vertices
	void init_buffer();
	void clear_buffer();
	void flush(HE_WEIGHT_UPDATE RULE);
	void print() const;

	// various sampler
	#define VERTEX_WORD 0
	#define VERTEX_FEAT	100000000
	void sample_edge(HE_SAMPLING METHOD, int degree, WordDic& dic, WordDic& alldic);
	void sample_edge(HE_SAMPLING METHOD, int degree, FeatDic& dic, FeatDic& alldic);
};

#endif /* HEdge_H_ */