#include <iostream>
#include <sstream>
#include <vector>
using namespace std;
#define bucketSize 99991

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
  struct node {
    string key;
    T2 value;
  };

public:
  vector<node> bucket[bucketSize];
  int total_elements = 0;

  long long hash(string k) {
    // returns the index to be hashed into the array
    // write some hashFunction on string which will return the hash value in int
    // & then return the hashValue%arraySize
    long long retHashVal = compute_hash(k);
    long long arrIndex = retHashVal % bucketSize;
    if(arrIndex<0){
      arrIndex+=bucketSize;
    }
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
    for (auto it = bucket[val].begin(); it != bucket[val].end(); it++) {
      if (it->key ==
          newKey) {   // newKey is already present in the map so update it
        it->value = v; // updated with the new value
        return;
      }
    }

    // now if the key is not present in the vector insert at the end of the
    // vector
    node obj;
    obj.key = newKey;
    obj.value = v;
    bucket[val].push_back(obj);
    total_elements++;
  }

  void remove(T1 k) {
    // apply the erase logic here
    // apply the map logic here
    string newKey;

    stringstream ss;
    ss << k;
    ss >> newKey;

    long long val = hash(newKey);

    // now first search whether the key is already present in the map
    for (auto it = bucket[val].begin(); it != bucket[val].end(); it++) {
      if (it->key ==
          newKey) { // newKey is already present in the map so delete it
        bucket[val].erase(it);
        total_elements--; // updated with the new value
        return;
      }
    }
    return;
  }

  bool find(T1 k) {
    // apply the find logic here
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

  int q;
  cin >> q;
  unordered_mapp<string, string> map;
  while (q) {

    int opType;
    cin >> opType;

    string key;
    string value;

    switch (opType) {
    case 1: {

      cin >> key >> value;
      map.insert(key, value);

    }

    break;
    case 2: {
      cin >> key;
      map.remove(key);
    }

    break;
    case 3: {
      cin >> key;
      bool ans = map.find(key);
      if (ans == true) {
        cout << 1 << endl;
      } else {
        cout << 0 << endl;
      }
    }

    break;
    case 4: {
      cin >> key;
      auto ans = map[key];

      cout << ans << endl;
    }

    break;

    default:
      cout << "Invalid user input!\n";
      break;
    }

    q--;
  }

  return 0;
}