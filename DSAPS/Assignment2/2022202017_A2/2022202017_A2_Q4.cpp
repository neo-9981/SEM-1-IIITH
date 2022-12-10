#include <algorithm>
#include <iostream>
using namespace std;

template <class T> class sparseArray {
public:
  class element { // represents the particular element of a sparseArray matrix
  public:
    int iRow;
    int jColumn;
    T value;
  };

  // public:
  int noOfRows;
  int noOfColumns;
  int noOfNonZeroElements;
  element sparseElementArray[100];

  // public:
  sparseArray() {
    noOfRows = 0;
    noOfColumns = 0;
    noOfNonZeroElements = 0;
  }
  sparseArray(int N, int M) {
    noOfRows = N;
    noOfColumns = M;
    noOfNonZeroElements = 0;
    // sparseElementArray = new element[100000];
  }
  // ~sparseArray() { delete[] sparseElementArray; }
};

template <typename T>
void swap1(T &a, T &b) {
  T tmp = a;
  a = b;
  b = tmp;
}

template <typename T>
void addArray(sparseArray<T> *s1, sparseArray<T> *s2, int N, int M) {
  sparseArray<T> sum(N, M); // sum will store the final sum
  sum.sparseElementArray[s1->noOfNonZeroElements + s2->noOfNonZeroElements];
  int i = 0; // used to iterate over the first sparse matrix
  int j = 0; // used to iterate over the second sparse matrix
  int k = 0; // used to iterate over the sum sparse matrix

  while (i < s1->noOfNonZeroElements && j < s2->noOfNonZeroElements) {
    if ((s1->sparseElementArray[i].iRow) < (s2->sparseElementArray[j].iRow)) {
      sum.sparseElementArray[k++] = s1->sparseElementArray[i++];
    } else if ((s1->sparseElementArray[i].iRow) >
               (s2->sparseElementArray[j].iRow)) {
      sum.sparseElementArray[k++] = s2->sparseElementArray[j++];
    } else {
      if ((s1->sparseElementArray[i].jColumn) <
          (s2->sparseElementArray[j].jColumn)) {
        sum.sparseElementArray[k++] = s1->sparseElementArray[i++];
      } else if ((s1->sparseElementArray[i].jColumn) >
                 (s2->sparseElementArray[j].jColumn)) {
        sum.sparseElementArray[k++] = s2->sparseElementArray[j++];
      } else {
        sum.sparseElementArray[k] = s1->sparseElementArray[i++];
        sum.sparseElementArray[k++].value += s2->sparseElementArray[j++].value;
      }
    }
  }
  // cout << "PRINT1\n";
  while(i!=s1->noOfNonZeroElements){
     sum.sparseElementArray[k++] = s1->sparseElementArray[i++]; 
  }
  // cout << "PRINT2"<<s2->noOfNonZeroElements<<"//"<<j<<endl;
  while(j!=s2->noOfNonZeroElements){
    // cout << "PRINT5"<<s2->noOfNonZeroElements<<"//"<<j<<endl;
    sum.sparseElementArray[k++] = s2->sparseElementArray[j++];
  }
  // cout << "PRINT3\n";
  sum.noOfNonZeroElements = k;
  cout << "Printing addition of the sparse matrices: \n";
  int temp = 0;
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < M; j++) {
      if (temp < sum.noOfNonZeroElements && sum.sparseElementArray[temp].iRow == i &&
          sum.sparseElementArray[temp].jColumn == j) {
        cout << sum.sparseElementArray[temp].value << " ";
        temp++;
      } else {
        cout << 0 << " ";
      }
    }
    cout << endl;
  }

  /* for(int i = 0; i < sum.noOfNonZeroElements; i++) {
      cout << sum.sparseElementArray[i].iRow << " " <<
    sum.sparseElementArray[i].jColumn
           << " " << sum.sparseElementArray[i].value << endl;
    }  */
}

