#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <iterator>
#include <cctype>

using namespace std;
using namespace std::chrono;

struct Contact {
    string name;
    string surname;
    string telephone;
    string city;

    string fullName() const {
        return name + " " + surname;
    }

    string upperFullName() const {
        string full = fullName();
        transform(full.begin(), full.end(), full.begin(), ::toupper);
        return full;
    }
};

template<class T>
class ContactManager {
public:
    ContactManager(vector<T>& contacts) : contacts(contacts) {}

    void insertionSort() {
        for (size_t i = 1; i < contacts.size(); ++i) {
            T key = contacts[i];
            int j = i - 1;
            while (j >= 0 && contacts[j].upperFullName() > key.upperFullName()) {
                contacts[j + 1] = contacts[j];
                --j;
            }
            contacts[j + 1] = key;
        }
    }

    void quickSort(int low, int high) {
        if (low < high) {
            int pi = partition(low, high);
            quickSort(low, pi - 1);
            quickSort(pi + 1, high);
        }
    }

    void mergeSort(int left, int right) {
        if (left < right) {
            int mid = left + (right - left) / 2;
            mergeSort(left, mid);
            mergeSort(mid + 1, right);
            merge(left, mid, right);
        }
    }

    void heapSort() {
        int n = contacts.size();
        for (int i = n / 2 - 1; i >= 0; i--) {
            percDown(n, i);
        }
        for (int i = n - 1; i > 0; i--) {
            swap(contacts[0], contacts[i]);
            percDown(i, 0);
        }
    }

    bool sequentialSearch(const vector<string>& queries, vector<T>& results) {
        bool found = false;
        for (const auto& contact : contacts) {
            if (allKeywordsMatch(contact, queries)) {
                results.push_back(contact);
                found = true;
            }
        }
        return found;
    }

    bool binarySearch(const vector<string>& queries, vector<T>& results) {
        int left = 0;
        int right = contacts.size() - 1;
        bool found = false;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (allKeywordsMatch(contacts[mid], queries)) {
                results.push_back(contacts[mid]);
                found = true;
                // Check neighbors
                int l = mid - 1;
                while (l >= left && allKeywordsMatch(contacts[l], queries)) {
                    results.push_back(contacts[l]);
                    l--;
                }
                int r = mid + 1;
                while (r <= right && allKeywordsMatch(contacts[r], queries)) {
                    results.push_back(contacts[r]);
                    r++;
                }
                break;
            }
            if (contacts[mid].upperFullName() < joinKeywords(queries)) {
                left = mid + 1;
            }
            else {
                right = mid - 1;
            }
        }
        sort(results.begin(), results.end(), [](const T& a, const T& b) {
            return a.upperFullName() < b.upperFullName();
            });
        return found;
    }

    void printContacts(const vector<T>& results) const {
        for (const auto& contact : results) {
            cout << contact.upperFullName() << " " << contact.telephone << " " << contact.city << endl;
        }
    }

private:
    vector<T>& contacts;

    bool allKeywordsMatch(const T& contact, const vector<string>& keywords) {
        string upperFullName = contact.upperFullName();
        return all_of(keywords.begin(), keywords.end(), [&upperFullName](const string& keyword) {
            return upperFullName.find(keyword) != string::npos;
            });
    }

    string joinKeywords(const vector<string>& keywords) {
        string result;
        for (const auto& keyword : keywords) {
            result += keyword + " ";
        }
        if (!result.empty()) {
            result.pop_back(); // Remove the trailing space
        }
        return result;
    }

    int partition(int low, int high) {
        // Find the median of low, mid, high and use it as pivot
        int mid = low + (high - low) / 2;
        string pivot = median(contacts[low].upperFullName(), contacts[mid].upperFullName(), contacts[high].upperFullName());

        // Move the pivot to the end for partitioning
        if (pivot == contacts[mid].upperFullName()) {
            swap(contacts[mid], contacts[high]);
        }
        else if (pivot == contacts[low].upperFullName()) {
            swap(contacts[low], contacts[high]);
        }

        pivot = contacts[high].upperFullName();
        int i = (low - 1);
        for (int j = low; j <= high - 1; j++) {
            if (contacts[j].upperFullName() <= pivot) {
                i++;
                swap(contacts[i], contacts[j]);
            }
        }
        swap(contacts[i + 1], contacts[high]);
        return (i + 1);
    }

    string median(const string& a, const string& b, const string& c) {
        if ((a > b) ^ (a > c))
            return a;
        else if ((b < a) ^ (b < c))
            return b;
        else
            return c;
    }

    void merge(int left, int mid, int right) {
        int i = left;
        int j = mid + 1;

        while (i <= mid && j <= right) {
            if (contacts[i].upperFullName() <= contacts[j].upperFullName()) {
                i++;
            }
            else {
                T temp = contacts[j];
                for (int k = j; k > i; k--) {
                    contacts[k] = contacts[k - 1];
                }
                contacts[i] = temp;
                i++;
                mid++;
                j++;
            }
        }
    }

    void percDown(int n, int i) {
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if (left < n && contacts[left].upperFullName() > contacts[largest].upperFullName()) {
            largest = left;
        }

        if (right < n && contacts[right].upperFullName() > contacts[largest].upperFullName()) {
            largest = right;
        }

        if (largest != i) {
            swap(contacts[i], contacts[largest]);
            percDown(n, largest);
        }
    }
};

