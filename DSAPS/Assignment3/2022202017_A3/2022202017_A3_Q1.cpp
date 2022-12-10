#include <iostream>
#include <vector>
using namespace std;
vector<string> resultAutoComplete; // vector to store the final result of autoComplete
vector<int> resultSpellCheck; // vector ot store the final result of spellCheck
vector<string> resultAutoCorrect; // vector to store the final result of autoCorrect
vector<string> inputAutoCorrect; // vector to store the input for editDistance problem
class trieNode {
public:
  trieNode *children[26];
  bool endOfWord; // flag to know the end of the string
  trieNode() {    // constructor
    for (int i = 0; i < 26; i++)
      children[i] = NULL;
    endOfWord = false;
  }
};

class trie {
private:
  trieNode *pointerToRoot;

public:
  trie() {
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
  }

  bool spellCheck(string inputString) { // used to check whether the given word
                                        // is present in the trie or not
    trieNode *T = pointerToRoot;
    for (char c : inputString) {
      if (T->children[c - 'a'] == NULL) {
        return false;
      }
      T = T->children[c - 'a'];
    }
    // resultSpellCheck.push_back(T->endOfWord);
    cout << T->endOfWord << endl;
  }

  void helperAutoComplete(trieNode *T, string str) {
    if (T->endOfWord) {
      resultAutoComplete.push_back(str);
    }

    for (int i = 0; i < 26; i++) {
      if (T->children[i]) {
        string strTemp = str + char(i + 'a');
        helperAutoComplete(T->children[i], strTemp);
      }
    }
         
  }

  bool
  autoComplete(string prefixString) { // used to find all words in the trie
                                      // which begin with the given input string

    trieNode *T = pointerToRoot;
    for (char x : prefixString) {
      int c = x - 'a';
      if (T->children[c] == NULL) {
        cout << "The given word doesn't exist in the trie!\n";
         return false;
      }
      T = T->children[c];
    }

    helperAutoComplete(T, prefixString);

// print the result for second operation
  if(resultAutoComplete.size()>0){
    cout << resultAutoComplete.size() << endl;
  for (auto str : resultAutoComplete) {
    cout << str << endl;
  }
  }
  resultAutoComplete.clear(); // clear the entires of the vector

     return true;
  }

// used for counting edit distance 
    int dp[110][110];
    int editDistanceHelper(string word1,string word2,int i,int j)
    {
        if(i<0 && j<0)
        {
            return 0;
        }
        if(i<0 && j>=0)
        {
            return j+1;
        }
        if(i>=0 && j<0)
        {
            return i+1;
        }
        if(dp[i][j]!=-1)
        {
            return dp[i][j];
        }
        int res=0;
        if(word1[i]!=word2[j])
        {
            int a=1+editDistanceHelper(word1,word2,i,j-1);
            int b=1+editDistanceHelper(word1,word2,i-1,j);
            int c=1+editDistanceHelper(word1,word2,i-1,j-1);
            res=min(a,min(b,c));
        }else
        {
            res=editDistanceHelper(word1,word2,i-1,j-1);
        }
        return dp[i][j]=res;
    }
    int editDistance(string word1, string word2) {
        for(int i=0; i<110; i++){
            for(int j=0; j<110; j++){

                dp[i][j]=-1;
            }
            
        }
        int len1=word1.size();
        int len2=word2.size();
        if(len1==0 || len2==0)
        {
            return abs(len1-len2);
        }
        return editDistanceHelper(word1,word2,len1,len2);
    }

void merge(vector<string>& inputVector, int start, int mid, int end) {
int a = start ,b = mid+1;

string result[end-start+1];
int k=0;

for(int i = start ;i <= end ;i++) {
    if(a > mid)      //checks if first part comes to an end or not .
       result[ k++ ] = inputVector[ b++] ;

   else if ( b > end)   //checks if second part comes to an end or not
       result[ k++ ] = inputVector[ a++ ];

   else if( inputVector[ a ] < inputVector[ b ])     //checks which part has smaller element.
      result[ k++ ] = inputVector[ a++ ];

   else
      result[ k++ ] = inputVector[ b++];
 }
  for (int a=0 ; a< k ;a ++) {
   // push back the elements into the original array
     inputVector[ start++ ] = result[ a ] ;                          
  }
}

void merge_sort (vector<string>& inputVector , int start , int end )
   {
           if( start < end ) {
           int mid = (start + end ) / 2 ;           
           merge_sort (inputVector, start , mid ) ;                 
           merge_sort (inputVector,mid+1 , end ) ;            

          merge(inputVector,start , mid , end );   
   }                    
}


  bool autoCorrect(string inputString) { // Find all the words in the dictionary
    // which are at an edit distance(Levenshtein distance) of at most 3 from the
    // given input.
    // traverse for all the strings of the trie and compare for it's edit distance with the given
    // input string if it is less than 3 then push it into the vector and print the vector
    // else skip
    int sizeInput = inputAutoCorrect.size();
    for(int i=0; i<sizeInput; i++){ // for all the words present in the dictionary 
        int getEditDistance = editDistance(inputAutoCorrect[i], inputString);
        if(getEditDistance<=3){ // if the edit distance is atmost 3, then push the word into the result vector for autocorrect
                resultAutoCorrect.push_back(inputAutoCorrect[i]);
        }
    }
    int sizeOutput = resultAutoCorrect.size();
    merge_sort(resultAutoCorrect, 0, sizeOutput-1); // sort the resultant array
    
    // print utility
    cout << sizeOutput << endl;
    for(auto it: resultAutoCorrect){
        cout << it << endl;
    }

    // now clear the vector 
    resultAutoCorrect.clear();

  }

//   void printTrie(trieNode *root) {}
};

int main() {

  int noOfinputs;
  int noOfQueries;
  vector<int> result1;
  cin >> noOfinputs;
  cin >> noOfQueries; 

  trie trieObject;
  for (int i = 0; i < noOfinputs; i++) {
    string inputString;
    cin >> inputString;
    inputAutoCorrect.push_back(inputString); // store the input words into the vector to be later used by autoCorrect function 
    trieObject.insertWord(inputString);
  }

  for (int i = 0; i < noOfQueries; i++) {
    int operationType;
    string inputQuery;
    cin >> operationType;
    cin >> inputQuery;

    if (operationType == 1) { // call to spellCheck
      int result = trieObject.spellCheck(inputQuery);

    } else if (operationType == 2) { // call to autoComplete
       resultAutoComplete.clear();
      trieObject.autoComplete(inputQuery);

    } else if (operationType == 3) { // call to autoCorrect
        trieObject.autoCorrect(inputQuery);
    } else {
      cout << "Invalid input from the user!\n";
    }
  }
  
  return 0;
}