int flag = 0;
template <typename T> void transposeArray(sparseArray<T> *s, int N, int M) {
    // cout << "Inside Transpose\n";
  for (int i = 0; i < s->noOfNonZeroElements; i++) {
    swap1(s->sparseElementArray[i].iRow, s->sparseElementArray[i].jColumn);
  }
    /* cout << "PRINT1\n";
     for(int i = 0; i < s->noOfNonZeroElements; i++) {
      cout << s->sparseElementArray[i].iRow << " " <<
    s->sparseElementArray[i].jColumn
           << " " << s->sparseElementArray[i].value << endl;
    }   */

  sort(s->sparseElementArray, s->sparseElementArray + s->noOfNonZeroElements,
       [](auto p, auto q) {
         return p.iRow < q.iRow || p.iRow == q.iRow && p.jColumn < q.jColumn;
       });
/* cout << "PRINT2\n";

for(int i = 0; i < s->noOfNonZeroElements; i++) {
      cout << s->sparseElementArray[i].iRow << " " <<
    s->sparseElementArray[i].jColumn
           << " " << s->sparseElementArray[i].value << endl;
    }   */

  if (flag == 0) {
    cout << "Printing transpose of the sparse matrices: \n";
  int temp = 0;
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < N; j++) {
      if (temp < s->noOfNonZeroElements && s->sparseElementArray[temp].iRow == i &&
          s->sparseElementArray[temp].jColumn == j) {
        cout << s->sparseElementArray[temp].value << " ";
        temp++;
      } else {
        cout << 0 << " ";
      }
    }
    cout << endl;
  }
  }
}

template <typename T>
void multiplyArray(sparseArray<T> *s1, sparseArray<T> *s2, int N1, int M1,
                   int N2, int M2) {

  //               cout << "Print1\n";
  if (M1 != N2) {
    cout << "Cannot multiply the matrices with given dimensions!\n";
    return;
  }
  flag = 1;

  sparseArray<int> mul(N1,M2);
  // first take transpose of second matrix
  // cout << "Print2\n";
  transposeArray(s2, N2, M2);
  // cout << "Print3\n";
  for (int i = 0; i < s1->noOfNonZeroElements; i++) {
    for (int j = 0; j < s2->noOfNonZeroElements; j++) {
      if ((s1->sparseElementArray[i].jColumn) ==
          (s2->sparseElementArray[j].jColumn)) {

        mul.sparseElementArray[mul.noOfNonZeroElements].value =
            (s1->sparseElementArray[i].value) *
            (s2->sparseElementArray[j].value);
        mul.sparseElementArray[mul.noOfNonZeroElements].iRow = i;
        mul.sparseElementArray[mul.noOfNonZeroElements].jColumn = j;
        mul.noOfNonZeroElements++;
      }
    }
  }

/* for(int i = 0; i < mul.noOfNonZeroElements; i++) {
      cout << mul.sparseElementArray[i].iRow << " " <<
    mul.sparseElementArray[i].jColumn
           << " " << mul.sparseElementArray[i].value << endl;
    } */






  // cout << "Print4\n";
  sort(mul.sparseElementArray,
       mul.sparseElementArray + mul.noOfNonZeroElements, [](auto p, auto q) {
         return p.iRow < q.iRow || p.iRow == q.iRow && p.jColumn < q.jColumn;
       });

  /* for(int i = 0; i < mul.noOfNonZeroElements; i++) {
      cout << mul.sparseElementArray[i].iRow << " " <<
    mul.sparseElementArray[i].jColumn
           << " " << mul.sparseElementArray[i].value << endl;
    } */

  // cout << "Print5\n";
  // now we need to remove the identical cases

    

  for (int i = 0; i < mul.noOfNonZeroElements - 1; i++) {
    // cout << "Print6\n";
    while ((i < mul.noOfNonZeroElements - 1) &&
           (mul.sparseElementArray[i].iRow ==
            mul.sparseElementArray[i + 1].iRow) &&
           (mul.sparseElementArray[i].jColumn ==
            mul.sparseElementArray[i + 1].jColumn)) {
      mul.sparseElementArray[i].value = mul.sparseElementArray[i].value +
                                         mul.sparseElementArray[i + 1].value;

      // now shift the remaining elements to the left
      int j = i + 1;
      for (j; j < mul.noOfNonZeroElements - 1; j++) {
        // cout << "Print7\n";
        mul.sparseElementArray[j].iRow = mul.sparseElementArray[j + 1].iRow;
        mul.sparseElementArray[j].jColumn =
            mul.sparseElementArray[j + 1].jColumn;
        mul.sparseElementArray[j].value = mul.sparseElementArray[j + 1].value;
      }
      mul.noOfNonZeroElements--;
    }
  }

 for(int i = 0; i < mul.noOfNonZeroElements; i++) {
      cout << mul.sparseElementArray[i].iRow << " " <<
    mul.sparseElementArray[i].jColumn
           << " " << mul.sparseElementArray[i].value << endl;
    } 

  /* // cout << "Print8\n";
   cout << "Printing multiplication of the sparse matrices: \n";
  int temp = 0;
  for (int i = 0; i < N1; i++) {
    for (int j = 0; j < N2; j++) {
      if (temp < mul.noOfNonZeroElements && mul.sparseElementArray[temp].iRow == i &&
          mul.sparseElementArray[temp].jColumn == j) {
        cout << mul.sparseElementArray[temp].value << " ";
        temp++;
      } else {
        cout << 0 << " ";
      }
    }
    cout << endl;
  } */
}

