#include <cassert>
#include <cstdio>
#include <algorithm>
#include "HEdge.hpp"
using namespace std;

HEdge::HEdge() {
	this->M = 0;
	this->N = 0;
	this->nz = 0;
}

HEdge::~HEdge() {
 	free(this->_i);
 	free(this->_j);
 	free(this->_val);
 	free(this->_w);
}

HEdge::HEdge(int M, int N, int nz, int* i, int* j, bool* val) {
	this->_i = i; this->_j = j; this->_val = val;
	this->M = M; this->N = N; this->nz = nz;
	this->_w = (float *) malloc(sizeof(float) * N);
}

HEdge::HEdge(int M, int N, int nz, int* i, int* j, bool* val, float* w) {
	this->_i = i; this->_j = j; this->_val = val;
	this->M = M; this->N = N; this->nz = nz;
	this->_w = w;
}

int HEdge::_find(HEdge& edge, int idx) {
	int* target_j_ptr;
	bool* target_val_ptr;
	int target_order;
	edge.get_edge(idx, &target_j_ptr, &target_val_ptr, &target_order);

	for (int i = 0; i < this->M; i++) {
		int* source_j_ptr;
		bool* source_val_ptr;
		int source_order;
		this->get_edge(i, &source_j_ptr, &source_val_ptr, &source_order);
		if (source_order != target_order) continue;
		
		bool flag = true;
		for (int j = 0; j < source_order; j++) {
			if (*(target_j_ptr + j) != *(source_j_ptr + j) ||
				*(target_val_ptr + j) != *(source_val_ptr + j)) {
				flag = false;
				break;
			}
		}
		if (flag) return i;
	}
	return NOT_FOUND;
}

void HEdge::get_edge(int idx, int** j_ptr, bool** val_ptr, int* order) const {
	assert(idx < this->N);
	*j_ptr = this->_j + this->_i[idx];
	*val_ptr = this->_val + this->_i[idx];
	*order = this->get_edge_order(idx);
}

int HEdge::get_edge_order(int idx) const {
	assert(idx < this->N);
	return this->_i[idx + 1] - this->_i[idx];
}

float HEdge::get_weight(int idx) const {
	return this->_w[idx];
}

void HEdge::_update_weight(HE_WEIGHT_UPDATE RULE, int idx, float weight) {
	switch(RULE) {
	case HE_WEIGHT_UPDATE_ADD:
		this->_w[idx] += weight;
		break;
	case HE_WEIGHT_UPDATE_REPLACE:
		this->_w[idx] = weight;
		break;
	}
}

void HEdge::merge(HE_WEIGHT_UPDATE RULE, HEdge& edge) {
	int* tmp = (int *) malloc(sizeof(int) * edge.M);
	int tmp_size = 0;
	int tmp_nz = 0;
	for (int i = 0; i < edge.M; i++) {
		int idx = this->_find(edge, i);
		if (NOT_FOUND != idx) {
			this->_update_weight(RULE, idx, edge.get_weight(idx));
		} else { // increase size
			tmp[tmp_size++] = i;
			tmp_nz += edge.get_edge_order(i);
		}
	}
	if (0 == tmp_size) return;

	int* new_i = (int *) malloc(sizeof(int) * (this->M + 1 + tmp_size));
	int* new_j = (int *) malloc(sizeof(int) * (this->nz + tmp_nz));
	bool* new_val = (bool *) malloc(sizeof(bool) * (this->nz + tmp_nz));
	float* new_w = (float *) malloc(sizeof(float) * (this->M + tmp_size));

	memcpy(new_i, this->_i, sizeof(int) * (this->M + 1));
	memcpy(new_j, this->_j, sizeof(int) * this->nz);
	memcpy(new_val, this->_val, sizeof(bool) * this->nz);
	memcpy(new_w, this->_w, sizeof(float) * this->M);

	free(this->_i);
	free(this->_j);
	free(this->_val);
	free(this->_w);

	this->_i = new_i;
	this->_j = new_j;
	this->_val = new_val;
	this->_w = new_w;

	new_i += this->M + 1;
	new_j += this->nz;
	new_val += this->nz;
	new_w += this->M;

	for (int i = 0; i < tmp_size; i++) {
		int idx = tmp[i];
		int* j_ptr; bool* val_ptr; int order;
		edge.get_edge(idx, &j_ptr, &val_ptr, &order);
		float w = edge.get_weight(idx);
		*new_i = *(new_i - 1) + order; new_i += 1;
		*new_w = w; new_w += 1;
		memcpy(new_j, j_ptr, sizeof(int) * order); new_j += order;
		memcpy(new_val, val_ptr, sizeof(bool) * order); new_val += order;
	}
	free(tmp);

	this->M += tmp_size;
	this->N = this->get_num_vertices();
	this->nz += tmp_nz;
}

int HEdge::get_num_vertices() const {
	int _max = -1;
	for (int i = 0; i < this->nz; i++) {
		if (_max < *(this->_j + i)) _max = *(this->_j + i);
	}
	return _max;
}

#define PRINT_MAX 30
void HEdge::print() const {
	printf("\n=== HE ===\ni:");
	for (int i = 0; i < min(PRINT_MAX, this->M + 1); i++)
		printf(" %d", this->_i[i]);
	printf("\nj:");
	for (int i = 0; i < min(PRINT_MAX, this->nz); i++)
		printf(" %d", this->_j[i]);
	printf("\nv:");
	for (int i = 0; i < min(PRINT_MAX, this->nz); i++)
		printf(" %d", this->_val[i]);
	printf("\nw:");
	for (int i = 0; i < min(PRINT_MAX, this->M); i++)
		printf(" %.1f", this->_w[i]);
	printf("\n");
}

int main(int argc, char** arg) {
	// HN1
	int M = 3;
	int N = 4;
	int nz = 7;
	int* i = (int *) malloc(sizeof(int) * (M + 1));
	int* j = (int *) malloc(sizeof(int) * nz);
	bool* val = (bool *) malloc(sizeof(bool) * nz);
	float* w = (float *) malloc(sizeof(float) * M);

	memcpy(i, (int[4]){0, 2, 4, 7}, sizeof(int) * (M + 1));
	memcpy(j, (int[7]){0, 1, 1, 3, 0, 1, 2}, sizeof(int) * nz);
	memcpy(val, (bool[7]){1, 1, 1, 1, 1, 1, 1}, sizeof(bool) * nz);
	memcpy(w, (float[3]){0.1, 0.2, 0.3}, sizeof(float) * M);

	// HN2
	int M2 = 3;
	int N2 = 5;
	int nz2 = 8;
	int* i2 = (int *) malloc(sizeof(int) * (M2 + 1));
	int* j2 = (int *) malloc(sizeof(int) * nz2);
	bool* val2 = (bool *) malloc(sizeof(bool) * nz2);
	float* w2 = (float *) malloc(sizeof(float) * M2);

	memcpy(i2, (int[4]){0, 2, 4, 8}, sizeof(int) * (M2 + 1));
	memcpy(j2, (int[8]){0, 1, 1, 3, 0, 1, 2, 4}, sizeof(int) * nz2);
	memcpy(val2, (bool[8]){1, 1, 1, 1, 1, 1, 1, 1}, sizeof(bool) * nz2);
	memcpy(w2, (float[3]){0.2, 0.3, 0.4}, sizeof(float) * M2);

	HEdge e(M, N, nz, i, j, val, w);
	HEdge e2(M2, N2, nz2, i2, j2, val2, w2);
	e.print();
	e2.print();
	e.merge(HE_WEIGHT_UPDATE_ADD, e2);
	e.print();
}