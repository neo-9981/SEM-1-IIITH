#include <iostream>
#include <unordered_map>
using namespace std;

int currentSize = 0;         // used to know the current size of the cache
int capacity = 0;            // denotes the total capacity of the LRU cache
class doublyLinkedListNode { // node structure for DLL
public:
  int key;
  int value;
  doublyLinkedListNode *prev;
  doublyLinkedListNode *next;
  doublyLinkedListNode(int k, int v) {
    key = k;
    value = v;
    prev = NULL;
    next = NULL;
  }
};

doublyLinkedListNode *head;
doublyLinkedListNode *tail;

unordered_map<int, doublyLinkedListNode *> mp;

void insertNode(
    doublyLinkedListNode *newNode) { // we will always insert the newnode after
                                     // the head to maintain the LRU property
  newNode->prev = head;
  newNode->next = head->next;
  head->next->prev = newNode;
  head->next = newNode;
}

void deleteNode(doublyLinkedListNode *givenNode) { // deletes any arbitrary
                                                   // node
  doublyLinkedListNode *previousNode = givenNode->prev;
  doublyLinkedListNode *nextNode = givenNode->next;
  previousNode->next = givenNode->next;
  nextNode->prev = givenNode->prev;
}

void moveAfterHead(
    doublyLinkedListNode *currentNode) { // move the node right after the head
  deleteNode(currentNode);               // remove the node
  insertNode(currentNode); // and then move that node right after the head
}

void deleteBeforeTail() { // we will delete node just before tail for cache
                          // eviction as it will be the LRU one.
  mp.erase(tail->prev->key);
  deleteNode(tail->prev);
}

void setValue(int key, int value) { // set the <key,value> pair

  if (!(mp.find(key) == mp.end())) { // the key is already present

    // get the address from the map
    mp[key]->value = value;
    moveAfterHead(mp[key]);

  } else { // if we are inserting the key for the first time into the cache
    if (currentSize != capacity) { // if the cache capacity is not yet full
      doublyLinkedListNode *newNode =
          new doublyLinkedListNode(key, value); // first initialise the node
      insertNode(newNode); // then insert the node right after the head
      currentSize++;

      mp[key] = newNode; // update the map

    } else { // if the capacity of the cache is full, so we need to evict LRU
             // element from the cache
      deleteBeforeTail(); // first remove the last element from the cache (i.e.
                          // the LRU one)
      currentSize--;

      doublyLinkedListNode *newNode =
          new doublyLinkedListNode(key, value); // first initialise the node
      insertNode(newNode); // then insert the node right after the head
      currentSize++;

      mp[key] = newNode; // update the map
    }
  }
}

int getValue(int key) {
  if (mp.find(key) ==
      mp.end()) { // first check whether it is present in the map
    return -1;
  } else {
    moveAfterHead(mp[key]);
    doublyLinkedListNode *temp = mp[key];
    return temp->value; // return the value of the node
  }
}

int main() {
  head = new doublyLinkedListNode(-999, -999);
  tail = new doublyLinkedListNode(-999, -999);
  head->prev = NULL;
  head->next = tail;

  tail->prev = head;
  tail->next = NULL;

  cin >> capacity;

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
      res = getValue(key);
      resArray[i] = res;
    }
    if (queryType == 2) { // call to setValue function
      int key;
      int value;
      cin >> key;
      cin >> value;
      setValue(key, value);
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