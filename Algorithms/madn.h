#ifndef _MNRCSketch_H
#define _MNRCSketch_H

#include "../utils/BOBHash.h"
#include <unordered_map>
// #include <Eigen/Dense>
#include <cmath>
#include <vector>
#include <cstdint>
#include <string.h>
#include <iostream>
using namespace std;
// using namespace Eigen;
#define N  999 //精度为小数点后面3位

class MNRCSketch
{
private:
	int*** C;
    int*** indicators;
	unsigned int* hash_seeds;
    BOBHash * bobhash;
	int rand_d;
	int m, d, T, cell_num, cell_bit;
	float p;
	unordered_map<unsigned int, int> flow_labels;
	bool offline_query = false;
	unordered_map<unsigned int, int> estimate_table;

public:
	MNRCSketch(int _m, int _d, int _T, float _p, int _cell_num, int _cell_bit)
	{
		m = _m;
		d = _d;
		T = _T;
		p = _p;
        cell_num = _cell_num;
        cell_bit = _cell_bit;
		rand_d = -1;
		srand(time(NULL));
		C = new int** [d];
        indicators = new int** [d];
		for (int i = 0; i < d; i++) {
			C[i] = new int*[m];
            indicators[i] = new int*[m];
            for (int j = 0;j < m;j++) {
                C[i][j] = new int[cell_num];
                memset(C[i][j], sizeof(int)*cell_num, 0);
                indicators[i][j] = new int[cell_num];
                memset(indicators[i][j], sizeof(int)*cell_num, 0);
            }
		}

        flow_labels.max_load_factor(0.85);

		hash_seeds = new unsigned int[d];
		for (int i = 0; i < d; i++)
		{
			hash_seeds[i] = i * 1024;
		}
        bobhash = new BOBHash[d];
        for (int i = 0; i < d; i++)
		{
			bobhash[i] = BOBHash(i + 1000);
		}
	}

	void Insert(char* str)
	{
		rand_d = (rand_d + 1) % d;
		unsigned int j, s_i, cell_i;
        int left, right;
        j = bobhash[rand_d].run(str, strlen(str));
		s_i = j & 0b1;
        cell_i = ((j>>1)&0b111111) % cell_num;
        j = (j >> 7) % m;
        left = cell_i - 1;
        while (left>=0 and indicators[rand_d][j][left]) {
            left-=1;
        }
        left+=1;
        right=cell_i;
        while (right<cell_num and indicators[rand_d][j][right]) right+=1;
		if (s_i == 0)
		{
			C[rand_d][j][right]--;
		}
		else
		{
			C[rand_d][j][right]++;
		}
		
        int l1, new_right, new_left;
        l1 = (right-left+1)*cell_bit-1;
        if (C[rand_d][j][right]>(1<<l1)-1 or C[rand_d][j][right]<-(1<<l1)) {
            if (right==cell_num-1) {
                new_right=left-1 ;    
                new_left=new_right-1;
                while (new_left>=0 and indicators[rand_d][j][new_left]) new_left-=1;
                new_left+=1;
                C[rand_d][j][right]+=C[rand_d][j][new_right];
                indicators[rand_d][j][new_right]=1;  
            }
            else {
                new_left=right+1;
                new_right=new_left;
                while (new_right<cell_num and indicators[rand_d][j][new_right]) new_right+=1;
                C[rand_d][j][new_right] += C[rand_d][j][right];
                indicators[rand_d][j][right] = 1;
            }
        }
        if (T <= abs(C[rand_d][j][right])) {
			float r = (float)(rand()) / ((float)(RAND_MAX));
			if (r < p) {
                unsigned int flow = 0;
                memcpy(&flow, str, 4);
				flow_labels[flow]++;
			}
		}
	}

    int Query(char * str)
	{
        unsigned int flow = 0;
        memcpy(&flow, str, 4);
		if (flow_labels.find(flow) != flow_labels.end()) {
            int cur = flow_labels[flow];
			return min(cur, d) * T + (int)(cur * 1.0 / p);
		}
		return 1;
	}

	// int Offline_query(const char * str, double alpha = 800.0, int steps = 800)
	// {
	// 	if (!offline_query) {
	// 		Solve(alpha, steps);
	// 	}
    //     string flow = string(str);
	// 	if (estimate_table.find(flow) != estimate_table.end()) {
	// 		return estimate_table[flow];
	// 	}
	// 	return 1;
	// }


	// void Solve(double alpha, int steps) {
	// 	offline_query = true;
	// 	int sign_value[2] = { -1, 1 };
	// 	int len_x_table = flow_labels.size();
	// 	ArrayXd x_table = ArrayXd::Zero(len_x_table, 1);
	// 	// unsigned int* flow_table = new unsigned int[len_x_table];
	// 	string* flow_table = new string[len_x_table];
	// 	ArrayXd ones = ArrayXd::Ones(len_x_table, 1);

