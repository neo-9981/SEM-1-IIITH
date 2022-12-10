#include <iostream>
using namespace std;

template <class T>
class Deque {
  int front;
  int rear;
  int capacity = 0;  // total no of cells in dq
  int filled = 0;    // total no of elements in dq
  T *dq_array = nullptr;

 public:
  // 1. deque() - initialise a empty deque.Time complexity: O(1)
  Deque() {  // constructor
    capacity = 2;
    filled = 0;
    front = rear = -1;
    dq_array = new T[capacity];
  }
  void deque(){
    clear();
  }

void deque(int x, T d){
    resize(x, d);
}
  // 2. deque(n, x) - initialise a deque of length n with all values as x.
  // Time complexity: O(n)
  Deque(int n, int x) {
    capacity = n;
    filled = n;
    front = 0;
    rear = n-1;
    dq_array = new T[capacity];
  }

  // 3. push_back(x) - append data x at the end. Time complexity: O(1)
  void push_back(T x) {
    // update DQ
    if (filled == capacity) {  // if the dq_array is full
      int new_capacity = capacity * 2;
      T *new_dq_array = new T[new_capacity];

      // insert all the elements of the old DQ to the new DQ
      for (int i = 0; i < filled; i++) {
        new_dq_array[i] = dq_array[front];
        front = (front + 1) % capacity;
      }
      new_dq_array[filled] = x;
      filled++;
      capacity = new_capacity;
      delete[] dq_array;
      dq_array = new_dq_array;
      front = 0;
      rear = filled - 1;
    } else {
      rear = (rear + 1) % capacity;
      dq_array[rear] = x;
      filled++;
    }
  }

  // 4. pop_back() - erase data at the end. Time complexity: O(1)
  T pop_back() {
    if (filled > 0) {
      filled--;
      rear = (rear - 1 + capacity) % capacity;
    } else {
      // by default dq_array
      capacity = 2;
      filled = front = rear = 0;
      if(dq_array){
            delete[] dq_array;
      }
      dq_array = new T[capacity];
      return dq_array[0];  // return default value
    }
  }

  // 5. push_front(x) - append data x at the beginning. Time complexity:O(1)
  void push_front(T x) {
    // update DQ
    if (filled == capacity) {  // if the dq_array is full
      int new_capacity = capacity * 2;
      T *new_dq_array = new T[new_capacity];

      // insert all the elements of the old DQ to the new DQ
      for (int i = 0; i < filled; i++) {
        new_dq_array[i] = dq_array[front];
        front = (front + 1) % capacity;
      }
      capacity = new_capacity;
      new_dq_array[capacity - 1] = x;
      delete[] dq_array;
      dq_array = new_dq_array;
      front = capacity - 1;
      rear = filled - 1;
      filled++;
    } else {
      front = (front + capacity - 1) % capacity;
      dq_array[front] = x;
      filled++;
    }
  }

  // 6. pop_front() - erase an element from the beginning. Time
  // complexity: O(1)
  T pop_front() {
    if (filled > 0) {
      filled--;
      front = (front + 1) % capacity;
    } else {
      // by default dq_array
      capacity = 2;
      filled = front = rear = 0;
      if(dq_array){
            delete[] dq_array;
      }
      dq_array = new T[capacity];
      return dq_array[0];  // return default value
    }
  }

  // 7. front() - returns the first element(value) in the deque. Time
  // complexity: O(1)
  T get_front() {
    if (filled > 0) {
      return dq_array[front];
    } else {
      // by default dq_array
      capacity = 2;
      filled = front = rear = 0;
      if(dq_array){
            delete[] dq_array;
      }
      dq_array = new T[capacity];
      return dq_array[0];  // return default value
    }

  }