template <class T> class sparseLL {
public:
  class node {
  public:
    int iRow;
    int jColumn;
    T value;
    node *next = NULL;
    node() {}
    node(int r, int c, T v) {
      iRow = r;
      jColumn = c;
      value = v;
    }
  };

  int noOfRows;
  int noOfColumns;
  int noOfNonZeroElements;
  node *head;
  node *tail;
  sparseLL() {

    head = NULL;
    tail = NULL;
  }

  void insert(int r, int c, T v) {
    auto n = new node(r, c, v);
    if ((head == NULL) &&
        (tail == NULL)) { // i.e. we are inserting the first element
      head = n;
      tail = head;
    } else { // when more than one element is present
      tail->next = n;
      tail = n;
    }
  }

  node *midpoint(node *head) {
    if (head == NULL or head->next == NULL) {
      return head;
    }

    node *slow = head, *fast = head->next;
    while (fast && fast->next) {
      slow = slow->next;
      fast = fast->next->next;
    }

    return slow;
  }

  node *mergeSortedLists(node *a, node *b) {
    if (a == NULL)
      return b;
    if (b == NULL)
      return a;

    node *temp;
    // p.iRow < q.iRow || p.iRow == q.iRow && p.jColumn < q.jColumn
    if (a->iRow <= b->iRow || a->iRow == b->iRow && a->jColumn < b->jColumn) {
      temp = a;
      temp->next = mergeSortedLists(a->next, b);
    } else {
      temp = b;
      temp->next = mergeSortedLists(a, b->next);
    }

    return temp;
  }

  node *sortList(node *head) {
    if (head == NULL or head->next == NULL) {
      return head;
    }

    node *mid = midpoint(head);
    node *a = head;
    node *b = mid->next;

    mid->next = NULL;

    a = sortList(a);
    b = sortList(b);

    node *temp = mergeSortedLists(a, b);
    return temp;
  }

  void deleteNode(node *head, node *d) {

    node *temp = head;
    while (temp->next != d) {
      temp = temp->next;
    }
    temp->next = d->next;
    d->next = NULL;
    delete (d);
  }
};