	// 	unsigned int** all_j = new unsigned int* [d];
	// 	for (int i = 0; i < d; i++) {
	// 		all_j[i] = new unsigned int[len_x_table];
	// 	}
	// 	unsigned int** all_s_i = new unsigned int* [d];
	// 	for (int i = 0; i < d; i++) {
	// 		all_s_i[i] = new unsigned int[len_x_table];
	// 	}
	// 	unsigned int j, s_i, cell_i;
    //     string flow;
	// 	int cnt = 0;
	// 	for (auto it = flow_labels.begin(); it != flow_labels.end(); it++) {
	// 		flow = it->first;
	// 		for (int i = 0; i < d; i++) {
	// 			// MurmurHash3_x86_32(&flow, 4, hash_seeds[i], &j);
    //             j = bobhash[i].run(flow.c_str(), strlen(flow.c_str()));
	// 			s_i = j & 1;
    //             cell_i = ((j>>1)&0b111111) % cell_num;
    //             j = (j >> 7) % m;

    //             while (cell_i < cell_num and indicators[i][j][cell_i]) cell_i+=1;

    //             j = j*cell_num + cell_i;

	// 			all_j[i][cnt] = i * m + j;
	// 			all_s_i[i][cnt] = s_i;
	// 		}


	// 		x_table[cnt] = min(it->second, d) * T + (int)(it->second * 1.0 / p);
	// 		flow_table[cnt] = flow;
	// 		cnt++;
	// 	}
		
    //     ArrayXd new_C_nij = ArrayXd::Zero(d * m * cell_num);

    //     for (int i_index = 0; i_index < len_x_table; i_index++) {
    //         int flow_size = x_table[i_index];
    //         for (int i = 0; i < d; i++) {
    //             j = all_j[i][i_index];
    //             s_i = all_s_i[i][i_index];
    //             new_C_nij[j] += flow_size * sign_value[s_i] / d;
    //         }
    //     }

    //     for (int i = 0; i < d; i++) {
    //         for (int j = 0; j < m; j++) {
    //             for (int cell_i = 0; cell_i < cell_num; cell_i++) {
    //                 new_C_nij[i * m * cell_num + j * cell_num + cell_i] -= C[i][j][cell_i];
    //             }
    //         }
    //     }

	// 	double beta1 = 0.9;
	// 	double beta2 = 0.999;
	// 	double epsilon = 1e-8;
	// 	ArrayXd  m_t = ArrayXd::Zero(len_x_table, 1);
	// 	ArrayXd  v_t = ArrayXd::Zero(len_x_table, 1);
	// 	ArrayXd  m_bias = ArrayXd::Zero(len_x_table, 1);
	// 	ArrayXd  v_bias = ArrayXd::Zero(len_x_table, 1);
	// 	ArrayXd  g_t = ArrayXd::Zero(len_x_table, 1);
	// 	double beta1_i = beta1;
	// 	double beta2_i = beta2;
	// 	double alpha_t;
	// 	double temp_gradient;
	// 	double change_value;

	// 	for (int one_step = 0; one_step < steps; one_step++) {
	// 		for (int i_index = 0; i_index < len_x_table; i_index++) {
	// 			temp_gradient = 0;
	// 			for (int i = 0; i < d; i++) {
	// 				j = all_j[i][i_index];
	// 				s_i = all_s_i[i][i_index];
	// 				temp_gradient += new_C_nij[j] * sign_value[s_i];
	// 			}
	// 			g_t[i_index] = temp_gradient * 2.0 / d;
	// 		}

	// 		m_t = beta1 * m_t + (1 - beta1) * g_t;
	// 		v_t = beta2 * v_t + (1 - beta2) * g_t.square();

	// 		m_bias = m_t / (1 - beta1_i);
	// 		v_bias = v_t / (1 - beta2_i);
	// 		alpha_t = alpha * sqrt(1 - beta2_i) / (1 - beta1_i);
	// 		beta1_i *= beta1;
	// 		beta2_i *= beta2;

	// 		auto old_x_table = x_table;
	// 		x_table = x_table - alpha_t * m_bias / (v_bias.sqrt() + epsilon);
	// 		x_table = x_table.max(ones);

	// 		for (int i_index = 0; i_index < len_x_table; i_index++) {
	// 			change_value = (x_table[i_index] - old_x_table[i_index]) / d;
	// 			for (int i = 0; i < d; i++) {
	// 				j = all_j[i][i_index];
	// 				s_i = all_s_i[i][i_index];
	// 				new_C_nij[j] += sign_value[s_i] * change_value;
	// 			}
	// 		}
	// 	}
	// 	for (int i_index = 0; i_index < len_x_table; i_index++) {
	// 		estimate_table[flow_table[i_index]] = x_table[i_index];
	// 	}

	// 	delete[]all_j;
	// 	delete[]all_s_i;
	// 	delete[]flow_table;
	// }

	~MNRCSketch()
	{
		for (int i = 0; i < d; i++)
		{
			delete[]C[i];
		}

		delete[]hash_seeds;


	}
};
#endif//_MNRCSketch_H
