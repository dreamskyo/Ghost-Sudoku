
#include "DLX.hpp"
#include <SDL3/SDL.h>
using namespace std;

// 映射函式：將 cell(row, col) 放入 num 對應到第幾欄
int DLX::cellCol(int r, int c) { return 9 * r + c; }          // 0~80
int DLX::rowCol(int r, int num) { return 81 + 9 * r + num; }  // 81~161
int DLX::colCol(int c, int num) { return 162 + 9 * c + num; } // 162~242
int DLX::boxCol(int r, int c, int num) {
    return 243 + 9 * ((r / 3) * 3 + (c / 3)) + num;       // 243~323
}

void DLX::randonsudoku() {

    vector<int> nums{ 0, 1, 2, 3, 4, 5, 6, 7, 8 };
    random_shuffle(nums.begin(), nums.end());
    
    // 清空盤面
    for(int i = 0; i < 9; ++i) {
        for(int j = 0; j < 9; ++j) {
            sudoku[i][j] = 0;
        }
    }

    // 隨機填入數字
    vector<int> available{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    random_shuffle(available.begin(), available.end());
    for(int i = 0; i < 9; ++i) {
        sudoku[i][nums[i]] = available[i];
    }
}

void DLX::initDLX(){
    // 初始化欄位 header，形成 circular linked list
    root = new Column();
    root->L = root->R = root;
    for (int i = 0; i < COLS; ++i) {
        columns[i].name = i;
        columns[i].U = columns[i].D = &columns[i];
        columns[i].S = 0;
        columns[i].C = &columns[i];

        // 插入到 root 的右邊
        columns[i].L = root->L;
        columns[i].R = root;
        root->L->R = &columns[i];
        root->L = &columns[i];
    }

    // 為每個可能的 (r, c, n) 建立對應節點
    for (int r = 0; r < N; ++r) {
        for (int c = 0; c < N; ++c) {
            for (int n = 0; n < N; ++n) {
                int rowID = (r * 81) + (c * 9) + n;
                vector<int> colIDs = {
                    cellCol(r, c),
                    rowCol(r, n),
                    colCol(c, n),
                    boxCol(r, c, n)
                };

                Node* first = nullptr;
                Node* prev = nullptr;

                for (int colID : colIDs) {
                    Column* col = &columns[colID];

                    // 建立節點
                    Node* node = new Node();
                    node->C = col;
                    node->rowID = rowID;

                    // 垂直插入 column
                    node->U = col->U;
                    node->D = col;
                    col->U->D = node;
                    col->U = node;
                    col->S++;

                    // 左右連結（行內）
                    if (!first) {
                        first = node;
                        node->L = node->R = node;
                    } else {
                        node->L = prev;
                        node->R = first;
                        prev->R = node;
                        first->L = node;
                    }

                    prev = node;
                    row_nodes[rowID].push_back(node); // 可選：儲存節點指標
                    node_pool.push_back(node);
                }
            }
        }
    }
    //cout << "初始化完成：729 行 × 324 欄\n";
}

// 移除欄位 col 與其相關列（行）
void DLX::cover(Column* col) {
    // 移除 col 自身
    col->R->L = col->L;
    col->L->R = col->R;

    // 遍歷 col 欄中所有節點（從下往下）
    for (Node* row = col->D; row != col; row = row->D) {
        // 對該節點所在的行，移除其他欄位中的節點
        for (Node* node = row->R; node != row; node = node->R) {
            node->D->U = node->U;
            node->U->D = node->D;
            ((Column*)(node->C))->S--; // 該欄剩餘節點數減一
        }
    }
}

// 還原欄位 col 與其所有相關節點（反向操作）
void DLX::uncover(Column* col) {
    // 反向遍歷 col 欄中的所有節點（從上往上）
    for (Node* row = col->U; row != col; row = row->U) {
        // 對該節點所在的行，還原其他欄位中的節點
        for (Node* node = row->L; node != row; node = node->L) {
            ((Column*)node->C)->S++;
            node->D->U = node;
            node->U->D = node;
        }
    }

    // 還原 col 自身
    col->R->L = col;
    col->L->R = col;
}

// 選擇節點最少的 Column，減少分支（最佳化）
DLX::Column* DLX::selectColumn() {
    Column* best = nullptr;
    int minSize = 1e9;
    for (Column* c = static_cast<Column*>(root->R); c != root; c = static_cast<Column*>(c->R)) {
        if (c->S < minSize) {
            minSize = c->S;
            best = c;
        }
    }
    return best;
}

// 主遞迴搜尋函式
bool DLX::search(int k) {
    if (root->R == root) {
        // 沒有欄位可選，表示解完整了
        return true;
    }

    Column* col = selectColumn();
    if (!col || col->S == 0) return false;

    cover(col);

    for (Node* row = col->D; row != col; row = row->D) {
        solution.push_back(row);

        // Cover 該行中所有欄（跳過自己）
        for (Node* node = row->R; node != row; node = node->R) {
            cover((Column*)(node->C));
        }

        if (search(k + 1)) {
            return true; // 找到解
        }

        // 回溯
        for (Node* node = row->L; node != row; node = node->L) {
            uncover((Column*)(node->C));
        }
        solution.pop_back();
    }

    uncover(col);
    return false;
}

// 預先 cover 題目已知的數字
void DLX::applyClues() {
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            int val = sudoku[r][c];
            if (val != 0) {
                int n = val - 1;
                int rowID = r * 81 + c * 9 + n;
                // 用這一行的所有節點做 cover
                for (Node* node : row_nodes[rowID]) {
                    cover((Column*)(node->C));
                }
            }
        }
    }
}

