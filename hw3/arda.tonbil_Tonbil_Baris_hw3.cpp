#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <chrono>
#include <cctype>
#include <algorithm>
#include <map>

using namespace std;

struct DocumentItem {
    string documentName;
    int count;
};

struct WordItem {
    string word;
    WordItem* left = nullptr;
    WordItem* right = nullptr;
    vector<DocumentItem> details = {};
    int height;

    WordItem(const string& word, WordItem* left = nullptr, WordItem* right = nullptr, int height = 0) :
        word(word), left(left), right(right), height(height), details({}) {}
};

string tolower_string(string s) {
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return tolower(c); });
    return s;
}

template <class Key, class Value>
class AVLSearchTree {
public:
    AVLSearchTree();
    ~AVLSearchTree();
    void insert(Key key);
    void remove(Key key);
    Value find(Key key) const;
    Value findMin() const;
    void makeEmpty();
    bool isEmpty() const;
    int getBalance(Value ptr) const;

    void rotateWithLeftChild(Value& k2);
    void rotateWithRightChild(Value& k1);
    void doubleWithLeftChild(Value& k3);
    void doubleWithRightChild(Value& k1);

private:
    Value root = nullptr;
    void insert(Key key, Value& ptr);
    void remove(Key key, Value& ptr);
    void makeEmpty(Value& ptr);
    Value find(Key key, Value ptr) const;
    Value findMin(Value ptr) const;
    int getHeight(Value ptr) const;
};

template <class Key, class Value>
AVLSearchTree<Key, Value>::AVLSearchTree() {
    root = nullptr;
}

template<class Key, class Value>
AVLSearchTree<Key, Value>::~AVLSearchTree() {
    makeEmpty();
}

template<class Key, class Value>
int AVLSearchTree<Key, Value>::getHeight(Value ptr) const {
    return ptr == nullptr ? -1 : ptr->height;
}

template<class Key, class Value>
void AVLSearchTree<Key, Value>::remove(Key key) {
    remove(key, root);
}

template<class Key, class Value>
void AVLSearchTree<Key, Value>::makeEmpty() {
    makeEmpty(root);
}

template<class Key, class Value>
void AVLSearchTree<Key, Value>::makeEmpty(Value& ptr) {
    if (ptr != nullptr) {
        makeEmpty(ptr->left);
        makeEmpty(ptr->right);
        delete ptr;
    }
    ptr = nullptr;
}

template<class Key, class Value>
void AVLSearchTree<Key, Value>::insert(Key key) {
    insert(key, root);
}

template<class Key, class Value>
void AVLSearchTree<Key, Value>::insert(Key key, Value& ptr) {
    if (ptr == nullptr) {
        ptr = new WordItem(key, nullptr, nullptr, 0);
    }
    else if (key < ptr->word) {
        insert(key, ptr->left);
        if (getHeight(ptr->left) - getHeight(ptr->right) == 2) {
            if (key < ptr->left->word)
                rotateWithLeftChild(ptr);
            else
                doubleWithLeftChild(ptr);
        }
    }
    else if (ptr->word < key) {
        insert(key, ptr->right);
        if (getHeight(ptr->right) - getHeight(ptr->left) == 2) {
            if (key > ptr->right->word)
                rotateWithRightChild(ptr);
            else
                doubleWithRightChild(ptr);
        }
    }
    ptr->height = max(getHeight(ptr->left), getHeight(ptr->right)) + 1;
}

