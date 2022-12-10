#include <iostream>
#include <vector>
using namespace std;
vector<int> vec1, vec2;
int total;
int maxElement = 1;

int find(int a) {
  if (vec1[a] == a)
    return a;
  return (vec1[a] = find(vec1[a]));
}

int main() {
  int n, q, b1, b2;
  cin >> n >> q;
  total = n;
  vec1.resize(n);
  vec2.resize(n);
  for (int i = 0; i < n; i++) {
    vec1[i] = i;
    vec2[i] = 1;
  }
  for (int i = 0; i < q; i++) {
    cin >> b1 >> b2;
    int pa = find(b1), pb = find(b2);
    if (pa != pb) {
      vec1[pa] = pb;
      total--;
      vec2[pb] += vec2[pa];
      if (maxElement < vec2[pb]) {
        maxElement = vec2[pb];
      }
    }
    cout << total << " " << maxElement << endl;
  }
  cout << endl;

  return 0;
}