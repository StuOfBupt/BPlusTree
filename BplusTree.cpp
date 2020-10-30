//
// Created by 王尚荣 on 2020/9/26.
//

#include "BplusTree.h"
#include <queue>
using namespace std;

TreeNode::TreeNode(int m) {
    M = m;
    keyNums = 0;
    Keys = new char*[M + 1];
    child = new TreeNode*[M + 1];
    for(int i = 0; i <= M; i++) {
        child[i] = nullptr;
        Keys[i] = nullptr;
    }
    next = nullptr;

}
/**
 * TreeNode节点析构函数
 * 当节点为叶子节点时才会释放空间
 */
TreeNode::~TreeNode() {
    if(child[0] == nullptr){
        for(int i = 0; i < keyNums; i++)
            delete Keys[i];
    }else{
        for(int i = 0; i < keyNums; i++)
            delete child[i];
    }
    delete[] Keys;
    delete[] child;
}

BplusTree::BplusTree(int m) {
    if(m < 3)
        throw "m must be large than 3!";
    M = m;
    root = new TreeNode(M);
}
/**
 * 插入
 * @param key 待插入的key
 */
void BplusTree::Insert(char* key) {
    char *key_copy = new char[strlen(key)];
    strcpy(key_copy, key);
    root = Recursive_Insert(root, key_copy, 0, nullptr);
}
/**
 * 递归插入key
 * @param T 插入的节点
 * @param key 插入的值
 * @param i 当前节点是其父节点的第i叉
 * @param parent 节点T的父节点
 */
TreeNode* BplusTree::Recursive_Insert(TreeNode* T, char* key, int i, TreeNode* parent) {
    int j = 0;
    while(j < T->keyNums && strcmp(key, T->Keys[j]) >= 0){
        if(strcmp(key, T->Keys[j]) == 0)
            return T;         //有重复的值，则无需插入
        j++;
    }
    if( j != 0 && T->child[0] != nullptr)
        j--;
    if(T->child[0] == nullptr){
        // T 是叶子节点
        T = InsertElement(true, parent, T, key, i, j);
    }else{
        // T 是索引节点
        T->child[j] = Recursive_Insert(T->child[j], key, j, T);
    }
    //调整节点
    int Limit = M;
    TreeNode* Sibling = nullptr;
    if(T->keyNums > Limit){
        if(parent == nullptr){
            // T 为根节点的情况
            T = SplitNode(parent, T, i);
        }else{
            Sibling = FindSibling(parent, i);
            if(Sibling){
                // 存在有多余key空间的兄弟节点,将多出的一个元素移动过去
                MoveElement(T, Sibling, parent, i, 1);
            }else{
                // 兄弟节点没有空余key空间,分裂节点
                T = SplitNode(parent, T, i);
            }
        }

    }
    // 当有父节点时，更新父节点i处key的值
    if(parent){
        parent->Keys[i] = T->Keys[0];
    }
    return T;
}
/**
 * 对节点插入一个元素（key 或 node）：
 *                  对父节点插入T节点
 *                  对T节点插入key值
 * @param isKey     插入的是值还是节点
 * @param parent    T节点的父节点
 * @param T         插入的节点
 * @param key       插入的值
 * @param i         对T节点插入值key时，i为T在parent的位置,j为key要插入的位置
 * @param j         对parent节点插入T节点时，i为待插入的位置。j、key无用
 * @return          返回插入的节点或插入key的节点
 */
TreeNode *BplusTree::InsertElement(bool isKey, TreeNode* parent, TreeNode* T, char* key, int i, int j) {
    int k;
    if(isKey){
        //  将key插入到T节点处
        k = T->keyNums - 1;
        //一趟插入排序
        while(k >= j){
            T->Keys[k + 1] = T->Keys[k];
            k-- ;
        }
        // 插入 key
        T->Keys[j] = key;
        if(parent){
            //必要时更新父节点key
            parent->Keys[i] = T->Keys[0];
        }
        T->keyNums++;
    } else{
        // 将节点T插入到parent中
        if(T->child[0] == nullptr){
            // 若T为叶子节点， 则需要重新连接叶子节点
            if(i > 0)
                parent->child[i - 1]->next = T;
            T->next = parent->child[i];
        }
        k = parent->keyNums - 1;
        while (k >= i){
            // 需要同时更新孩子节点和key列表
            parent->child[k + 1] = parent->child[k];
            parent->Keys[k + 1] = parent->Keys[k];
            k--;
        }
        parent->child[i] = T;
        parent->Keys[i] = T->Keys[0];
        parent->keyNums++;
    }

    return T;
}
/**
 * 分裂节点
 * @param parent    T节点的父节点
 * @param T         待分裂的节点
 * @param i         该节点在父节点的位置
 * @return          返回插入后的节点（若创建新根则返回新的根节点）
 */
