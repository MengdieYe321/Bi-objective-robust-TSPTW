#pragma once

#include <iostream>

//min binary heap according to value in array 'label'
class binheap
{
public:
	int nheap;
	int maxsize = 0;
	int* key;
	int* pos;

	binheap(int maxsize) 
		: maxsize(maxsize)
	{
		nheap = 0;
		maxsize = maxsize;
		key = new int[maxsize];
		pos = new int[maxsize];
	}

	~binheap()
	{
		delete[] key;
		delete[] pos;
	}

	int moveup(double* label, int i)
	{
		if (i < 0) return -1;

		int p = (int)((int)(i - 1) * 0.5); 

		if (label[key[i]] < label[key[p]])
		{
			int tmp = key[i];

			key[i] = key[p];

			key[p] = tmp;

			tmp = pos[key[i]];

			pos[key[i]] = pos[key[p]];

			pos[key[p]] = tmp;

			return p;
		}

		return -1;
	}

	int movedown(double* label, int i)
	{
		if (2 * i + 1 > nheap) return nheap + 1;

		int inew = i;

		if (label[key[2 * i + 1]] < label[key[inew]])
		{
			inew = 2 * i + 1;
		}

		if (2 * i + 2 < nheap) 
		{
			if (label[key[2 * i + 2]] < label[key[inew]])
			{
				inew = 2 * i + 2;
			}
		}
		if (inew != i)
		{
			int tmp = key[i];

			key[i] = key[inew];

			key[inew] = tmp;

			tmp = pos[key[i]];

			pos[key[i]] = pos[key[inew]];

			pos[key[inew]] = tmp;

			return inew;
		}

		return nheap;
	}

	/*
	the parent conpare with left child first using lable, smaller one
	the conpare with the right child using lable, smaller one
	no scequence between left and right child (lable)
	every group includes 3 nodes
	*/
	void add(double* label, int k)
	{
		if (nheap > maxsize)
		{
			std::cout << " Heap overflow: terminating program \n Press Enter to quit ";

			std::cin.get();

			exit(0);
		}

		nheap = nheap + 1;

		key[nheap - 1] = k;

		pos[k] = nheap - 1;

		int i = nheap - 1;

		while (i > 0)
		{
			i = moveup(label, i);
		}
	}

	/*
	choose the first root node accroding to the smallest label
	the one which is being chosen always has the smallest label
	*/
	int choose(double* label)
	{
		if (nheap < 1) return -1;

		int j = key[0];

		int i = 0; 

		pos[j] = -1;

		if (nheap > 1)
		{
			key[0] = key[nheap - 1];

			pos[key[nheap - 1]] = 0;

		}
		nheap = nheap - 1;	

		while (i < nheap - 1)
		{
			i = movedown(label, i);
		}

		return j;
	}

	void decreased_label(double* label, int k)
	{
		int i = pos[k];

		while (i > 0)
		{
			i = moveup(label, i);
		}
	}

	void increased_label(double* label, int k)
	{
		int i = pos[k];

		while (i < nheap - 1)
		{
			i = movedown(label, i);
		}
	}

};

//binheap bhp(7);
//int key = 0;
//double *lable;
//lable = (double*)malloc(7 * sizeof(double));
//lable[0] = 5;
//lable[1] = 4;
//lable[2] = 1;
//lable[3] = 6;
//lable[4] = 7;
//lable[5] = 3;
//lable[6] = 2;
//
//while (key <= 6) {
//	bhp.add(lable, key);
//	key++;
//	///*for (int i = 0; i < key; i++) {
//	//	cout << i <<" " << bhp.key[i] << " " << lable[bhp.key[i]] << endl;
//	//}
//	//cout << endl;*/
//}
//
//cout << bhp.nheap << endl;
//for (int t = 0; t < bhp.nheap; t++) {
//	int j = bhp.choose(lable);
//	cout << "key: " << j << endl;
//	cout << "nheap: " << bhp.nheap << endl;
//	for (int i = 0; i < bhp.nheap; i++) {
//		cout << i << " " << bhp.key[i] << " " << lable[bhp.key[i]] << endl;
//	}
//	cout << endl;
//}

