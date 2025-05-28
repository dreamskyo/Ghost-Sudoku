
#ifndef __DLX_HPP__
#define __DLX_HPP__

#include <iostream>
#include <vector>
#include <ctime>
using namespace std;

class DLX{

private:

	const int N = 9;
	const int SIZE = N * N * N; // 729 行
	const int COLS = 4 * N * N; // 324 欄

	struct Node {
    	Node *L, *R, *U, *D;
    	Node *C;  // 所屬欄的 Column Header
		int rowID;  // 該節點對應的 Sudoku 729 行中的哪一列
	};
	struct Column : Node {
    	int S;  // 目前欄中有幾個節點（1的數量）
    	int name; // 例如第0~323欄
	};

	int cellCol(int r, int c);
	int rowCol(int r, int num);
	int colCol(int c, int num); 
	int boxCol(int r, int c, int num);

	vector<int> cols;
	vector<vector<int>> sudoku;
	vector<Node*> solution;
	vector<vector<int>> *answer;
    vector<Column> columns;
    Node* root;
    vector<vector<Node*>> row_nodes; // 每行的節點（最多 4 個）	
	vector<Node*> node_pool;

public:

	DLX(vector<vector<int>> *output){
		answer = output;
		cols = {
        	cellCol(1, 2),
        	rowCol(1, 4),
        	colCol(2, 4),
        	boxCol(1, 2, 4)
    	};
		sudoku.assign(9,vector<int>(9,0)); // 0 表示空格，其餘是題目的提示數
		columns.resize(COLS);
		root = nullptr;
		row_nodes.resize(SIZE);
	}

	void randonsudoku();
	void initDLX();
	void cover(Column* col);
	void uncover(Column* col);
	Column* selectColumn();
	bool search(int k);
	void applyClues();
	void extractAnswer();
	void printGrid();
	bool validateSudoku();
	void clearDLX();
	bool DLXmain();

	~DLX(){
		clearDLX();
	};
	
};

#endif