#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

struct DocumentItem {
	string documentName;
	int count;
};

struct WordItem {  //node structure
	string word;
	WordItem* left = nullptr;
	WordItem* right = nullptr;
	vector <DocumentItem> details = {};
	int height;

	WordItem(const string& word, WordItem* left = nullptr, WordItem* right = nullptr, int height = 0) :
		word(word), left(left), right(right), height(height), details({}) {}


};

string tolower_string(string s);

string tolower_string(string s) {  //embedded tolower function is used for chars so I defined a function to obtain lowercase strings
	string last_string = "";
	for (int i = 0; i < s.length(); i++) {
		if ((s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z')) last_string += tolower(s[i]);
		else {  //there are invalid characters (non alphabetical)
			last_string = "";
			break;
		}
	}
	return last_string;
}


template <class Key, class Value>
class AVLSearchTree
{
public:
	AVLSearchTree();   //mostly taken from slides
	~AVLSearchTree();
	void insert(Key key) const;
	void remove(Key key) const;
	Value find(Key key) const;
	Value findMin() const;
	void makeEmpty();
	bool isEmpty();
	int getBalance(Value& ptr) const;

	void rotateWithLeftChild(Value& k2) const;
	void rotateWithRightChild(Value& k1) const;
	void doubleWithLeftChild(Value& k3) const;
	void doubleWithRightChild(Value& k1) const;


private:  //used for default arguments
	Value root = nullptr;  
	void insert(Key key, Value& ptr) const;
	void remove(Key key, Value& ptr) const;
	void makeEmpty(Value& ptr) const;
	Value find(Key key, Value ptr) const;  
	Value findMin(Value ptr) const;
	int getHeight(Value ptr) const;
};

template <class Key, class Value>
AVLSearchTree<Key, Value>::AVLSearchTree() {   //constructor
	root = nullptr;
}

template<class Key, class Value>
AVLSearchTree<Key, Value>::~AVLSearchTree() {  //destructor
	makeEmpty();
}

template<class Key, class Value>
int AVLSearchTree<Key, Value>::getHeight(Value ptr) const   //it is used to handle nullptr->height case
{
	if (ptr == nullptr) return 0;
	else return ptr->height;
}


template<class Key, class Value>
void AVLSearchTree<Key, Value>::remove(Key key) const {  //root ptr is given default
	remove(key, (Value&)root);
}




template<class Key, class Value>
void AVLSearchTree<Key, Value>::makeEmpty() {   //root ptr is given default
	makeEmpty((Value&)root);
}

template<class Key, class Value>
void AVLSearchTree<Key, Value>::makeEmpty(Value& ptr) const {
	if (ptr != nullptr) {
		makeEmpty(ptr->left);  //recursively deleting the nodes
		makeEmpty(ptr->right);
		delete ptr;
	}
	ptr = nullptr;
}


template<class Key, class Value>
void AVLSearchTree<Key, Value>::insert(Key key) const {   //root ptr is given default
	insert(key, (Value&)root);
}

template<class Key, class Value>
void AVLSearchTree<Key, Value>::insert(Key key, Value& ptr) const {
	if (ptr == nullptr) {  //first node of tree
		ptr = new WordItem(key, nullptr, nullptr, 0);
	}

	else if (key < ptr->word) {  //nodes are ordered as it is an AVL
		insert(key, ptr->left);

		if (getHeight(ptr->left) - getHeight(ptr->right) == 2) {  //balancing the tree
			if (key < ptr->left->word) rotateWithLeftChild(ptr);
			else doubleWithLeftChild(ptr);
		}
	}
	
	//same comments apply for this case too 

	else if (ptr->word < key) {
		insert(key, ptr->right); 
		if (getHeight(ptr->right) - getHeight(ptr->left) == 2) {
			if (key < ptr->right->word) rotateWithRightChild(ptr);
			else doubleWithRightChild(ptr);
		}
	}


	//new height values
	if (ptr->left == nullptr && ptr->right == nullptr) ptr->height = 0;
	else if (ptr->right == nullptr && ptr->left != nullptr) ptr->height = ptr->left->height + 1;
	else if (ptr->left == nullptr && ptr->right != nullptr) ptr->height = ptr->right->height + 1;
	else ptr->height = max(ptr->left->height, ptr->right->height) + 1;
}

template<class Key, class Value>
void AVLSearchTree<Key, Value>::remove(Key key, Value& ptr) const {
	if (ptr == nullptr) return;  //nothing to remove


	if (key < ptr->word) {   //nodes are ordered from left to right alphabetically as it is an AVL
		remove(key, ptr->left);
		if (getBalance(ptr) > 1) {  //balancing the tree
			if (key < ptr->left->word) {
				rotateWithLeftChild(ptr);
			}
			else {
				doubleWithLeftChild(ptr);
			}
		}
	}

	//same comments for this case
	else if (key > ptr->word) {
		remove(key, ptr->right);
		if (getBalance(ptr) > 1) {
			if (key > ptr->right->word) {
				rotateWithRightChild(ptr);
			}
			else {
				doubleWithRightChild(ptr);
			}
		}
	}
	else { //found the node with the word to remove
		if (ptr->left == nullptr) {
			Value temp = ptr;  //don't delete the ptr but you can delete temp
			ptr = ptr->right;
			delete temp;
		}
		else if (ptr->right == nullptr) {
			Value temp = ptr;  //same comment
			ptr = ptr->left;
			delete temp;
		}
		else {  //two children for particular node
			Value temp = findMin(ptr->right);
			if (temp != nullptr) {
				ptr->word = temp->word;
				remove(temp->word, ptr->right);
			}
		}

	}


	//setting heights accurately
	ptr->height = max(getHeight(ptr->left), getHeight(ptr->right)) + 1;
	
}


template<class Key, class Value>
int AVLSearchTree<Key, Value>::getBalance(Value& ptr) const {
	if (ptr == nullptr) return 0;   //handling ptr = nullptr case
	return getHeight(ptr->left) - getHeight(ptr->right);
}



template<class Key, class Value>
Value AVLSearchTree<Key, Value>::find(Key key) const
{
	return find(key, root);
}



template<class Key, class Value>
Value AVLSearchTree<Key, Value>::find(Key key, Value ptr) const
{  //recursively finds the word
	key = tolower_string(key); 
	if (ptr == nullptr) return nullptr;  
	else if (key < ptr->word) return find(key, ptr->left);
	else if (key > ptr->word) return find(key, ptr->right);
	else if (key == ptr->word) return ptr;  //found the word
}

template<class Key, class Value>
Value AVLSearchTree<Key, Value>::findMin() const
{
	return findMin((Value&)root);
}

template<class Key, class Value>
Value AVLSearchTree<Key, Value>::findMin(Value ptr) const
{  //recursion again, but checks left only as order is from left to right
	if (ptr == nullptr) return nullptr;
	else if (ptr->left == nullptr) return ptr;
	else return findMin(ptr->left);
}



template <class Key, class Value>
bool AVLSearchTree<Key, Value>::isEmpty() {
	return (root == nullptr); //if root is null, then there is no node
}


//rotation and doubling graphs in slides are used in doubling and rotating

template<class Key, class Value>
void AVLSearchTree<Key, Value>::rotateWithLeftChild(Value& k2) const {
	if (k2 == nullptr || k2->left == nullptr) {
		// Cannot perform rotation, k2 is nullptr or k2->left is nullptr
		return;
	}

	
	Value k1 = k2->left;
	k2->left = k1->right;
	k1->right = k2;
	k2->height = max(getHeight(k2->left), getHeight(k2->right)) + 1;
	k1->height = max(getHeight(k1->left), getHeight(k2)) + 1;
	k2 = k1;
}

template<class Key, class Value>
void AVLSearchTree<Key, Value>::rotateWithRightChild(Value& k1) const {
	if (k1 == nullptr || k1->right == nullptr) {
		// Cannot perform rotation, k1 is nullptr or k1->right is nullptr
		return;
	}

	Value k2 = k1->right;
	k1->right = k2->left;
	k2->left = k1;
	k1->height = max(getHeight(k1->left), getHeight(k1->right)) + 1;
	k2->height = max(getHeight(k2->right), getHeight(k1)) + 1;
	k1 = k2;
}

template<class Key, class Value>
void AVLSearchTree<Key, Value>::doubleWithRightChild(Value& k1) const {
	if (k1 == nullptr || k1->left == nullptr) {
		// Cannot perform double rotation, k1 is nullptr or k1->left is nullptr
		return;
	}

	rotateWithRightChild(k1->left);
	rotateWithLeftChild(k1);
}

template<class Key, class Value>
void AVLSearchTree<Key, Value>::doubleWithLeftChild(Value& k1) const {
	if (k1 == nullptr || k1->left == nullptr) {
		// Cannot perform double rotation, k1 is nullptr or k1->left is nullptr
		return;
	}

	rotateWithLeftChild(k1->left);
	rotateWithRightChild(k1);
}





int main()
{
	AVLSearchTree<string, WordItem*> myTree;
	int fileNum;
	cout << "Enter number of input files: ";
	cin >> fileNum;
	vector <string> filenames;
	for (int i = 0; i < fileNum; i++) {
		string new_filename;
		cout << "Enter " << i + 1 << ". file name:";  //collecting filenames into the vector filenames
		cin >> new_filename;
		filenames.push_back(new_filename);
	}

	for (int i = 0; i < filenames.size(); i++) {
		ifstream file(filenames[i]);
		if (!file.is_open()) {  //wrong filename or missing file
			cout << filenames[i] << " could not be opened!\n";
		}

		else {
			string word, word_lower;
			while (file >> word) { //starting to build the tree
				word_lower = tolower_string(word);  //tolower again
				if (word_lower.length() > 0 && myTree.find(word_lower) == nullptr) {  //word is new to tree (non alphabeticals are already eliminated)
					myTree.insert(word_lower);
					DocumentItem new_save; //documentitem is used to track the count in each document
					new_save.documentName = filenames[i];
					new_save.count = 1;  
					myTree.find(word_lower)->details.push_back(new_save);  //finally the documentitem is stored in worditem's details vector
				}

				else if (word_lower.length() > 0 && myTree.find(word_lower) != nullptr) {  //word is not new
					bool new_filename_save = true;  //but filename might be new
					for (int y = 0; y < myTree.find(word_lower)->details.size(); y++) {
						if (myTree.find(word_lower)->details[y].documentName == filenames[i]) {  //document with name filename is already checked, so the count should be incremented
							new_filename_save = false;  //set from true to false
							myTree.find(word_lower)->details[y].count += 1;
							break;
						}
					}
					if (new_filename_save) {  //new filename should be used to hold track
						DocumentItem new_filename_item;
						new_filename_item.documentName = filenames[i];
						new_filename_item.count = 1;
						myTree.find(word_lower)->details.push_back(new_filename_item);
					}
				}
			}
		}
	}
	cin.ignore();  //if this is not used and not used here, then the query input is taken wrongly (character or the whole input might be lost)
	while (true) {
		string search;
		cout << "Enter queried words in one line: ";
		getline(cin, search);

		if (tolower_string(search.substr(0, 10)) == "endofinput") return 0;  //end of input

		else if (tolower_string(search.substr(0, 6)) == "remove") {
			myTree.remove(tolower_string(search.substr(7)));  //characters before index 7 are "remove" and the rest of string is to be removed
			cout << search.substr(7) << " has been REMOVED\n";
		}

		else {
			istringstream iss(search);
			string substring;
			vector<WordItem*> found_nodes = {};
			bool all_words_found = true;
			while (iss >> substring) {  //reading the search input word by word
				if (myTree.find(tolower_string(substring)) == nullptr) {  //all words in query are not found, so given query is not found
					all_words_found = false;
					cout << "No document contains the given query\n";
					break;
				}
				else {
					found_nodes.push_back(myTree.find(substring));  //nodes of found words are pushed into found_nodes vector
				}
			}

			if (all_words_found == true) {  //given query is found

				//3D for loop is for this: For each filename, every found word should be printed with their counts.
				for (int i = 0; i < filenames.size(); i++) {  //for each filename
					bool document_printed = false; //this is just for formatting the commas and dots
					for (int y = 0; y < found_nodes.size(); y++) {   //every found word
						for (int z = 0; z < found_nodes[y]->details.size(); z++) {    //should be printed with their counts
							if (filenames[i] == found_nodes[y]->details[z].documentName) {
								if (document_printed) {
									cout << ", ";
								}
								else {
									cout << "in Document " << filenames[i] << ", ";  //initialization of sentence
									document_printed = true;
								}
								cout << found_nodes[y]->word << " found " << found_nodes[y]->details[z].count << " times";
							}
						}
					}
					if (document_printed) { //end for a particular filename
						cout << ".\n";
					}
				}
			}
		}
		cout << "\n";
	}

}