TreeNode *BplusTree::SplitNode(TreeNode* parent, TreeNode* T, int i) {
    int j,k,Limit;
    TreeNode* newNode = new TreeNode(M);
    k = 0;
    j = T->keyNums / 2;
    Limit = T->keyNums;
    //将有半部分复制到新节点中
    while (j < Limit){
        if(T->child[0] != nullptr){
            //针对索引节点，复制其子节点指针
            newNode->child[k] = T->child[j];
            T->child[j] = nullptr;
        }
        newNode->Keys[k] = T->Keys[j];
        T->Keys[j] = nullptr;
        newNode->keyNums++ ;
        T->keyNums--;
        k++;
        j++;
    }
    if(parent != nullptr){
        //将新节点插入到父节点中，位置是i+1 (当前节点的位置为i)
        InsertElement(false, parent, newNode, nullptr, i + 1, 0);
    }else{
        //当前节点T为根节点，分裂之后则需要创建新的根节点
        parent = new TreeNode(M);
        //将两节点插入到根节点中的0，1位置上去
        InsertElement(false, parent, T, nullptr, 0, 0);
        InsertElement(false, parent, newNode, nullptr, 1, 0);
        return parent;
    }
    return T;
}
/**
 * 查找兄弟节点，其存储的key未满，否则返回nullptr
 * @param parent 父节点
 * @param i      当前节点在父节点的位置
 * @return       有多余key空间的兄弟节点或nullptr
 */
TreeNode *BplusTree::FindSibling(TreeNode *parent, int i) const {
    TreeNode* Sibling = nullptr;
    int Limit = M;
    if(i == 0){
        if(parent->child[1]->keyNums < Limit)
            Sibling = parent->child[1];
    } else if(parent->child[i - 1]->keyNums < Limit)
        Sibling = parent->child[i - 1];
    else if(i+1 < parent->keyNums && parent->child[i + 1]->keyNums < Limit)
        Sibling = parent->child[i + 1];
    return Sibling;
}
/**
 * 移动元素， Src和Dst为相邻节点
 * @param Src       源节点
 * @param Dst       目的节点
 * @param parent    父节点
 * @param i         Src在父节点中的位置
 * @param n         移动元素的个数
 * @return          父节点
 */
TreeNode *BplusTree::MoveElement(TreeNode *Src, TreeNode *Dst, TreeNode *parent, int i, int n) {
    char* tempKey;
    TreeNode* child;
    int j = 0;
    bool isSrcFirst = false;        //是否Src节点在前面
    if(strcmp(Src->Keys[0], Dst->Keys[0]) < 0)
        isSrcFirst = true;

    if(isSrcFirst){
        // 节点Src 在 Dst前
        if(Src->child[0]){
            //Src 为索引节点
            while(j < n){
                child = Src->child[Src->keyNums - 1];
                RemoveElement(false, Src, child, Src->keyNums - 1, 0);
                // 由于Src 在 Dst前面， 因此Src的子节点移动到Dst时需要插入到最前面，即 i = 0
                InsertElement(false, Dst, child, nullptr, 0, 0);
                j++;
            }
        }else{
            //Src 为叶子节点
            while(j < n){
                tempKey = Src->Keys[Src->keyNums - 1];
                RemoveElement(true, parent, Src, i, Src->keyNums - 1);
                // Src 在 parent中的位置是i Dst 在 parent中的位置是 i + 1
                InsertElement(true, parent, Dst, tempKey, i + 1, 0);
                j++;
            }
        }
        //更新父节点的Keys
        parent->Keys[i + 1] = Dst->Keys[0];
        //更新Src 和 Dst下的叶子节点的连接
        if(Src->keyNums > 0)
            FindMostRight(Src)->next = FindMostLeft(Dst);

    }else{
        // 节点Src 在 Dst 后
        if(Src->child[0]){
            //Src 为索引节点
            while(j < n){
                child = Src->child[0];
                RemoveElement(false, Src, child, 0, 0);
                // Src 在 Dst后面，因此需要将Src的最左边的孩子移动到Dst最右边，即 i = Dst->keyNums
                InsertElement(false, Dst, child, nullptr, Dst->keyNums, 0);
                j++;
            }
        }else{
            // Src 为叶子节点
            while (j < n){
                tempKey = Src->Keys[0];
                RemoveElement(true, parent, Src, i, 0);
                InsertElement(true, parent, Dst, tempKey, i - 1, Dst->keyNums);
                j++;
            }
        }
        // 更新父节点i的值
        parent->Keys[i] = Src->Keys[0];
        //更新Src 和 Dst下的叶子节点的连接
        if(Src->keyNums > 0)
            FindMostRight(Dst)->next = FindMostLeft(Src);

    }
    return parent;
}
/**
 * 移除节点中的一个元素（key 或 node）:
 *                  移除parent下的T节点
 *                  移除T节点中的key
 * @param isKey     移除的是否为key
 * @param parent    节点T的父节点
 * @param T         节点T
 * @param i         节点T在父节点中的位置
 * @param j         key在节点T中的位置
 * @return          移除key后的节点或移除的节点
 */
