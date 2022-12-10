#include <iostream>
#include <sstream>
#include <vector>
using namespace std;
#define bucketSize 99991

struct node {
    string key;
    int value;
    // int element_count=0;
  };

long long compute_hash(string const &s) {
  const int p = 31;
  const int m = 1e9 + 9;
  long long hash_value = 0;
  long long p_pow = 1;
  for (char c : s) {
    hash_value = (hash_value + (c - 'a' + 1) * p_pow) % m;
    p_pow = (p_pow * p) % m;
  }
  return hash_value;
}

template <typename T1, typename T2> class unordered_mapp {
public:
  /* struct node {
    string key;
    T2 value;
    // int element_count=0;
  }; */

public:
  vector<node> bucket[bucketSize];
  int total_elements = 0;

  long long hash(string k) {
    
    long long retHashVal = compute_hash(k);
    long long arrIndex = retHashVal % bucketSize;
    if(arrIndex<0){
      arrIndex+=bucketSize;
    }
    cout << "arrIndex: " << arrIndex << endl;
    return arrIndex;
  }

  void insert(T1 k, T2 v) {
    // apply the insert logic here
    string newKey;
    stringstream ss;
    ss << k;
    ss >> newKey;

    long long val = hash(newKey);

    // now first search whether the key is already present in the map
    for (auto it = bucket[val].begin();it!=bucket[val].end();it++) {
      if (it->key ==
          newKey) {    // newKey is already present in the map so update it
        it->value += 1; // updated with the new value
        cout << "it.value: " << it->value << endl;
        return;
      }
    }

    // now if the key is not present in the vector insert at the end of the
    // vector
    node obj;
    obj.key = newKey;
    obj.value = 1;
    cout << "first time it.value: " << obj.value << endl;
    bucket[val].push_back(obj);
    total_elements++;
    cout << "total_elements: " << total_elements << endl;
  }

  void remove(T1 k) {
    
    string newKey;
    stringstream ss;
    ss << k;
    ss >> newKey;

    long long val = hash(newKey);

    // now first search whether the key is already present in the map
    for (auto it = bucket[val].begin(); it != bucket[val].end(); it++) {
      if (it->key ==
          newKey) { // newKey is already present in the map so delete it
          cout << " Before if in remove: it.value : " << it->value << endl;
        if (it->value == 1) {

          bucket[val].erase(it);
          total_elements--; // updated with the new value
          return;
        } else {
          it->value--;
          return;
        }
      }
    }
    return;
  }

  bool find(T1 k) {

    // first I will convert the key to string using stringstream
    string newKey;
    stringstream ss;
    ss << k;
    ss >> newKey;

    long long val = hash(newKey);
    for (auto it : bucket[val]) {

      if (it.key == newKey) {
        return true;
      }
    }
    return false;
  }

  T2 operator[](T1 k) {
    // apply the map logic here
    string newKey;
    stringstream ss;
    ss << k;
    ss >> newKey;

    long long val = hash(newKey);

    // now first search whether the key is already present in the map
    for (auto it : bucket[val]) {
      if (it.key ==
          newKey) {      // newKey is already present in the map so update it
        return it.value; // updated with the new value
      }
    }

    // return the default value
    return T2();
  }
};

int main() {

  int n, k;
  cin >> n >> k;
  int A[n];

  for (int i = 0; i < n; i++) {
    cin >> A[i];
  }

  vector<int> v;
  int i;
  unordered_mapp<int, int> mpp;

  // first we will store the 1st window into the map
  for (int i = 0; i < k; i++) {
    mpp.insert(A[i], A[i]);
  }

  // store the size of the 1st window into the vector
  v.push_back(mpp.total_elements);

  // now 2nd window onwards we will
  // first decrement the freqency of the previous elements by 1
  for (int i = k; i < n; i++) {

    mpp.remove(A[i - k]);
    mpp.insert(A[i], A[i]);
    v.push_back(mpp.total_elements);
    
  }

  for(auto it:v){
    cout << it << " ";
  }
  return 0;
}
