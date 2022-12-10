#include <iostream>
#include <math.h>
using namespace std;

struct treeNode {
  long int sum; // used to hold the current sum of the subsequence
  int index; // used to hold the next index to be visited into the subsequence
  treeNode() {}                 // default constructor
  treeNode(long int s, int i) { // parameterised constructor
    sum = s;
    index = i;
  }
};

void absoluteArray(long int *array, int size) {
  for (int i = 0; i < size; i++) {
    if (array[i] < 0) {
      array[i] = -array[i];
    }
  }
}

int getMaxSum(long int *array,
              int size) { // returns the maxSum i.e. the sum of all
  // the +ve integers into the array
  long int maxSum = 0;
  for (int i = 0; i < size; i++) {
    if (array[i] > 0) {
      maxSum = maxSum + array[i];
    }
  }
  return maxSum;
}

void swap1(long int &a, long int &b) {
  long int tmp = a;
  a = b;
  b = tmp;
}

void quickSort(long int *nums, int l, int r) {
  if (l >= r)
    return;
  int i = l;
  for (int j = l; j <= r - 1; j++) {
    if (nums[j] <= nums[r]) {
      swap(nums[i], nums[j]);
      i++;
    }
  }
  swap(nums[i], nums[r]);
  quickSort(nums, l, i - 1);
  quickSort(nums, i + 1, r);
}

class priorityQ {

public:
  treeNode *pqArray;
  int currSize;

  priorityQ() {
    currSize = 0;
    pqArray = new treeNode[100000];
  }

  void push(treeNode e) {
    currSize++;
    pqArray[currSize - 1] = e;

    long long ind = currSize - 1;
    while (true) {
      int parent = (ind - 1) / 2;
      if (parent < 0 || parent == ind ||
          pqArray[parent].sum <= pqArray[ind].sum) {
        break;
      }

      struct treeNode temp = pqArray[parent];
      pqArray[parent] = pqArray[ind];
      pqArray[ind] = temp;

      ind = parent;
    }
  }

  int size() { return currSize; }

  treeNode top() { return pqArray[0]; }
  void pop() {
    pqArray[0] = pqArray[currSize - 1];
    currSize--;
    heapify(0);
  }
  void heapify(int ind) {
    long long leftChild = 2 * ind + 1;
    long long rightChild = 2 * ind + 2;

    int maxIndex = ind;

    if (leftChild < currSize) {
      if (pqArray[leftChild].sum < pqArray[maxIndex].sum) {
        maxIndex = leftChild;
      }
    }
    if (rightChild < currSize) {
      if (pqArray[rightChild].sum < pqArray[maxIndex].sum) {
        maxIndex = rightChild;
      }
    }

    if (maxIndex != ind) {
      struct treeNode temp;
      temp.sum = pqArray[ind].sum;
      temp.index = pqArray[ind].index;

      pqArray[ind].sum = pqArray[maxIndex].sum;
      pqArray[ind].index = pqArray[maxIndex].index;

      pqArray[maxIndex].sum = temp.sum;
      pqArray[maxIndex].index = temp.index;

      heapify(maxIndex);
    }
  }
};

int main() {
  int noOfIntegers;
  int kValue;

  cin >> noOfIntegers;
  cin >> kValue;

  long int numsArray[noOfIntegers]; // input array
  for (int i = 0; i < noOfIntegers; i++) {
    cin >> numsArray[i];
  }

  // now sum get the maxSum from the array
  long int maxSum;
  maxSum = getMaxSum(numsArray, noOfIntegers); // store the maximum sum

  absoluteArray(numsArray, noOfIntegers); // makes all the array values +ve
  quickSort(numsArray, 0, noOfIntegers);  // sort the array

  int topKSum[kValue];
  topKSum[0] = 0;
  int currSizeTopKSumArray =
      1; // used to track the #elements present in the array

  priorityQ mh;
  // push into priorityQ
  treeNode n1;
  n1.sum = numsArray[0];
  n1.index = 1;

  mh.push(n1);

  while (currSizeTopKSumArray < kValue) {

    treeNode topNodeValue = mh.top();
    mh.pop();
    topKSum[currSizeTopKSumArray] = topNodeValue.sum;
    currSizeTopKSumArray++;

    if (topNodeValue.index < noOfIntegers) {

      treeNode n2;
      n2.sum = topNodeValue.sum + numsArray[topNodeValue.index];
      n2.index = topNodeValue.index + 1;
      mh.push(n2);

      treeNode n3;
      n3.sum = topNodeValue.sum - numsArray[topNodeValue.index - 1] +
               numsArray[topNodeValue.index];
      n3.index = topNodeValue.index + 1;
      mh.push(n3);
    }
  }

  // cout << maxSum << " ";
  for (int i = 0; i < kValue; i++) {
    cout << maxSum - topKSum[i] << " ";
    cout << endl;
  }

  return 0;
}
