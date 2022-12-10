#include <iostream>
#include <unordered_map>

using namespace std;

class doublyLinkedListNode {
public:
  int key;
  doublyLinkedListNode *next, *prev;
  doublyLinkedListNode(int k) {
    key = k;
    next = NULL;
    prev = NULL;
  }
};

class DLL {

public:
  doublyLinkedListNode *head, *tail;

  DLL() {

    head = new doublyLinkedListNode(0);
    tail = new doublyLinkedListNode(0);
    head->next = tail;
    tail->prev = head;
  }

  void
  insertNode(int key) { // we will always insert node right after the head node
    doublyLinkedListNode *newNode = new doublyLinkedListNode(key);
    newNode->prev = head;
    newNode->next = head->next;
    head->next->prev = newNode;
    head->next = newNode;
  }

  void deleteBeforeTail() {

    tail->prev->prev->next = tail;
    tail->prev = tail->prev->prev;
  }

  void erase(doublyLinkedListNode *givenNode) { // used to delete any random
                                                // node given pointer to it
    doublyLinkedListNode *previousNode = givenNode->prev;
    doublyLinkedListNode *nextNode = givenNode->next;
    previousNode->next = givenNode->next;
    nextNode->prev = givenNode->prev;
    delete givenNode;
  }

  int lastNodeValue() { // will return the key of the last node
    return tail->prev->key;
  }
  doublyLinkedListNode *
  firstNodePointer() { // will return the pointer to node after the head node
    return head->next;
  }

  bool isEmpty() { // check for empty condition
    return head->next == tail;
  }
};

class LFUCache {

public:
  unordered_map<int, doublyLinkedListNode *> address;
  unordered_map<int, int> keyValue, counter;
  unordered_map<int, DLL> cacheMemory;
  int minimumFrequency, capacity;

  LFUCache(int c) { // constructor
    capacity = c;
  }

  void set(int key, int value) {
    if (capacity == 0)
      return;
    if (keyValue.find(key) !=
        keyValue.end()) { // whenever key will be inserted for th firt time
      keyValue[key] = value;
      cacheMemory[counter[key]].erase(address[key]);
      if (minimumFrequency == counter[key] and
          cacheMemory[counter[key]].isEmpty()) {
        minimumFrequency++;
      }
      counter[key]++;
      cacheMemory[counter[key]].insertNode(key);
      address[key] = cacheMemory[counter[key]].firstNodePointer();
      return;
    }
    if (capacity == keyValue.size()) { // now we need to evict page from cache
      cacheEviction();
    }

    keyValue[key] = value;
    counter[key] = 1;
    cacheMemory[1].insertNode(key);
    address[key] = cacheMemory[1].firstNodePointer();
    minimumFrequency = 1;
  }
  int get(int key) {
    if (keyValue.find(key) == keyValue.end())
      return -1; // for out of bounds

    cacheMemory[counter[key]].erase(address[key]);
    if (minimumFrequency == counter[key] and
        cacheMemory[counter[key]].isEmpty()) {
      minimumFrequency++;
    }
    counter[key]++;
    cacheMemory[counter[key]].insertNode(key);
    address[key] = cacheMemory[counter[key]].firstNodePointer();
    return keyValue[key];
  }
  void cacheEviction() { // used to remove page from the cache when cache
                         // capacity is full

    int temp = cacheMemory[minimumFrequency].lastNodeValue();

    cacheMemory[minimumFrequency].deleteBeforeTail();
    address.erase(temp);
    counter.erase(temp);
    keyValue.erase(temp);
  }
};

int main() {

  int capacity;
  cin >> capacity;

  LFUCache lfuObject(capacity);
  int noOfQueries;
  cin >> noOfQueries;

  int resArray[noOfQueries];
  for (int i = 0; i < noOfQueries;
       i++) { // initialise the resArray with default values
    resArray[i] = -2;
  }

  for (int i = 0; i < noOfQueries; i++) {
    int queryType;
    cin >> queryType;
    if (queryType == 1) { // call to getValue function
      int key;
      cin >> key;
      int res;
      res = lfuObject.get(key);
      // res = lfuobject.get(key);
      resArray[i] = res;
    }
    if (queryType == 2) { // call to setValue function
      int key;
      int value;
      cin >> key;
      cin >> value;
      lfuObject.set(key, value);
    }
  }

  for (int i = 0; i < noOfQueries;
       i++) { // print the final output to the screen
    if (resArray[i] != -2) {
      cout << resArray[i] << endl;
    }
  }
  return 0;
}