void toUpperCase(string& str) {
    transform(str.begin(), str.end(), str.begin(), ::toupper);
}

vector<string> splitKeywords(const string& query) {
    istringstream iss(query);
    return vector<string>{istream_iterator<string>{iss}, istream_iterator<string>{}};
}

int main() {
    ifstream inputFile;
    string fileName;
    string query;

    cout << "Please enter the contact file name: ";
    cin >> fileName;
    inputFile.open(fileName);

    if (!inputFile) {
        cerr << "Unable to open file " << fileName << endl;
        return 1;
    }

    vector<Contact> contacts;
    string line;
    while (getline(inputFile, line)) {
        stringstream ss(line);
        Contact contact;
        ss >> contact.name >> contact.surname >> contact.telephone >> contact.city;
        contacts.push_back(contact);
    }
    inputFile.close();

    cout << "Please enter the word to be queried: ";
    cin.ignore();
    getline(cin, query);

    vector<string> upperKeywords = splitKeywords(query);
    for (auto& keyword : upperKeywords) {
        toUpperCase(keyword);
    }

    ContactManager<Contact> manager(contacts);

    // Create copies of the contact list for each sorting algorithm
    vector<Contact> contactsInsertion = contacts;
    vector<Contact> contactsQuick = contacts;
    vector<Contact> contactsMerge = contacts;
    vector<Contact> contactsHeap = contacts;

    ContactManager<Contact> managerInsertion(contactsInsertion);
    ContactManager<Contact> managerQuick(contactsQuick);
    ContactManager<Contact> managerMerge(contactsMerge);
    ContactManager<Contact> managerHeap(contactsHeap);

    // Measure sorting times
    auto start = high_resolution_clock::now();
    managerInsertion.insertionSort();
    auto end = high_resolution_clock::now();
    auto insertionSortTime = duration_cast<nanoseconds>(end - start).count();

    start = high_resolution_clock::now();
    managerQuick.quickSort(0, contactsQuick.size() - 1);
    end = high_resolution_clock::now();
    auto quickSortTime = duration_cast<nanoseconds>(end - start).count();

    start = high_resolution_clock::now();
    managerMerge.mergeSort(0, contactsMerge.size() - 1);
    end = high_resolution_clock::now();
    auto mergeSortTime = duration_cast<nanoseconds>(end - start).count();

    start = high_resolution_clock::now();
    managerHeap.heapSort();
    end = high_resolution_clock::now();
    auto heapSortTime = duration_cast<nanoseconds>(end - start).count();

    // Print sorting times
    cout << endl;
    cout << "Sorting the vector copies" << endl;
    cout << "======================================" << endl;
    cout << "Quick Sort Time: " << quickSortTime << " Nanoseconds" << endl;
    cout << "Insertion Sort Time: " << insertionSortTime << " Nanoseconds" << endl;
    cout << "Merge Sort Time: " << mergeSortTime << " Nanoseconds" << endl;
    cout << "Heap Sort Time: " << heapSortTime << " Nanoseconds" << endl;
    cout << endl;

    // Measure search times and perform searches multiple times for accuracy
    vector<Contact> results;
    int N = 100; // Number of repetitions

    start = high_resolution_clock::now();
    for (int i = 0; i < N; i++) {
        results.clear();
        managerQuick.binarySearch(upperKeywords, results);
    }
    end = high_resolution_clock::now();
    auto binarySearchTime = duration_cast<nanoseconds>(end - start).count() / N;

    cout << "Searching for " << query << endl;
    cout << "======================================" << endl;
    if (!results.empty()) {
        managerQuick.printContacts(results);
    }
    else {
        cout << query << " does NOT exist in the dataset" << endl;
    }
    cout << "Binary Search Time: " << binarySearchTime << " Nanoseconds" << endl;

    results.clear();
    start = high_resolution_clock::now();
    for (int i = 0; i < N; i++) {
        results.clear();
        managerQuick.sequentialSearch(upperKeywords, results);
    }
    end = high_resolution_clock::now();
    auto sequentialSearchTime = duration_cast<nanoseconds>(end - start).count() / N;

    cout << endl;
    cout << "Search results for Sequential Search:" << endl;
    if (!results.empty()) {
        managerQuick.printContacts(results);
    }
    else {
        cout << query << " does NOT exist in the dataset" << endl << endl;
    }
    cout << "Sequential Search Time: " << sequentialSearchTime << " Nanoseconds" << endl;

    // Calculate and print speedups
    cout << endl;
    cout << "SpeedUp between Search Algorithms" << endl;
    cout << "======================================" << endl;
    cout << "(Sequential Search/ Binary Search) SpeedUp = " << static_cast<double>(sequentialSearchTime) / binarySearchTime << endl;

    cout << endl;
    cout << "SpeedUps between Sorting Algorithms" << endl;
    cout << "======================================" << endl;
    cout << "(Insertion Sort/ Quick Sort) SpeedUp = " << static_cast<double>(insertionSortTime) / quickSortTime << endl;
    cout << "(Merge Sort / Quick Sort) SpeedUp = " << static_cast<double>(mergeSortTime) / quickSortTime << endl;
    cout << "(Heap Sort / Quick Sort) SpeedUp = " << static_cast<double>(heapSortTime) / quickSortTime << endl;

    return 0;
}
