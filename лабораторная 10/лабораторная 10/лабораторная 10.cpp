#include <iostream>
#include <string>
#include <map>
#include <chrono>
#include <algorithm>
#include <random>
#include <climits>

using namespace std;
using namespace chrono;

// Часть 1: AVL-дерево
struct AVLNode {
    int key, height;
    AVLNode* left, * right;

    AVLNode(int k) {
        key = k;
        height = 1;
        left = right = nullptr;
    }
};

int h(AVLNode* n) { return n ? n->height : 0; }

int bal(AVLNode* n) { return n ? h(n->left) - h(n->right) : 0; }

void upd(AVLNode* n) {
    if (n) n->height = 1 + max(h(n->left), h(n->right));
}

// Правый поворот (случай Left-Left)
AVLNode* rotRight(AVLNode* y) {
    AVLNode* x = y->left;
    y->left = x->right;
    x->right = y;
    upd(y); upd(x);
    return x;
}

// Левый поворот (случай Right-Right)
AVLNode* rotLeft(AVLNode* x) {
    AVLNode* y = x->right;
    x->right = y->left;
    y->left = x;
    upd(x); upd(y);
    return y;
}

AVLNode* avlInsert(AVLNode* root, int key) {
    // 1. Обычная вставка BST
    if (!root) return new AVLNode(key);
    if (key < root->key) root->left = avlInsert(root->left, key);
    else if (key > root->key) root->right = avlInsert(root->right, key);
    else return root;

    // 2. Обновляем высоту
    upd(root);

    // 3. Балансируем
    int b = bal(root);
    if (b > 1 && key < root->left->key) return rotRight(root);       // LL
    if (b < -1 && key > root->right->key) return rotLeft(root);      // RR
    if (b > 1 && key > root->left->key) {                            // LR
        root->left = rotLeft(root->left);
        return rotRight(root);
    }
    if (b < -1 && key < root->right->key) {                          // RL
        root->right = rotRight(root->right);
        return rotLeft(root);
    }
    return root;
}

bool avlSearch(AVLNode* root, int key) {
    if (!root) return false;
    if (root->key == key) return true;
    if (key < root->key) return avlSearch(root->left, key);
    return avlSearch(root->right, key);
}

// Доп.задание: поиск минимума (идём влево до конца)
int avlMin(AVLNode* root) {
    if (!root) return INT_MIN;
    while (root->left) root = root->left;
    return root->key;
}

void delAVL(AVLNode* root) {
    if (!root) return;
    delAVL(root->left);
    delAVL(root->right);
    delete root;
}


// Часть 2: TRIE (префиксное дерево)

struct TrieNode {
    map<char, TrieNode*> next;
    bool isEnd = false;
};

TrieNode* trieInsert(TrieNode* root, string word) {
    if (!root) root = new TrieNode();
    TrieNode* cur = root;
    for (char c : word) {
        if (!cur->next[c]) cur->next[c] = new TrieNode();
        cur = cur->next[c];
    }
    cur->isEnd = true;
    return root;
}

bool trieSearch(TrieNode* root, string word) {
    if (!root) return false;
    TrieNode* cur = root;
    for (char c : word) {
        if (!cur->next[c]) return false;
        cur = cur->next[c];
    }
    return cur->isEnd;
}

// Доп.задание: поиск по префиксу
void findPrefix(TrieNode* node, string current, string& prefix, string result[], int& count) {
    if (!node) return;
    if (node->isEnd) {
        result[count++] = current; // нашли слово
    }
    for (auto& p : node->next) {
        findPrefix(p.second, current + p.first, prefix, result, count);
    }
}

int trieByPrefix(TrieNode* root, string prefix, string result[]) {
    if (!root) return 0;
    TrieNode* cur = root;
    // 1. Идём к концу префикса
    for (char c : prefix) {
        if (!cur->next[c]) return 0;
        cur = cur->next[c];
    }
    // 2. Собираем все слова
    int count = 0;
    findPrefix(cur, prefix, prefix, result, count);
    return count;
}


