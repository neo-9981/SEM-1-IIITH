#include <cfloat>
#include <cmath>
#include <fstream>
#include <iostream>

using std::cout;
using namespace std;

// matrix to store Energy function values
float **dualGradiantEnergyMatrix;

// function to calculate Energy Matrix
void dualGradientEnergyFunction(int ***rgb, int H, int W, int H_, int W_) {
  for (int i = 0; i < H; i++) {
    for (int j = 0; j < W; j++) {
      int u = (i == 0) ? (H - 1) : (i - 1);
      int d = (i == H - 1) ? (0) : (i + 1);
      int l = (j == 0) ? (W - 1) : (j - 1);
      int r = (j == W - 1) ? (0) : (j + 1);

      float Rxj = rgb[i][r][0] - rgb[i][l][0];
      float Gxj = rgb[i][r][1] - rgb[i][l][1];
      float Bxj = rgb[i][r][2] - rgb[i][l][2];

      float deltaX2 = Rxj * Rxj + Gxj * Gxj + Bxj * Bxj;

      float Ryi = rgb[u][j][0] - rgb[d][j][0];
      float Gyi = rgb[u][j][1] - rgb[d][j][1];
      float Byi = rgb[u][j][2] - rgb[d][j][2];

      float deltaY2 = Ryi * Ryi + Gyi * Gyi + Byi * Byi;

      float inputSqrt = deltaX2 + deltaY2;
      dualGradiantEnergyMatrix[i][j] = sqrt(inputSqrt);
    }
  }
}
void solve(int ***rgb, int H, int W, int C, int H_, int W_, int C_) {
  // We've provided you the driver.py and main.cpp for your convinience
  // Please go through them and understand the file handling and input/output
  // format Feel free to experiment on your own

  // can access the array using rgb[i][j][k] like a regular 3D array

  // Write your code here

  // Defining Gradient Energy matrix
  dualGradiantEnergyMatrix = new float *[H];
  for (int i = 0; i < H; i++) {
    dualGradiantEnergyMatrix[i] = new float[W];
  }

  // Calculating Vertical & Horizontal Seam & deleting it alternately.
  while ((H > H_) || (W > W_)) {
    // VERTICAL SEAM
    if (W > W_) {
      // CALCULATE GRADIENT MATRIX
      dualGradientEnergyFunction(rgb, H, W, H_, W_);

      // CALCULATE VERTICAL SEAM
      for (int r = 1; r < H; r++) {
        for (int c = 0; c < W; c++) {
          if (c == 0) {  // when the pixel belogs to first column

            dualGradiantEnergyMatrix[r][c] +=
                fmin(dualGradiantEnergyMatrix[r - 1][c],
                     dualGradiantEnergyMatrix[r - 1][c + 1]);
          }

          else if (c == W - 1) {  // when the pixel belongs to the last column

            dualGradiantEnergyMatrix[r][c] +=
                fmin(dualGradiantEnergyMatrix[r - 1][c],
                     dualGradiantEnergyMatrix[r - 1][c - 1]);
          } else {  // when the pixel doesn't belong to the boundary columns

            dualGradiantEnergyMatrix[r][c] +=
                fmin(dualGradiantEnergyMatrix[r - 1][c - 1],
                     fmin(dualGradiantEnergyMatrix[r - 1][c],
                          dualGradiantEnergyMatrix[r - 1][c + 1]));
          }
        }
      }

      // DELETE VERTICAL SEAM
      float minH = FLT_MAX;
      int minHIndx;
      for (int c = 0; c < W - 1;
           c++) {  // calculating the index of min energy value in bottom row
        if (dualGradiantEnergyMatrix[H - 1][c] < minH) {
          minH = dualGradiantEnergyMatrix[H - 1][c];
          minHIndx = c;
        }
      }

      // BACKTRACK CODE
      for (int r = H - 1; r > 0; r--) {
        for (int c = minHIndx; c < W - 1; c++) {  // SHIFT the pixels
          rgb[r][c][0] = rgb[r][c + 1][0];
          rgb[r][c][1] = rgb[r][c + 1][1];
          rgb[r][c][2] = rgb[r][c + 1][2];
        }

        // BACKTRACK CODE
        if (minHIndx == 0) {  // when the pixel belongs to the leftmost column
          if (dualGradiantEnergyMatrix[r - 1][minHIndx] >=
              dualGradiantEnergyMatrix[r - 1][minHIndx + 1])
            minHIndx += 1;
        }

        else if (minHIndx ==
                 W - 1) {  // when the pixel belongs to the rightmost column
          if (dualGradiantEnergyMatrix[r - 1][minHIndx - 1] <=
              dualGradiantEnergyMatrix[r - 1][minHIndx])
            minHIndx -= 1;
        } else {  // when the pixel belongs to the columns other than the
                  // leftmost & rightmost one
          if (dualGradiantEnergyMatrix[r - 1][minHIndx - 1] <=
                  dualGradiantEnergyMatrix[r - 1][minHIndx] &&
              dualGradiantEnergyMatrix[r - 1][minHIndx - 1] <=
                  dualGradiantEnergyMatrix[r - 1][minHIndx + 1]) {
            minHIndx -= 1;
          } else if (dualGradiantEnergyMatrix[r - 1][minHIndx + 1] <=
                         dualGradiantEnergyMatrix[r - 1][minHIndx] &&
                     dualGradiantEnergyMatrix[r - 1][minHIndx + 1] <=
                         dualGradiantEnergyMatrix[r - 1][minHIndx - 1]) {
            minHIndx += 1;
          }
        }
      }
      W--;
    }

    // HORIZONTAL SEAM
    if (H > H_) {
      // CALCULATE GRADIENT MATRIX
      dualGradientEnergyFunction(rgb, H, W, H_, W_);

      // CALCULATE HORIZONTAL SEAM
      for (int c = 1; c < W; c++) {
        for (int r = 0; r < H; r++) {
          if (r == 0) {  // when the pixel belogs to topmost row

            dualGradiantEnergyMatrix[r][c] +=
                fmin(dualGradiantEnergyMatrix[r][c - 1],
                     dualGradiantEnergyMatrix[r + 1][c - 1]);
          }

          else if (r ==
                   H - 1) {  // when the pixel belongs to the bottommost row

            dualGradiantEnergyMatrix[r][c] +=
                min(dualGradiantEnergyMatrix[r - 1][c - 1],
                    dualGradiantEnergyMatrix[r][c - 1]);
          } else {  // when the pixel doesn't belong to the boundary rows

            dualGradiantEnergyMatrix[r][c] +=
                min(dualGradiantEnergyMatrix[r - 1][c - 1],
                    min(dualGradiantEnergyMatrix[r][c - 1],
                        dualGradiantEnergyMatrix[r + 1][c - 1]));
          }
        }
      }

      // DELETE HORIZONTAL SEAM
      float minW = FLT_MAX;
      int minWIndx;
      for (int r = 0; r < H - 1; r++) {  // calculating the index of min energy
                                         // value in rightmost column
        if (dualGradiantEnergyMatrix[r][W - 1] < minW) {
          minW = dualGradiantEnergyMatrix[r][W - 1];
          minWIndx = r;
        }
      }

      // BACKTRACK CODE
      for (int c = W - 1; c > 0; c--) {
        for (int r = minWIndx; r < H - 1; r++) {  // SHIFT the pixels
          rgb[r][c][0] = rgb[r + 1][c][0];
          rgb[r][c][1] = rgb[r + 1][c][1];
          rgb[r][c][2] = rgb[r + 1][c][2];
        }

        if (minWIndx == 0) {  // when the pixel belongs to the topmost row
          if (dualGradiantEnergyMatrix[minWIndx + 1][c - 1] <=
              dualGradiantEnergyMatrix[minWIndx][c - 1])
            minWIndx += 1;
        }

        else if (minWIndx ==
                 W - 1) {  // when the pixel belongs to the bottommost row
          if (dualGradiantEnergyMatrix[minWIndx][c - 1] >=
              dualGradiantEnergyMatrix[minWIndx - 1][c - 1])
            minWIndx -= 1;
        }

        else {  // when the pixel belongs to the rows other than the topmost &
                // bottommost one
          if (dualGradiantEnergyMatrix[minWIndx - 1][c - 1] <=
                  dualGradiantEnergyMatrix[minWIndx][c - 1] &&
              dualGradiantEnergyMatrix[minWIndx - 1][c - 1] <=
                  dualGradiantEnergyMatrix[minWIndx + 1][c - 1]) {
            minWIndx -= 1;
          } else if (dualGradiantEnergyMatrix[minWIndx + 1][c - 1] <=
                         dualGradiantEnergyMatrix[minWIndx - 1][c - 1] &&
                     dualGradiantEnergyMatrix[minWIndx + 1][c - 1] <=
                         dualGradiantEnergyMatrix[minWIndx][c - 1]) {
            minWIndx += 1;
          }
        }
      }
      H--;
    }
  }
}

