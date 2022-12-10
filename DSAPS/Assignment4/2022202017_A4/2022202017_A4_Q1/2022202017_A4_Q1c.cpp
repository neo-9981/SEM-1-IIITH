#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

string lp(string s) {
  int n = s.size();
  if (n == 0)
    return "";
  vector<vector<bool>> dp(n, vector<bool>(n, false));
  for (int i = 0; i < n; i++) {
    dp[i][i] = true;
  }
  string ans = "";
  ans += s[0];
  for (int i = n - 1; i >= 0; i--) {
    for (int j = i + 1; j < n; j++) {
      if (s[i] == s[j]) {

        if (j - i == 1 || dp[i + 1][j - 1]) {
          dp[i][j] = true;
          if (ans.size() < j - i + 1) {
            ans = s.substr(i, j - i + 1);
          } else if (ans.size() == j - i + 1) {
            ans = min(ans, s.substr(i, j - i + 1));
          }
        }
      }
    }
  }
  return ans;
}

int main() {

  string str;
  cin >> str;
  cout << lp(str);

  return 0;
}