// Часть 3: BST (для сравнения)

struct BSTNode {
    int key;
    BSTNode* left, * right;
    BSTNode(int k) : key(k), left(nullptr), right(nullptr) {}
};

BSTNode* bstInsert(BSTNode* root, int key) {
    if (!root) return new BSTNode(key);
    if (key < root->key) root->left = bstInsert(root->left, key);
    else if (key > root->key) root->right = bstInsert(root->right, key);
    return root;
}

bool bstSearch(BSTNode* root, int key) {
    if (!root) return false;
    if (root->key == key) return true;
    if (key < root->key) return bstSearch(root->left, key);
    return bstSearch(root->right, key);
}

void delBST(BSTNode* root) {
    if (!root) return;
    delBST(root->left);
    delBST(root->right);
    delete root;
}


// Замер времени
void test(int N, bool sorted) {
    int* data = new int[N];
    for (int i = 0; i < N; i++) data[i] = i;

    if (!sorted) {
        // Перемешиваем (случайные данные)
        random_device rd;
        mt19937 g(rd());
        shuffle(data, data + N, g);
    }

    // AVL 
    AVLNode* avl = nullptr;
    auto t1 = high_resolution_clock::now();
    for (int i = 0; i < N; i++) avl = avlInsert(avl, data[i]);
    auto t2 = high_resolution_clock::now();
    double avlIns = duration<double>(t2 - t1).count();

    t1 = high_resolution_clock::now();
    for (int i = 0; i < N; i++) avlSearch(avl, data[i]);
    t2 = high_resolution_clock::now();
    double avlSrch = duration<double>(t2 - t1).count();

    // BST 
    BSTNode* bst = nullptr;
    t1 = high_resolution_clock::now();
    for (int i = 0; i < N; i++) bst = bstInsert(bst, data[i]);
    t2 = high_resolution_clock::now();
    double bstIns = duration<double>(t2 - t1).count();

    t1 = high_resolution_clock::now();
    for (int i = 0; i < N; i++) bstSearch(bst, data[i]);
    t2 = high_resolution_clock::now();
    double bstSrch = duration<double>(t2 - t1).count();

    // Вывод
    cout << N << "\t" << avlIns << "\t" << avlSrch
        << "\t" << bstIns << "\t" << bstSrch << endl;

    // Чистка
    delAVL(avl);
    delBST(bst);
    delete[] data;
}

int main() {
    setlocale(LC_ALL, "rus");
    // Часть 1 
    cout << " Часть 1: AVL " << endl;
    AVLNode* avl = nullptr;
    int keys[] = { 10, 20, 30, 40, 50, 25 };
    for (int k : keys) avl = avlInsert(avl, k);
    cout << "Минимум: " << avlMin(avl) << " (ожидается 10)" << endl;
    delAVL(avl);

    
    cout << "\n Часть 2: TRIE " << endl;
    TrieNode* trie = nullptr;
    string words[] = { "apple", "app", "apricot", "banana" };
    for (string w : words) trie = trieInsert(trie, w);

    cout << "Поиск 'apple': " << (trieSearch(trie, "apple") ? "True" : "False") << endl;
    cout << "Поиск 'apples': " << (trieSearch(trie, "apples") ? "True" : "False") << endl;

    string res[10]; // максимум 10 слов
    int cnt = trieByPrefix(trie, "ap", res);
    cout << "Слова с 'ap': ";
    for (int i = 0; i < cnt; i++) cout << res[i] << " ";
    cout << endl;

    cout << "\n Часть 3: сравнение" << endl;
    cout << "N\tAVL ins\tAVL srch\tBST ins\tBST srch" << endl;

    cout << "\nСлучайные:" << endl;
    for (int N : {1000, 5000, 10000}) test(N, false);

    cout << "\nОтсортированные (худший случай BST):" << endl;
    for (int N : {1000, 5000, 10000}) test(N, true);

    return 0;
}