#include <cmath>
#include <iostream>
#include <string>
using namespace std;

bool great(string dividend, string divisor) {
  return dividend.length() > divisor.length() ||
         dividend.length() == divisor.length() && dividend > divisor;
}

bool greateq(string dividend, string divisor) {
  return dividend.length() > divisor.length() ||
         dividend.length() == divisor.length() && dividend >= divisor;
}

string trim(string dividend) {
  int indx = 0;
  while (dividend[indx] == '0') {
    indx++;
  }

  dividend = dividend.substr(indx, dividend.length() - indx + 1);
  return dividend;
}

string addition(string op1, string op2) {
  // return to_string(stoi(op1)+stoi(op2));
  int op1Size = op1.size();
  int op2Size = op2.size();

  int minSize = min(op1Size, op2Size);
  int maxSize = max(op1Size, op2Size);

  string res = "";
  int carry = 0;
  int delta = maxSize - minSize;

  // padding
  if (op2Size < op1Size) {
    string pad = "";
    for (int i = 0; i < delta; i++) pad += "0";
    op2 = pad + op2;
  }

  // padding
  if (op1Size < op2Size) {
    string pad = "";
    for (int i = 0; i < delta; i++) pad += "0";
    op1 = pad + op1;
  }

  for (int i = maxSize - 1; i >= delta; i--) {
    int sum = (op1[i] - '0') + (op2[i] - '0') + carry;
    if (sum > 9) {
      carry = sum / 10;
      sum = sum % 10;
    } else {
      carry = 0;
    }
    res = to_string(sum) + res;
  }

  for (int i = delta - 1; i >= 0; i--) {
    if (minSize == op1Size) {  // op1 is smaller

      int sum = op2[i] - '0' + carry;
      if (sum > 9) {
        carry = sum / 10;
        sum = sum % 10;
      } else {
        carry = 0;
      }
      res = to_string(sum) + res;
    } else {  // op2 is smaller

      int sum = op1[i] - '0' + carry;
      if (sum > 9) {
        carry = sum / 10;
        sum = sum % 10;
      } else {
        carry = 0;
      }
      res = to_string(sum) + res;
    }
  }

  if (carry) {
    res = to_string(carry) + res;
  }
  return res;
}

string subtraction(string op1, string op2) {
  int op1Size = op1.size();
  int op2Size = op2.size();

  int minSize = min(op1Size, op2Size);
  int maxSize = max(op1Size, op2Size);

  string res = "";

  int delta = maxSize - minSize;

  // padding
  string pad = "";
  for (int i = 0; i < delta; i++) pad += "0";
  op2 = pad + op2;

  op2Size = op2.size();

  minSize = min(op1Size, op2Size);
  maxSize = max(op1Size, op2Size);

  int borrow = 0;
  for (int i = maxSize - 1; i >= 0; i--) {
    int sum = (op1[i] - '0') - (op2[i] - '0') + borrow;
    if (sum < 0) {
      sum = sum + 10;
      borrow = -1;
    } else {
      borrow = 0;
    }
    res = to_string(sum) + res;
  }
  string final_ans = trim(res);
  // dividend == "" ? "0" : dividend
  return final_ans == "" ? "0" : final_ans;
}

string multiplication(string op1, string op2) {
  string ans;
  int indx_op1 = 0;
  int indx_op2 = 0;

  int l1 = op1.length();
  int l2 = op2.length();

  int dp[l1 + l2 + 1] = {0};
  int mul = 0;
  int carry = 0;
  int temp_sum = 0;

  for (int i = l1 - 1; i >= 0; i--) {
    for (int j = l2 - 1; j >= 0; j--) {
      mul = ((op1[i] - '0') * (op2[j] - '0'));
      temp_sum = dp[i + j + 1] + mul;
      dp[i + j + 1] = temp_sum % 10;
      dp[i + j] += (temp_sum) / 10;
    }
  }

  for (int i = 0; i < l1 + l2; i++) {
    if (ans.size() != 0)
      ans += to_string(dp[i]);
    else if (dp[i] != 0)
      ans += to_string(dp[i]);
  }

  if (ans.size() == 0) return "0";

  /* for(int i=0; i<=l1+l2; i++){
    ans.push_back(dp[i]+'0');
  }
 */

  return trim(ans);

  /*
     //trimming leading zeros for op1
    while(op1[indx_op1]!='0'){
      indx_op1++;
    }
    op1 = op1.substr(indx_op1, op1.length()-indx_op1);

    //trimming leading zeros for op2
    while(op2[indx_op2]!='0'){
      indx_op2++;
    }
    op2 = op2.substr(indx_op2, op2.length()-indx_op2);
    int carry = 0;




    string s;
    for (int i = op1.length() - 1; i >= 0; i--) {
      s.clear();
      for (int j = 0; j > i-op1.length()+1; j++) s += '0';


      for (int j = op2.length()-1; j >= 0; j--) {
        int v = (op1[j] - '0') * (op2[i] - '0') + carry;

        ans = addition(ans, (to_string(v % 10)  + s ));

        carry = v / 10;
      }


       for(int j=op2.length()-1; j>=0; j--){
        string s;
        int carry;
        for(int i=op1.length(); i>=0; i--){
          int v = (op1[i]-'0')*(op2[j]-'0')+carry;

          s = to_string(v%10)+s;
          carry = v/10;

        }

      }
      if (carry) s = char(carry + '0') + s;

      ans = addition(((ans == "") ? "0" : ans), s);

    return ans; */
}