template <typename T>
void addLL(sparseLL<T> *s1, sparseLL<T> *s2, int N1, int M1, int N2, int M2) {
  // cout << "print1\n";
  sparseLL<T> sum; // used to store the final sum of the matrices
  auto iterator1 = s1->head;
  auto iterator2 = s2->head;

  while ((iterator1 != NULL) && (iterator2 != NULL)) {
    // cout << "print2\n";
    if ((iterator1->iRow) < (iterator2->iRow)) {
      // cout << "print3\n";
      sum.insert(iterator1->iRow, iterator1->jColumn, iterator1->value);
      iterator1 = iterator1->next;
    } else if ((iterator1->iRow) > (iterator2->iRow)) {
      // cout << "print4\n";
      sum.insert(iterator2->iRow, iterator2->jColumn, iterator2->value);
      iterator2 = iterator2->next;
    } else {
      // cout << "print5\n";
      if ((iterator1->jColumn) < (iterator2->jColumn)) {
        // cout << "print6\n";
        sum.insert(iterator1->iRow, iterator1->jColumn, iterator1->value);
        iterator1 = iterator1->next;
      } else if ((iterator1->jColumn) > (iterator2->jColumn)) {
        // cout << "print7\n";
        sum.insert(iterator2->iRow, iterator2->jColumn, iterator2->value);
        iterator2 = iterator2->next;
      } else { // when both row & column are matching
        // now we need to add the values
        // cout << "print8\n";
        if (iterator1 != NULL && iterator2 != NULL) {
          sum.insert(iterator1->iRow, iterator1->jColumn,
                     iterator1->value + iterator2->value);
          iterator1 = iterator1->next;
          iterator2 = iterator2->next;
        }
      }
    }
  }
  while (iterator1 != NULL) {
    sum.insert(iterator1->iRow, iterator1->jColumn, iterator1->value);
    iterator1 = iterator1->next;
  }
  while (iterator2 != NULL) {
    sum.insert(iterator2->iRow, iterator2->jColumn, iterator2->value);
    iterator2 = iterator2->next;
  }

  if (flag == 0) {
    // print utility
    cout << "Printing addition of the sparse matrix:\n";
    /* auto iteratorAdd11 = sum.head;
    while (iteratorAdd11 != NULL) {
      cout << iteratorAdd11->iRow << " " << iteratorAdd11->jColumn << " "
           << iteratorAdd11->value << endl;
      iteratorAdd11 = iteratorAdd11->next;
    } */

    auto iteratorAdd12 = sum.head;
    for (int i = 0; i < N1; i++) {
      for (int j = 0; j < M1; j++) {
        // cout << "print9\n";

        if (iteratorAdd12 != NULL && iteratorAdd12->iRow == i &&
            iteratorAdd12->jColumn == j) {
          cout << iteratorAdd12->value << " ";
          iteratorAdd12 = iteratorAdd12->next;
        } else
          cout << "0 ";
      }
      cout << endl;
    }
  }
}
int flag1 = 0;
template <typename T> void transposeLL(sparseLL<T> *s, int N, int M) {
  // cout << "Print1\n";
  swap1(N, M);
  auto iterator = s->head;
  while (iterator != NULL) {
    swap(iterator->iRow, iterator->jColumn);
    iterator = iterator->next;
  }
  // cout << "Print1\n";
  auto newHead = s->sortList(s->head);
  s->head = newHead;
  // print utility
  // cout << "before print utility\n";
  /* auto iteratorTrans11 = s->head;
  while (iteratorTrans11 != NULL) {
    cout << iteratorTrans11->iRow << " " << iteratorTrans11->jColumn << " "
         << iteratorTrans11->value << endl;
    iteratorTrans11 = iteratorTrans11->next;
  } */

  if (flag1 == 0) {
    auto iteratorTrans12 = s->head;
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < M; j++) {
        // cout << "print9\n";

        if (iteratorTrans12 != NULL && iteratorTrans12->iRow == i &&
            iteratorTrans12->jColumn == j) {
          cout << iteratorTrans12->value << " ";
          iteratorTrans12 = iteratorTrans12->next;
        } else
          cout << "0 ";
      }
      cout << endl;
    }
  }
}

template <typename T>
void multiplyLL(sparseLL<T> *s1, sparseLL<T> *s2, int N1, int M1, int N2,
                int M2) {

  // cout << "Print1\n";
  if (M1 != N2) {
    cout << "Cannot multiply the matrices with given dimensions!\n";
    return;
  }
  flag1 = 1;

  auto mul = new sparseLL<T>();

  // first take transpose of second matrix
  // cout << "Print2\n";
  transposeLL(s2, N2, M2);
  // cout << "Print3\n";

  // sparseLL<T> sum; // used to store the final sum of the matrices
  auto iterator1 = s1->head;

  while ((iterator1 != NULL)) {
    auto iterator2 = s2->head;
    while ((iterator2 != NULL)) {
      // cout << "print4\n";
      if ((iterator1->jColumn) ==
          (iterator2->jColumn)) { // when both column value matches then add
                                  // their respective values
        //  cout << "print5\n";
        mul->insert(iterator1->iRow, iterator2->iRow,
                    iterator1->value * iterator2->value);
      }
      iterator2 = iterator2->next;
    }
    iterator1 = iterator1->next;
  }

  /* cout << "Priting before sort!\n";

  auto iteratorMul111 = mul->head;
  while (iteratorMul111 != NULL) {
    cout << iteratorMul111->iRow << " " << iteratorMul111->jColumn << " "
         << iteratorMul111->value << endl;
    iteratorMul111 = iteratorMul111->next;
  }
 */
  //  cout << "print6\n";
  auto newHead = mul->sortList(mul->head);
  mul->head = newHead;

  // now we need to merge repeated entries

  // cout << "print7\n";
  auto tempHead = mul->head;
  while (tempHead->next != NULL) {
    // cout << "print8\n";
    if ((tempHead != NULL) && (tempHead->next != NULL) &&
        (tempHead->iRow == tempHead->next->iRow) &&
        (tempHead->jColumn == tempHead->next->jColumn)) {
      //  cout << "print9\n";
      tempHead->value = tempHead->value + tempHead->next->value;
      // now we need to delete the tempHead->next node
      //  cout << "print10\n";
      mul->deleteNode(mul->head, tempHead->next);
      //  cout << "print11\n";
    }
    tempHead = tempHead->next;
  }
  //  cout << "print12\n";
  // print utility
  // cout << "before print utility\n";
  /*   auto iteratorMul11 = mul->head;
    while (iteratorMul11 != NULL) {
      cout << iteratorMul11->iRow << " " << iteratorMul11->jColumn << " "
           << iteratorMul11->value << endl;
      iteratorMul11 = iteratorMul11->next;
    }
   */
  auto iteratorMul12 = mul->head;
  for (int i = 0; i < N1; i++) {
    for (int j = 0; j < M2; j++) {
      //  cout << "print13\n";

      if (iteratorMul12 != NULL && iteratorMul12->iRow == i &&
          iteratorMul12->jColumn == j) {
        cout << iteratorMul12->value << " ";
        iteratorMul12 = iteratorMul12->next;
      } else
        cout << "0 ";
    }
    cout << endl;
  }
}

