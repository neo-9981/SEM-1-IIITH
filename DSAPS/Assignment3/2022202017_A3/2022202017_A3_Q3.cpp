#include <iostream>
using namespace std;
struct trieNode {

  trieNode *children[26];
  bool endOfWord; // flag to know the end of the string
  string word; // to store the final word 

  trieNode() { // constructor
    for (int i = 0; i < 26; i++)
      children[i] = NULL;
    endOfWord = false;
    word = "";
  }
};

class trie {
private:
  trieNode *pointerToRoot;

public:
  trie() { // constructor
    pointerToRoot = new trieNode(); // root of the trie
  }

  void insertWord(string inputString) { // to insert input strings into the trie
    trieNode *T = pointerToRoot;
    for (char c : inputString) {
      if (T->children[c - 'a'] == NULL)
        T->children[c - 'a'] = new trieNode();
      T = T->children[c - 'a'];
    }
    T->endOfWord = true;
    T->word = inputString; //store the word in the last node
  }

  void compute(char **grid, int i, int j, int r, int c,
             string *ans, int &count,trieNode *currentNode) {
    
    // if the current character is not present in the trie then skip and return
    // if the cell i alread visited then no need to visit it again skip and return
    // int idx = grid[i][j] - 'a';
      if (currentNode->children[grid[i][j] - 'a'] == NULL || grid[i][j] == '#') {
      return;
    }

    currentNode = currentNode->children[grid[i][j] - 'a'];
    if (currentNode->endOfWord==true) {
      ans[count]=(currentNode->word);
      count++;
      currentNode->endOfWord=false; // mark it as false so as to visit the same prefix words
      //return true;
    }

    char ch = grid[i][j]; // store current char
    grid[i][j] = '#';     // mark current node visited
    
    if (j < c - 1) // RIGHT
      compute(grid, i, j + 1, r, c, ans, count,currentNode);
    if (i < r - 1) // DOWN
      compute(grid, i + 1, j, r, c, ans, count, currentNode);
    if (j > 0) // LEFT
      compute(grid, i, j - 1, r, c, ans, count, currentNode);
    if (i > 0) // TOP
      compute(grid, i - 1, j, r, c, ans, count, currentNode);

    grid[i][j] = ch; //mark unvisited
  }

public:
  string* search(char **grid, int r, int c, string *words, int noOfSearch, int &count) {

    for (int i=0; i<noOfSearch; i++)
      insertWord(words[i]);
 
    string *solutionArr = new string[noOfSearch];// store the searched words in this array
    count=0; // used to store the index where the word is to be inserted
    for (int i = 0; i < r; ++i) {
      for (int j = 0; j < c; ++j)
        compute(grid, i, j, r, c, solutionArr, count, pointerToRoot);
    }
    return solutionArr;
  }
};


void merge(string inputArray[ ] , int start, int mid, int end) {
int a = start ,b = mid+1;

string result[end-start+1];
int k=0;

for(int i = start ;i <= end ;i++) {
    if(a > mid)      //checks if first part comes to an end or not .
       result[ k++ ] = inputArray[ b++] ;

   else if ( b > end)   //checks if second part comes to an end or not
       result[ k++ ] = inputArray[ a++ ];

   else if( inputArray[ a ] < inputArray[ b ])     //checks which part has smaller element.
      result[ k++ ] = inputArray[ a++ ];

   else
      result[ k++ ] = inputArray[ b++];
 }
  for (int a=0 ; a< k ;a ++) {
   // push back the elements into the original array
     inputArray[ start++ ] = result[ a ] ;                          
  }
}

void merge_sort (string inputArray[ ] , int start , int end )
   {
           if( start < end ) {
           int mid = (start + end ) / 2 ;           
           merge_sort (inputArray, start , mid ) ;                 
           merge_sort (inputArray,mid+1 , end ) ;            

          merge(inputArray,start , mid , end );   
   }                    
}

int main() {

  int row, col;
  cin >> row >> col;

  // char grid[row][col]; //to store the input grid
  char **grid;
  grid = new char*[row];
  for (int i = 0; i < row; i++) {
    grid[i] = new char[col];

    for (int j = 0; j < col; j++) {
      cin >> grid[i][j];
    }
  }

  int noOfSearch;
  cin >> noOfSearch;
  string arrip[noOfSearch]; // store the strings to be searched
  for (int i = 0; i < noOfSearch; i++) {
    string temp;
    cin >> temp;
    arrip[i] = temp;
  }

  trie obj;

  int count; // to be used in search function
  string *final;
  for(int i=0; i<noOfSearch; i++){
    final=obj.search(grid, row, col, arrip, noOfSearch, count);
  }
  
  cout << count << endl;
  // cout << "print" <<endl;
  merge_sort(final, 0, count-1); // sort the resultant array
  for(int i=0; i<count; i++){
    // cout << "print111" <<endl;
    cout << final[i] << endl; 

  }

  return 0;
}