string modulo(string number, string divisor) {
  if (number == "0") {
    return "0";
  }

  int lenD = divisor.length();
  int lenN = number.length();
  if (lenD > lenN) {
    return number;
  }
  string dividend = number.substr(0, lenD);
  int indx = dividend.length();

  while (indx < lenN) {
    while (greateq(dividend, divisor)) {
      dividend = subtraction(dividend, divisor);
      dividend = trim(dividend);  // to trim leading zeros
    }
    dividend = dividend + number[indx++];
  }

  while (greateq(dividend, divisor)) {
    dividend = subtraction(dividend, divisor);
    dividend = trim(dividend);  // to trim leading zeros
  }

  return dividend == "" ? "0" : dividend;
}

string expo(string op1, size_t op2) {
  string res = op1;

  if (op2 == 0 || op1 == "1") return "1";

  while (--op2) {
    res = multiplication(res, op1);
  }

  return res;

  /* if (op2 == "0") return "1";

  if (op2 == "1") return op1;

  string ans = "1";

  //for(int i=0; ){}

   while (op2.length()) {
    //op2%2
    if (op2.back()%2) ans = multiplication(ans, op1);

    //use modulo as division

    op1 = multiplication(op1, to_string((stoul(op2)/2)));
  }

  return ans; */
}

string gcd(string op1, string op2) {
  if (op2 == "0") return op1;

  return gcd(op2, modulo(op1, op2));
}

string factorial(string op) {
  string ans = "1";

  while (op != "0") {
    ans = multiplication(ans, op);
    op = subtraction(op, "1");
  }

  return ans;
}

string eval1(string ip) {
  string st1, st2, st3, res;
  char ch = '+';
  int len = ip.length();
  for (int k = 0; k < len;) {
    if (isdigit(ip[k])) {
      st2.push_back(ip[k]);
      k++;
    } else {
       if(ip[k]=='x'){
        int i;
        for(i=k+1;isdigit(ip[i]) ;i++){
              st3+=ip[i];

        }
        k=i;
        st2 = multiplication(st2, st3);
        st3.clear();
      }else if (ch == '+') {
        st1 = addition(st1, st2);
        st2.clear();
        ch = ip[k];
        k++;
      } else if (ch == '-') {
        st1 = subtraction(st1, st2);
        st2.clear();
        ch = ip[k];
        k++;

      } 
    }
  }

  if(ch=='+'){
      res = addition(st1, st2); 
      return res;
  }
  else if(ch='-'){
    res = subtraction(st1, st2);
    return res;
  }
  else{
    res = multiplication(st1, st2);
    return res;
  }

// cout << st1 <<ch<< st2;
}

int main() {
  int choice;
  
  while(true){

    cout << R"(1. Addition, Subtraction & Multiplication
2. Exponentiation
3. GCD
4. Factorial)";

cin>> choice;
  string ip,op1,op2;
  size_t op3;
switch (choice)
{
case 1:
  cin >> ip;
  cout << eval1(ip) << endl;
  break;
case 2:
  
  cin >> op1;
  cin >> op3;
  cout << expo(op1, op3)<< endl;
  break;
case 3:
  
  cin >> op1;
  cin >> op2;
  cout << gcd(op1, op2)<< endl;
  break;
case 4:
  
  cin >> op1;
  cout << factorial(op1)<< endl;
  break;

default:
return 0;

}
  }
    return 0;
}