int main() {
  string ip_dir = "./data/input/";
  string ip_file = "rgb_in.txt";
  ifstream fin(ip_dir + ip_file);

  int H, W, C;
  fin >> H >> W >> C;

  int ***rgb;
  rgb = new int **[H];
  for (int i = 0; i < H; ++i) {
    rgb[i] = new int *[W];
    for (int j = 0; j < W; ++j) {
      rgb[i][j] = new int[C];
      for (int k = 0; k < C; ++k) fin >> rgb[i][j][k];
    }
  }
  fin.close();

  int H_, W_, C_;
  cout << "Enter new value for H (must be less than " << H << "): ";
  cin >> H_;
  cout << "Enter new value for W (must be less than " << W << "): ";
  cin >> W_;
  cout << '\n';
  C_ = C;

  solve(rgb, H, W, C, H_, W_, C_);

  string op_dir = "./data/output/";
  string op_file = "rgb_out.txt";
  ofstream fout(op_dir + op_file);

  fout << H_ << " " << W_ << " " << C_ << endl;
  for (int i = 0; i < H_; ++i) {
    for (int j = 0; j < W_; ++j) {
      for (int k = 0; k < C_; ++k) {
        fout << rgb[i][j][k] << " ";
      }
    }
    fout << '\n';
  }
  fout.close();

  return 0;
}