  // 8. back() - returns the last element(value) in the deque. Time
  // complexity: O(1)
  T back() {
    if (filled > 0) {
      return dq_array[rear];
    } else {
      // by default dq_array
      capacity = 2;
      filled = front = rear = 0;
      if(dq_array){
            delete[] dq_array;
      }
      dq_array = new T[capacity];
      return dq_array[0];
    }
    /* T res;
    res = dq_array[rear];
    return res; */
  }

  // 9. D[n] - returns the nth element of the deque. You need to overload
  // the [ ] operator. Time complexity: O(1)
  T operator[](int indx) {
    // need to return default value
    if (indx >= filled) {
      // by default dq_array
      T temp;
      return temp;  // return default value
    }
    return dq_array[(front + indx) % capacity];
  }
    //  T()
  // 10. empty() - returns true if deque is empty else returns false. Time
  /// complexity: O(1)
  bool empty() { return filled == 0; }

  // 11. size() - returns the current size of deque. Time complexity: O(1)
  int size() { return filled; }

  /* 12. resize(x, d) - change the size dynamically to new size x. Time
  complexity: O(n)
  - If the new size x is greater than the current size of the
  deque, then insert new elements with default value d at the
  end of the queue.
  - If the new size x is smaller than the current size, then keep
  x elements from the beginning of the deque. */
  void resize(int x, T d) {
    int new_capacity = x;
    T *new_dq_array = new T[new_capacity];

    if (x >= filled) {
      // insert all the elements of the old DQ to the new DQ
      for (int i = 0; i < filled; i++) {
        new_dq_array[i] = dq_array[front];
        front = (front + 1) % capacity;
      }

      for (int i = filled; i < x; i++) {
      new_dq_array[i] = d;
      }
    }

    else {
      for (int i = 0; i < x; i++) {
        new_dq_array[i] = dq_array[front];
        front = (front + 1) % capacity;
      }
    }
    filled = x;
    capacity = new_capacity;
    delete[] dq_array;
    dq_array = new_dq_array;
    front = 0;
    rear = filled - 1;
  }

  // 13. clear() - remove all elements o99f deque. Time complexity: O(1)
  void clear() {
    delete[] dq_array;

    // by default dq_array
    capacity = 2;
    filled = 0;
    front = rear = -1;
    dq_array = new T[capacity];
  }

};
  
  int main() {
    //NOTE - Spqecify template for DQ and val during compile time
    Deque<int> DQ; // or Deque<float> DQ;
    int val; // or float val;
    // 0 - QUIT
    // 1 - deque()
    // 2 - deque(n, x)
    // 3 - push_back(x)
    // 4 - pop_back()
    // 5 - push_front(x)
    // 6 - pop_front()
    // 7 - front()
    // 8 - back()
    // 9 - empty()
    // 10 - size()
    // 11 - resize(n, x)
    // 12 - clear()
    // 13 - D[n]
    // 14 - display()
    int choice, flag = 1, n;
    while(flag) {
      
        cout << "-------------------------\n";
        cout << "choice: ";
        cin >> choice;
        switch(choice) {
            case 1:
                DQ.deque();
                break;
            case 2:
                cin >> n >> val;
                DQ.deque(n, val);
                break;
            case 3:
                cin >> val;
                DQ.push_back(val);
                break;
            case 4:
                DQ.pop_back();
                break;
            case 5:
                cin >> val;
                DQ.push_front(val);
                break;
            case 6:
                DQ.pop_front();
                break;
            case 7:
                cout << DQ.get_front() << endl;
                break;
            case 8:
                cout << DQ.back() << endl;
                break;
            case 9:
                cout << ((DQ.empty()) ? "true" : "false") << endl;
                break;
            case 10:
                cout << DQ.size() << endl;
                break;
            case 11:
                cin >> n >> val;
                DQ.resize(n, val);
                break;
            case 12:
                DQ.clear();
                break;
            case 13:
                cin >> n;
                cout << DQ[n] << endl;
                break;
            case 14:
                // For Debugging
               // DQ.display();
                break;
            default:
                cout << "Enter correct choice\n";
        }
    }
    return 0;
}