// 還原 solution[] 回數獨盤面
void DLX::extractAnswer() {
    // 先將所有格子初始化為 0
    for(int i = 0; i < 9; ++i) {
        for(int j = 0; j < 9; ++j) {
            (*answer)[i][j] = 0;
        }
    }

    // 從 solution 中還原答案
    for (Node* row : solution) {
        int rowID = row->rowID;
        int r = rowID / 81;
        int c = (rowID / 9) % 9;
        int n = rowID % 9;
        (*answer)[r][c] = n + 1;
    }

    // 檢查並補充缺失的數字
    for(int i = 0; i < 9; ++i) {
        for(int j = 0; j < 9; ++j) {
            if((*answer)[i][j] == 0) {
                // 建立已使用數字的標記
                bool used[10] = {false};
                
                // 檢查同一行
                for(int k = 0; k < 9; ++k) {
                    used[(*answer)[i][k]] = true;
                }
                
                // 檢查同一列
                for(int k = 0; k < 9; ++k) {
                    used[(*answer)[k][j]] = true;
                }
                
                // 檢查同一個 3x3 方格
                int boxRow = i - i % 3;
                int boxCol = j - j % 3;
                for(int r = 0; r < 3; ++r) {
                    for(int c = 0; c < 3; ++c) {
                        used[(*answer)[boxRow + r][boxCol + c]] = true;
                    }
                }
                
                // 找出可用的數字
                for(int num = 1; num <= 9; num++) {
                    if(!used[num]) {
                        (*answer)[i][j] = num;
                        //SDL_Log("Filled [%d][%d] with %d\n", i, j, num);
                        break;
                    }
                }
            }
        }
    }
}

// 印出盤面
void DLX::printGrid() {
    SDL_Log("Current grid state:\n");
    for (int r = 0; r < 9; ++r) {
        std::string row_str;
        for (int c = 0; c < 9; ++c) {
            int val = (*answer)[r][c];
            if(val < 1 || val > 9) {
                SDL_Log("Invalid value at [%d][%d]: %d\n", r, c, val);
            }
            row_str += std::to_string(val) + " ";
        }
        SDL_Log("%s\n", row_str.c_str());
    }
}

bool DLX::validateSudoku() {
    // 檢查每一行
    for(int i = 0; i < 9; ++i) {
        bool used[10] = {false};
        for(int j = 0; j < 9; ++j) {
            int val = (*answer)[i][j];
            if(val < 1 || val > 9 || used[val]) return false;
            used[val] = true;
        }
    }
    
    // 檢查每一列
    for(int j = 0; j < 9; ++j) {
        bool used[10] = {false};
        for(int i = 0; i < 9; ++i) {
            int val = (*answer)[i][j];
            if(val < 1 || val > 9 || used[val]) return false;
            used[val] = true;
        }
    }
    
    // 檢查每個 3x3 方格
    for(int box = 0; box < 9; ++box) {
        bool used[10] = {false};
        int boxRow = (box / 3) * 3;
        int boxCol = (box % 3) * 3;
        for(int i = 0; i < 3; ++i) {
            for(int j = 0; j < 3; ++j) {
                int val = (*answer)[boxRow + i][boxCol + j];
                if(val < 1 || val > 9 || used[val]) return false;
                used[val] = true;
            }
        }
    }
    
    return true;
}

void DLX::clearDLX() {
    // 先清理所有動態配置的節點
    for(Node* node : node_pool) {
        if(node != nullptr) {
            delete node;
        }
    }
    node_pool.clear();
    
    // 清理行節點的向量
    for (int i = 0; i < SIZE; ++i) {
        row_nodes[i].clear();
    }
    
    // 重置 columns
    for (int i = 0; i < COLS; ++i) {
        columns[i].U = columns[i].D = &columns[i];
        columns[i].L = columns[i].R = &columns[i];
        columns[i].S = 0;
    }
    
    // 清理根節點
    if(root != nullptr) {
        delete root;
        root = nullptr;
    }
    
    solution.clear();
    
    // 清空數獨盤面
    for(int i = 0; i < 9; ++i) {
        for(int j = 0; j < 9; ++j) {
            sudoku[i][j] = 0;
        }
    }
}

bool DLX::DLXmain(){
    // 重置所有資料結構
    clearDLX();
    
    // 使用當前時間作為種子，但只在第一次呼叫時設定
    static bool first_call = true;
    if(first_call) {
        srand(time(NULL));
        first_call = false;
    }
    
    // 生成新的數獨盤面
    randonsudoku();
    
    // 初始化 DLX 結構
    initDLX();
    
    // 套用已知數字
    applyClues();
    
    // 尋找解答
    if (search(0)) {
        extractAnswer();
        
        // 修正驗證邏輯
        if(!validateSudoku()){
            SDL_Log("錯誤解法!\n");
            clearDLX();
            return false;
        }
        
        //printGrid();
        return true;
    } else {
        SDL_Log("找不到解。\n");
        return false;
    }
}
