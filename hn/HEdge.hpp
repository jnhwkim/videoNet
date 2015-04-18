#ifndef HEdge_H_
#define HEdge_H_

#include "../index/WordDic.hpp"

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

typedef boost::unordered_map<std::string, int> map;

enum HE_WEIGHT_UPDATE {
 	HE_WEIGHT_UPDATE_ADD,
 	HE_WEIGHT_UPDATE_REPLACE,
 	HE_WEIGHT_UPDATE_DECAY,
};

enum HE_SAMPLING {
	HE_SAMPLING_RANDOM,
	HE_SAMPLING_GREEDY,
	HE_SAMPLING_SERENDIPITY,
}

class HEdge {
private:
	// CRS formatted sparse matrix H
	int* _i;		// size M + 1
	int* _j;		// size nz 
	bool* _val; // size nz
	float* _w;	// size M, hyperedge weight vector
	map _map;		// modal mapper i.e. 'w' + word id -> vertex id
	int _find(HEdge& edge, int idx);
	void _update_weight(HE_WEIGHT_UPDATE RULE, int idx, float weight);
public:
	int M; // # of hyperedges
	int N; // # of vertices
	int nz; // # of connections
	HEdge();
	~HEdge();
	HEdge(int M, int N, int nz, int* i, int* j, bool* val);
	HEdge(int M, int N, int nz, int* i, int* j, bool* val, float* w);
	void get_edge(int idx, int** j_ptr, bool** val_ptr, int* order) const;
	int get_edge_order(int idx) const;
	float get_weight(int idx) const;
	void merge(HE_WEIGHT_UPDATE RULE, HEdge& edge);
	int get_num_vertices() const;
	void print() const;

	// various sampler
	void sample_edge(HE_SAMPLING METHOD, int order, 
		WordDic& dic, WordDic& alldic);
	void sample_edge(HE_SAMPLING METHOD, int order, 
		FeatDic& dic, FeatDic& alldic);
};

#endif /* HEdge_H_ */