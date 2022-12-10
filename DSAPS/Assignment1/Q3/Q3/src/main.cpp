#include <iostream>
#include <fstream>

using namespace std;

void solve(int ***rgb, int H, int W, int C, int H_, int W_, int C_) {
    // We've provided you the driver.py and main.cpp for your convinience
    // Please go through them and understand the file handling and input/output format
    // Feel free to experiment on your own

    // can access the array using rgb[i][j][k] like a regular 3D array

    // Write your code here
}

int main() {
    string ip_dir = "./data/input/";
    string ip_file = "rgb_in.txt";
    ifstream fin(ip_dir + ip_file);

    int H, W, C;
    fin >> H >> W >> C;

    int ***rgb;
    rgb = new int **[H];
    for(int i = 0; i < H; ++i) {
        rgb[i] = new int *[W];
        for(int j = 0; j < W; ++j) {
            rgb[i][j] = new int[C];
            for(int k = 0; k < C; ++k)
                fin >> rgb[i][j][k];
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
    for(int i = 0; i < H_; ++i) {
        for(int j = 0; j < W_; ++j) {
            for(int k = 0; k < C_; ++k) {
                fout << rgb[i][j][k] << " ";
            }
        }
        fout << '\n';
    }
    fout.close();

    return 0;
}