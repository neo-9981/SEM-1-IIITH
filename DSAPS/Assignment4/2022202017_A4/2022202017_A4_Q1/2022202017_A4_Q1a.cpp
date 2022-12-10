
#include <iostream>
#include <vector>
using namespace std;

int main() {

  string ipStr;
  cin >> ipStr;

  vector<int> sArray;

  vector<int> count;
  count.resize(max(256, (int)ipStr.size()), 0);
  fill(count.begin(), count.end(), 0);

  int i = 0;
  if (i < (int)ipStr.size()) {
    do {
      count[ipStr[i]]++;
      i++;
    } while (i < (int)ipStr.size());
  }

  int j = 1;
  if (j < 256) {
    do {
      count[j] += count[j - 1];
      j++;
    } while (j < 256);
  }

  vector<int> pArray;
  pArray.resize(ipStr.size());

  int k = 0;
  if (k < (int)ipStr.size()) {
    do {
      pArray[--count[ipStr[k]]] = k;
      k++;
    } while (k < (int)ipStr.size());
  }

  vector<int> cArray;
  cArray.resize(ipStr.size());
  
  cArray[pArray[0]] = 0;
  int groups = 1;

  int ii = 1;
  if (ii < (int)ipStr.size()) {
    do {
      if (ipStr[pArray[ii]] != ipStr[pArray[ii - 1]])
        groups++;
      cArray[pArray[ii]] = groups - 1;

      ii++;
    } while (ii < (int)ipStr.size());
  }

  vector<int> pnArray;
  pnArray.resize((int)ipStr.size());

  vector<int> cnArray;
  cnArray.resize((int)ipStr.size());

  int h = 0;
  if ((1 << h) < (int)ipStr.size()) {
    do {

      int i = 0;
      if (i < (int)ipStr.size()) {
        do {

          pnArray[i] = pArray[i] - (1 << h);
          if (pnArray[i] < 0)
            pnArray[i] += (int)ipStr.size();

          i++;
        } while (i < (int)ipStr.size());
      }

      fill(count.begin(), count.begin() + groups, 0);

      int iii = 0;
      if (iii < (int)ipStr.size()) {
        do {
          count[cArray[pnArray[iii]]]++;
          iii++;
        } while (iii < (int)ipStr.size());
      }

      int jj = 1;
      if (jj < groups) {
        do {
          count[jj] += count[jj - 1];
          jj++;
        } while (jj < groups);
      }

      int kk = (int)ipStr.size() - 1;
      if (kk >= 0) {
        do {
          pArray[--count[cArray[pnArray[kk]]]] = pnArray[kk];
          kk--;
        } while (kk >= 0);
      }

      cnArray[pArray[0]] = 0;
      groups = 1;

      int mm = 1;
      if (mm < (int)ipStr.size()) {
        do {
          pair<int, int> cur = {
              cArray[pArray[mm]],
              cArray[(pArray[mm] + (1 << h)) % (int)ipStr.size()]};
          pair<int, int> prev = {
              cArray[pArray[mm - 1]],
              cArray[(pArray[mm - 1] + (1 << h)) % (int)ipStr.size()]};
          if (cur != prev)
            ++groups;
          cnArray[pArray[mm]] = groups - 1;
          mm++;
        } while (mm < (int)ipStr.size());
      }

      auto temp = std::move(cArray);
      cArray = std::move(cnArray);
      cnArray = std::move(temp);

      h++;
    } while ((1 << h) < (int)ipStr.size());
  }

  sArray = pArray;
  int index = sArray[0];
  int nn = ipStr.size();

  char opStr[(2 * nn)];

  for (int i = 0; i < nn; i++)
    opStr[i] = opStr[nn + i] = ipStr[i];

  for (int i = index; i < nn + index; i++)
    cout << opStr[i] << "";
  return 0;
}