## 1、实验内容

- 实现B+树结构

- 将给定的127万个字符串（dict.txt）利用Trie树存储
- 给定的待匹配字符串（string.txt），将查找到的单词写入result.txt

## 2、主要数据结构和流程

### 2.1 BplusTreeNode

- 数据结构

  ```c++
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
  };BplusTree
  ```

### 2.2 BplusTree

- 数据结构

  ```c++
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
  }
  ```

- 插入key

  - `Insert`

    ```c++
    /**
     * 插入
     * @param key 待插入的key
     */
    void BplusTree::Insert(char* key) {
        char *key_copy = new char[strlen(key)];
        strcpy(key_copy, key);
        root = Recursive_Insert(root, key_copy, 0, nullptr);
    }
    ```

    

  - `Recursive_Insert`

    ```c++
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
    ```

    

  - `Insert_Element`

    ```c++
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
    ```

  - `SplitNode`

    ```c++
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
    ```

  - `FindSibling`

    ```c++
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
    ```

  - `MoveElement`

    ```c++
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
    ```

  - `RemoveElement`

    ```c++
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
    ```
    
  - `FindMostRight`
  
    ```c++
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
    ```
  
  - `FindMostLeft`
  
    ```c++
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
    ```
  
- 查找key

  - `Find`

    ```c++
    /**
     * 查找key
     * @param key   待查找的key
     * @return      是否找到
     */
    bool BplusTree::Find(char* key) {
        return FindByNode(root, key);
    }
    ```

  - `FindByNode`

    ```c++
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
    ```

## 3、实验过程

1. 创建Bplus树对象
2. 读取dict.txt，将单词插入到B+树中
3. 读取string.txt，在B+树中查找，若查到则将其写入result.txt

## 4、遇到的问题

无

## 5、结果指标

准确率： 100% 查到6306个单词

| M    | 时间（ms） | 内存峰值（MB） |
| ---- | ---------- | -------------- |
| 3    | 1455       | 71             |
| 5    | 1214       | 39             |
| 10   | 1220       | 23             |
| 15   | 1250       | 24             |
| 20   | 1339       | 19             |

## 6、结论和总结

​	从结果指标能够发现，随着B+树的阶数（M）增大，内存峰值在不断减少，原因是M增大后，树的高度降低，整体节点数变少，因此内存占用变少，但同时时间开销先减少后增大，因为当M过大时，虽然查找的层数降低了，但是每层需要比较的次数变多了，当M很大的情况下，时间复杂度接近于O（N），当M取值合适时，查找复杂度为Log（M，N）
