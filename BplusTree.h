//
// Created by 王尚荣 on 2020/9/26.
//

#ifndef BPLUSTREE_BPLUSTREE_H
#define BPLUSTREE_BPLUSTREE_H
#include <cstdlib>
#include <cstdio>
#include <string.h>

class TreeNode{
public:
    TreeNode(int m);
    ~TreeNode();
    int keyNums;        // 当前key个数
    char **Keys;        // 存储key值
    TreeNode **child;   // 存储子节点
    TreeNode *next;     // 当且节点为叶子节点时有用
private:
    int M;
};

class BplusTree {
public:
    BplusTree(int m);
    void Insert(char* key);
    bool Find(char* key);
    void FormatPrint();
    ~BplusTree();
private:
    TreeNode* Recursive_Insert(TreeNode* T, char* key, int i, TreeNode* parent);
    TreeNode* InsertElement(bool isKey, TreeNode* parent, TreeNode* T, char* key, int i, int j);
    TreeNode* SplitNode(TreeNode* parent, TreeNode* T, int i);
    TreeNode* FindSibling(TreeNode* parent, int i) const;
    TreeNode* MoveElement(TreeNode* Src, TreeNode* Dst, TreeNode* parent, int i, int n);
    TreeNode* RemoveElement(bool isKey, TreeNode* parent, TreeNode* T, int i, int j);
    TreeNode* FindMostRight(TreeNode* T);
    TreeNode* FindMostLeft(TreeNode* T);
    bool FindByNode(TreeNode* node, char* key);
    TreeNode* root;
    int M;
};


#endif //BPLUSTREE_BPLUSTREE_H