template<class Key, class Value>
void AVLSearchTree<Key, Value>::remove(Key key, Value& ptr) {
    if (ptr == nullptr) return;

    if (key < ptr->word) {
        remove(key, ptr->left);
    }
    else if (key > ptr->word) {
        remove(key, ptr->right);
    }
    else if (ptr->left != nullptr && ptr->right != nullptr) {
        ptr->word = findMin(ptr->right)->word;
        remove(ptr->word, ptr->right);
    }
    else {
        Value oldNode = ptr;
        ptr = (ptr->left != nullptr) ? ptr->left : ptr->right;
        delete oldNode;
    }

    if (ptr != nullptr) {
        ptr->height = max(getHeight(ptr->left), getHeight(ptr->right)) + 1;
        int balance = getBalance(ptr);

        if (balance > 1) {
            if (getBalance(ptr->left) >= 0)
                rotateWithLeftChild(ptr);
            else
                doubleWithLeftChild(ptr);
        }
        else if (balance < -1) {
            if (getBalance(ptr->right) <= 0)
                rotateWithRightChild(ptr);
            else
                doubleWithRightChild(ptr);
        }
    }
}

template<class Key, class Value>
int AVLSearchTree<Key, Value>::getBalance(Value ptr) const {
    return (ptr == nullptr) ? 0 : getHeight(ptr->left) - getHeight(ptr->right);
}

template<class Key, class Value>
Value AVLSearchTree<Key, Value>::find(Key key) const {
    return find(key, root);
}

template<class Key, class Value>
Value AVLSearchTree<Key, Value>::find(Key key, Value ptr) const {
    key = tolower_string(key);
    if (ptr == nullptr) return nullptr;
    if (key < ptr->word) return find(key, ptr->left);
    if (key > ptr->word) return find(key, ptr->right);
    return ptr;
}

template<class Key, class Value>
Value AVLSearchTree<Key, Value>::findMin() const {
    return findMin(root);
}

template<class Key, class Value>
Value AVLSearchTree<Key, Value>::findMin(Value ptr) const {
    if (ptr == nullptr) return nullptr;
    if (ptr->left == nullptr) return ptr;
    return findMin(ptr->left);
}

template <class Key, class Value>
bool AVLSearchTree<Key, Value>::isEmpty() const {
    return root == nullptr;
}

template<class Key, class Value>
void AVLSearchTree<Key, Value>::rotateWithLeftChild(Value& k2) {
    Value k1 = k2->left;
    k2->left = k1->right;
    k1->right = k2;
    k2->height = max(getHeight(k2->left), getHeight(k2->right)) + 1;
    k1->height = max(getHeight(k1->left), getHeight(k2)) + 1;
    k2 = k1;
}

template<class Key, class Value>
void AVLSearchTree<Key, Value>::rotateWithRightChild(Value& k1) {
    Value k2 = k1->right;
    k1->right = k2->left;
    k2->left = k1;
    k1->height = max(getHeight(k1->left), getHeight(k1->right)) + 1;
    k2->height = max(getHeight(k2->right), getHeight(k1)) + 1;
    k1 = k2;
}

template<class Key, class Value>
void AVLSearchTree<Key, Value>::doubleWithLeftChild(Value& k3) {
    rotateWithRightChild(k3->left);
    rotateWithLeftChild(k3);
}

template<class Key, class Value>
void AVLSearchTree<Key, Value>::doubleWithRightChild(Value& k1) {
    rotateWithLeftChild(k1->right);
    rotateWithRightChild(k1);
}

struct HashNode {
    string word;
    vector<DocumentItem> details;
};

bool isPrime(int n) {
    if (n == 2 || n == 3)
        return true;
    if (n == 1 || n % 2 == 0)
        return false;
    for (int i = 3; i * i <= n; i += 2)
        if (n % i == 0)
            return false;
    return true;
}

int nextPrime(int n) {
    if (n % 2 == 0)
        n++;
    while (!isPrime(n)) {
        n += 2;
    }
    return n;
}

int hash_function(const string& key, int tableSize) {
    int hashVal = 0;
    for (char ch : key)
        hashVal = 37 * hashVal + ch;
    hashVal %= tableSize;
    if (hashVal < 0)
        hashVal += tableSize;
    return hashVal;
}

template<class HashedObj, class Key>
class HashTable {
public:
    enum EntryType { ACTIVE, EMPTY, DELETED };
    HashTable() {
        uniqueWordCount = 0;
        array_hash.resize(53);
    }