TreeNode *BplusTree::RemoveElement(bool isKey, TreeNode *parent, TreeNode *T, int i, int j) {
    int k, Limit;
    if(isKey){
        // 从节点T移除key, key在T的j位置
        Limit = T->keyNums;
        k = j + 1;
        while(k < Limit){
            T->Keys[k - 1] = T->Keys[k];
            k++;
        }
        T->Keys[Limit - 1] = nullptr;
        parent->Keys[i] = T->Keys[0];
        T->keyNums--;
    }else{
        // 将T节点从parent中删除
        if(T->child[0] == nullptr && i > 0){
            // 当T为叶子节点并且不在最左边,修改叶子节点的连接
            parent->child[i-1]->next = parent->child[i + 1];
        }
        Limit = parent->keyNums;
        k = i + 1;
        while(k < Limit){
            parent->Keys[k - 1] = parent->Keys[k];
            parent->child[k - 1] = parent->child[k];
            k++;
        }
        parent->child[Limit - 1] = nullptr;
        parent->Keys[Limit - 1] = nullptr;
        parent->keyNums--;
    }
    return T;
}
/**
 * 查找T节点所在树枝中最右边的叶子节点
 * @param T     叶子节点或索引节点
 * @return      最右边的叶子节点
 */
TreeNode *BplusTree::FindMostRight(TreeNode *T) {
    TreeNode* tmp = T;
    while(tmp && tmp->child[tmp->keyNums - 1]){
        tmp = tmp->child[tmp->keyNums - 1];
    }
    return tmp;
}
/**
 * 查找T节点所在树枝中最左边的叶子节点
 * @param T     叶子节点或索引节点
 * @return      最左边的叶子节点
 */
TreeNode *BplusTree::FindMostLeft(TreeNode *T) {
    TreeNode* tmp = T;
    while(tmp && tmp->child[0]){
        tmp = tmp->child[0];
    }
    return tmp;
}
/**
 * 查找key
 * @param key   待查找的key
 * @return      是否找到
 */
bool BplusTree::Find(char* key) {
    return FindByNode(root, key);
}

/**
 * 格式化打印出树
 */
void BplusTree::FormatPrint() {
    queue<pair<TreeNode*, int>> q;
    TreeNode* p ;
    int level = 0;
    q.push({root, level});
    while(!q.empty()){
        auto tmp = q.front();
        q.pop();
        p = tmp.first;
        int cur_level = tmp.second;
        if(p == nullptr)
            continue;
        if(cur_level > level){
            printf("\n");
            level = cur_level;
        }
        printf("[");
        for(int i = 0; i < p->keyNums; i++)
            printf("%s,", p->Keys[i]);

        for(int i = p->keyNums; i < M; i++)
            printf("NULL,");
        printf("]");
        if(p->child[0]){
            // p为索引节点
            printf("       ");
            for(int i = 0; i < p->keyNums; i++)
                q.push({p->child[i], cur_level + 1});
        }else{
            // p为叶子节点
            printf("--->");
        }
    }
}

bool BplusTree::FindByNode(TreeNode *node, char* key) {
    if(node == nullptr)
        return false;
    if(node->child[0]){
        // node为索引节点
        int i = 0;
        while(i < node->keyNums && strcmp(key, node->Keys[i]) >= 0){
            if(strcmp(key, node->Keys[i]) == 0)
                return true;
            i++;
        }
        if(i > 0)
            i--;
        return FindByNode(node->child[i], key);
    }else{
        // node为叶子节点
        for(int i = 0; i < node->keyNums; i++)
            if(strcmp(node->Keys[i], key) == 0)
                return true;
        return false;
    }
}

BplusTree::~BplusTree() {
    delete root;
}
