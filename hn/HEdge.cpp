#include <cassert>
#include "HEdge.hpp"

HEdge::HEdge() {
	this->M = 0;
	this->N = 0;
	this->_nz = 0;
}

HEdge::~HEdge() {
 	free(this->_i);
 	free(this->_j);
 	free(this->_val);
 	free(this->_w);
}

HEdge::HEdge(int M, int N, int nz, 
	int* i, int* j, bool* val) {
	this->_i = i; this->_j = j; this->_val = val;
	this->M = M; this->N = N; this->nz = nz;
	this->_w = (float *) malloc(sizeof(float) * N);
}

int HEdge::_find(HEdge& edge, int idx) {
	bool* target_j_ptr, target_val_ptr;
	edge.get_edge(idx, target_j_ptr, target_val_ptr);
	int target_order = edge.get_edge_order(idx);

	for (int i = 0; i < this->M; i++) {
		bool* source_j_ptr, source_val_ptr;
		this->get_edge(i, source_j_ptr, source_val_ptr);
		int source_order = this->get_edge_order(i);
		if (source_order != target_order) continue;
		
		bool flag = true;
		for (int j = 0; j < source_order; j++) {
			if (target_j_ptr[j] != source_j_ptr[j] && 
				target_val_ptr[j] != source_val_ptr[j]) {
				flag = false;
				break;
			}
		}
		if (flag) return i;
	}
	return NOT_FOUND;
}

void HEdge::get_edge(int idx, int* j_ptr, bool* val_ptr) {
	assert(idx < this->N);
	j_ptr = this->_j + this->_i[idx];
	val_ptr = this->_val + this->_i[idx];
}

int HEdge::get_edge_order(int idx) {
	assert(idx < this->N);
	return this->_i[idx + 1] - this->_i[idx];
}

float HEdge::get_weight(int idx) {
	return this->_w[idx];
}

void HEdge::update_weight(HE_WEIGHT_UPDATE RULE, int idx, float weight) {
	switch(RULE) {
	case HE_WEIGHT_UPDATE_ADD:
		this->_w[idx] += weight;
		break;
	case HE_WEIGHT_UPDATE_REPLACE:
		this->_w[idx] = weight;
		break;
	}
}

void HEdge::merge(HEdge& edge) {
	vector<int
	for (int i = 0; i < edge.M; i++) {
		int idx = this->_find(edge, i);
		if (NOT_FOUND != idx) {
			this->update_weight(idx, edge.get_weight(idx));
		} else { // increase size
			
		}
	}
}