    const HashedObj find(const Key& x) const {
        int currentPos = findPos(x);
        if (isActive(currentPos))
            return array_hash[currentPos].element;
        return nullptr;
    }

    void insert(const Key& x, const string& filename) {
        int currentPos = findPos(x);
        if (!isActive(currentPos)) {
            HashNode* new_save = new HashNode();
            new_save->word = x;
            DocumentItem new_save_first_occurrence = { filename, 1 };
            new_save->details.push_back(new_save_first_occurrence);
            array_hash[currentPos].element = new_save;
            array_hash[currentPos].info = ACTIVE;
            uniqueWordCount++;
        }
        else {
            HashNode* existingNode = array_hash[currentPos].element;
            bool found = false;
            for (auto& detail : existingNode->details) {
                if (detail.documentName == filename) {
                    detail.count++;
                    found = true;
                    break;
                }
            }
            if (!found) {
                existingNode->details.push_back({ filename, 1 });
            }
        }
        if (loadFactor() > 0.75) rehash();
    }

    void remove(const Key& x) {
        int currentPos = findPos(x);
        if (isActive(currentPos)) {
            delete array_hash[currentPos].element;
            array_hash[currentPos].element = nullptr;
            array_hash[currentPos].info = DELETED;
        }
    }

    float loadFactor() const {
        return static_cast<float>(uniqueWordCount) / array_hash.size();
    }

    int getUniqueWordCount() const {
        return uniqueWordCount;
    }

private:
    struct HashEntry {
        HashedObj element;
        EntryType info;

        HashEntry(EntryType i = EMPTY) : info(i), element(nullptr) {}
    };

    vector<HashEntry> array_hash;
    int uniqueWordCount;

    bool isActive(int currentPos) const {
        return array_hash[currentPos].info == ACTIVE;
    }

    int findPos(const Key& x) const {
        int collisionNum = 0;
        int currentPos = hash_function(x, array_hash.size());
        while (array_hash[currentPos].info != EMPTY &&
            array_hash[currentPos].element->word != x) {
            currentPos += ++collisionNum * collisionNum;
            currentPos %= array_hash.size();
        }
        return currentPos;
    }

    void rehash() {
        vector<HashEntry> oldArray = array_hash;
        array_hash.resize(nextPrime(2 * oldArray.size()));
        for (auto& entry : array_hash) {
            entry.info = EMPTY;
            entry.element = nullptr;
        }
        uniqueWordCount = 0;
        for (auto& entry : oldArray) {
            if (entry.info == ACTIVE) {
                for (auto& detail : entry.element->details) {
                    for (int i = 0; i < detail.count; i++) insert(entry.element->word, detail.documentName);
                }
                delete entry.element;
            }
        }
        cout << "rehashed...\n";
        cout << "previous table size: " << oldArray.size() << ", new table size: " << array_hash.size()
            << ", current unique word count " << uniqueWordCount << ", current load factor: " << loadFactor() << "\n";
    }
};

void processWord(string& word, AVLSearchTree<string, WordItem*>& myTree, HashTable<HashNode*, string>& hash_table, const string& filename) {
    string word_lower = tolower_string(word);
    if (all_of(word_lower.begin(), word_lower.end(), ::isalpha)) {
        WordItem* foundWord = myTree.find(word_lower);
        if (!foundWord) {
            myTree.insert(word_lower);
            foundWord = myTree.find(word_lower);
            foundWord->details.push_back({ filename, 1 });
        }
        else {
            auto it = find_if(foundWord->details.begin(), foundWord->details.end(),
                [&filename](const DocumentItem& item) {
                    return item.documentName == filename;
                });
            if (it != foundWord->details.end()) {
                it->count++;
            }
            else {
                foundWord->details.push_back({ filename, 1 });
            }
        }
        hash_table.insert(word_lower, filename);
    }
}