int main() {

  cout << "Select the type of DataStructure: " << endl;
  cout << "1. Array" << endl;
  cout << "2. Linked List" << endl;

  int typeOfDS;
  cin >> typeOfDS;
  sparseArray<int> s1;
  sparseArray<int> s2;
  sparseArray<int> s3;
  int N1, M1;
  int N2, M2;
  int N3, M3;

  sparseLL<int> l1;
  sparseLL<int> l2;
  sparseLL<int> l3;

  switch (typeOfDS) {
    int typeOfOperation;
    int userInputElement;
  case 1: // arrayType
    cout << "1.Addition\n";
    cout << "2.Multiplication\n";
    cout << "3.Transpose\n";
    cin >> typeOfOperation;
    if (typeOfOperation == 1 ||
        typeOfOperation ==
            2) { // for addition and multiplication we require two matrices

      cin >> N1;
      cin >> M1;
      // sparseArray<int> s1(N1, M1);
      s1.noOfRows = N1;
      s1.noOfColumns = M1;
      for (int i = 0; i < N1; i++) {
        for (int j = 0; j < M1; j++) {
          cin >> userInputElement;
          if (userInputElement != 0) {
            s1.sparseElementArray[s1.noOfNonZeroElements].iRow = i;
            s1.sparseElementArray[s1.noOfNonZeroElements].jColumn = j;
            s1.sparseElementArray[s1.noOfNonZeroElements].value =
                userInputElement;
            s1.noOfNonZeroElements++;
          }
        }
      }

      /* for (int i = 0; i < s1.noOfNonZeroElements; i++) {
        cout << s1.sparseElementArray[i].iRow << " "
             << s1.sparseElementArray[i].jColumn << " "
             << s1.sparseElementArray[i].value << endl;
      } */

      cin >> N2;
      cin >> M2;

      //   sparseArray<int> s2(N2, M2);
      s2.noOfRows = N2;
      s2.noOfColumns = M2;
      for (int i = 0; i < N2; i++) {
        for (int j = 0; j < M2; j++) {
          cin >> userInputElement;
          if (userInputElement != 0) {
            s2.sparseElementArray[s2.noOfNonZeroElements].iRow = i;
            s2.sparseElementArray[s2.noOfNonZeroElements].jColumn = j;
            s2.sparseElementArray[s2.noOfNonZeroElements].value =
                userInputElement;
            s2.noOfNonZeroElements++;
          }
        }
      }
      /* for (int i = 0; i < s2.noOfNonZeroElements; i++) {
        cout << s2.sparseElementArray[i].iRow << " "
             << s2.sparseElementArray[i].jColumn << " "
             << s2.sparseElementArray[i].value << endl;
      } */

    } else { // for transpose we require single input
      cin >> N3;
      cin >> M3;
      //   sparseArray<int> s3(N3, M3);
      s3.noOfRows = N3;
      s3.noOfColumns = M3;
      int userInputElement;
      for (int i = 0; i < N3; i++) {
        for (int j = 0; j < M3; j++) {
          cin >> userInputElement;
          if (userInputElement != 0) {
            s3.sparseElementArray[s3.noOfNonZeroElements].iRow = i;
            s3.sparseElementArray[s3.noOfNonZeroElements].jColumn = j;
            s3.sparseElementArray[s3.noOfNonZeroElements].value =
                userInputElement;
            s3.noOfNonZeroElements++;
          }
        }
      }
      /*  for (int i = 0; i < s3.noOfNonZeroElements; i++) {
         cout << s3.sparseElementArray[i].iRow << " "
              << s3.sparseElementArray[i].jColumn << " "
              << s3.sparseElementArray[i].value << endl;
       } */
    }

    if (typeOfOperation == 1) {
      // call to addition
      addArray(&s1, &s2, N1, M1);
    } else if (typeOfOperation == 2) {
      // call to multiplication
      multiplyArray(&s1, &s2, N1, M1, N2, M2);
    } else if (typeOfOperation == 3) {
      // call to transpose
      transposeArray(&s3, N3, M3);
    } else {
      cout << "Invalid user input!\n";
    }

    break;

  case 2: // linkedListType
    cout << "1.Addition\n";
    cout << "2.Multiplication\n";
    cout << "3.Transpose\n";
    cin >> typeOfOperation;
    if (typeOfOperation == 1 ||
        typeOfOperation ==
            2) { // for addition and multiplication we require two matrices

      cin >> N1;
      cin >> M1;
      l1.noOfRows = N1;
      l1.noOfColumns = M1;

      for (int i = 0; i < N1; i++) {
        for (int j = 0; j < M1; j++) {
          cin >> userInputElement;
          if (userInputElement != 0) {
            l1.insert(i, j, userInputElement);
            l1.noOfNonZeroElements++;
          }
        }
      }

      // print utility
      /* auto iterator11 = l1.head;
      for (int i = 0; i < N1; i++) {
          for (int j = 0; j < M1; j++) {
              if (iterator11->iRow == i && iterator11->jColumn == j) {
                  cout << iterator11->value << " ";
                  iterator11 = iterator11->next;
              }
              else
                  cout << "0 ";
          }
          cout << endl;
      }*/

      /* auto iterator12 = l1.head;
      while(iterator12!=NULL){
        cout << iterator12->iRow << " " << iterator12->jColumn << " " <<
      iterator12->value <<endl; iterator12 = iterator12->next;
      } */

      cin >> N2;
      cin >> M2;

      l2.noOfRows = N2;
      l2.noOfColumns = M2;

      for (int i = 0; i < N2; i++) {
        for (int j = 0; j < M2; j++) {
          cin >> userInputElement;
          if (userInputElement != 0) {
            l2.insert(i, j, userInputElement);
            l2.noOfNonZeroElements++;
          }
        }
      }
      // print utility
      /* auto iterator21 = l2.head;
      for (int i = 0; i < N2; i++) {
          for (int j = 0; j < M2; j++) {
              if (iterator21->iRow == i && iterator21->jColumn == j) {
                  cout << iterator21->value << " ";
                  iterator21 = iterator21->next;
              }
              else
                  cout << "0 ";
          }
          cout << endl;
      }*/

      /* auto iterator22 = l2.head;
      while(iterator22!=NULL){
        cout << iterator22->iRow << " " << iterator22->jColumn << " " <<
      iterator22->value <<endl; iterator22 = iterator22->next;
      } */

    } else { // for transpose we require single input
      cin >> N3;
      cin >> M3;

      l3.noOfRows = N3;
      l3.noOfColumns = M3;
      int userInputElement;
      for (int i = 0; i < N3; i++) {
        for (int j = 0; j < M3; j++) {
          cin >> userInputElement;
          if (userInputElement != 0) {
            l3.insert(i, j, userInputElement);
            l3.noOfNonZeroElements++;
          }
        }
      }

      // print utility
      /* auto iterator31 = l3.head;
       for (int i = 0; i < N3; i++) {
          for (int j = 0; j < M3; j++) {
              if (iterator31->iRow == i && iterator31->jColumn == j) {
                  cout << iterator31->value << " ";
                  iterator31 = iterator31->next;
              }
              else
                  cout << "0 ";
          }
          cout << endl;
      }*/

      /* auto iterator32 = l3.head;
      while(iterator32!=NULL){
        cout << iterator32->iRow << " " << iterator32->jColumn << " " <<
      iterator32->value <<endl; iterator32 = iterator32->next;
      } */
    }

    if (typeOfOperation == 1) {
      // call to addition
      // cout << "Before add\n";
      addLL(&l1, &l2, N1, M1, N2, M2);
      // cout << "After add\n";
    } else if (typeOfOperation == 2) {
      // call to multiplication
      // cout << "Before multiply\n";
      multiplyLL(&l1, &l2, N1, M1, N2, M2);
      // cout << "After multiply\n";
    } else if (typeOfOperation == 3) {
      // call to transpose
      // cout << "Before add\n";
      transposeLL(&l3, N3, M3);
      //  cout << "After add\n";
    } else {
      cout << "Invalid user input!\n";
    }
    break;

  default:
    break;
  }

  return 0;
}