void processFile(const string& filename, AVLSearchTree<string, WordItem*>& myTree, HashTable<HashNode*, string>& hash_table) {
    ifstream file(filename);
    stringstream buffer;
    buffer << file.rdbuf();
    string content = buffer.str();

    // Replace non-alphabetical characters and blanks with newlines
    for (char& c : content) {
        if (!isalpha(c)) {
            c = '\n';
        }
    }

    // Write the modified content back to the file
    ofstream outfile(filename);
    outfile << content;
    outfile.close();

    // Now process the file as usual
    string word;
    stringstream contentStream(content);
    while (contentStream >> word) {
        processWord(word, myTree, hash_table, filename);
    }
}

vector<string> splitWords(const string& text) {
    vector<string> words;
    string word;
    for (char c : text) {
        if (isalpha(c)) {
            word += c;
        }
        else {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
            words.push_back("\n");
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }
    return words;
}

int main() {
    AVLSearchTree<string, WordItem*> myTree;
    HashTable<HashNode*, string> hash_table;
    int fileNum;
    cout << "Enter number of input files: ";
    cin >> fileNum;
    vector<string> filenames(fileNum);
    for (int i = 0; i < fileNum; ++i) {
        cout << "Enter " << i + 1 << ". file name: ";
        cin >> filenames[i];
    }

    for (const auto& filename : filenames) {
        processFile(filename, myTree, hash_table);
    }

    cout << "After preprocessing, the unique word count is " << hash_table.getUniqueWordCount() << ". Current load ratio is " << hash_table.loadFactor() << "\n";

    string search;
    cout << "Enter queried words in one line: ";
    cin.ignore();
    getline(cin, search);

    // Convert the entire input string to lowercase
    search = tolower_string(search);

    vector<string> queryWords = splitWords(search);

    // Store results in maps for grouped output
    map<string, map<string, int>> bstResults;
    map<string, map<string, int>> hashTableResults;

    bool allWordsFoundInBST = true;
    bool allWordsFoundInHashTable = true;

    // Collect results for AVL Tree
    for (const auto& query : queryWords) {
        if (query != "\n") {
            WordItem* foundWord = myTree.find(query);
            if (foundWord) {
                for (const auto& detail : foundWord->details) {
                    bstResults[detail.documentName][query] += detail.count;
                }
            }
            else {
                allWordsFoundInBST = false;
            }
        }
    }

    // Collect results for Hash Table
    for (const auto& query : queryWords) {
        if (query != "\n") {
            const HashNode* foundNode = hash_table.find(query);
            if (foundNode) {
                for (const auto& detail : foundNode->details) {
                    hashTableResults[detail.documentName][query] += detail.count;
                }
            }
            else {
                allWordsFoundInHashTable = false;
            }
        }
    }

    // Print the results for AVL Tree
    if (!allWordsFoundInBST) {
        cout << "No document contains the given query\n";
    }
    else {
        for (const auto& doc : bstResults) {
            cout << "in Document " << doc.first << ", ";
            bool first = true;
            for (const auto& word : doc.second) {
                if (!first) cout << ", ";
                cout << word.first << " found " << word.second << " times";
                first = false;
            }
            cout << ".\n";
        }
    }

    // Print the results for Hash Table
    if (!allWordsFoundInHashTable) {
        cout << "No document contains the given query\n";
    }
    else {
        for (const auto& doc : hashTableResults) {
            cout << "in Document " << doc.first << ", ";
            bool first = true;
            for (const auto& word : doc.second) {
                if (!first) cout << ", ";
                cout << word.first << " found " << word.second << " times";
                first = false;
            }
            cout << ".\n";
        }
    }

    int k = 20;
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < k; ++i) {
        for (const auto& query : queryWords) {
            myTree.find(query);
        }
    }
    auto BSTTime = chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start);

    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < k; ++i) {
        for (const auto& query : queryWords) {
            hash_table.find(query);
        }
    }
    auto HTTime = chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - start);

    cout << "Time: " << BSTTime.count() / k << " ns\n";
    cout << "Time: " << HTTime.count() / k << " ns\n";
    cout << "Speed Up: " << static_cast<float>(BSTTime.count()) / HTTime.count() << "\n";

    return 0;
}
