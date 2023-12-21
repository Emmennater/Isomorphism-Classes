// Author: Emmett J.
// Date:   12/21/2023

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <bitset>
#include <cmath>
#include <chrono>
using namespace std;
using namespace std::chrono;

#define N 7

void generateCombinationsHelper(vector<int>& currentCombination, int start, int n, vector<vector<int>>& result) {
    if (start == n) {
        result.push_back(currentCombination);
        return;
    }

    for (int i = 0; i < n; ++i) {
        if (find(currentCombination.begin(), currentCombination.end(), i) == currentCombination.end()) {
            currentCombination.push_back(i);
            generateCombinationsHelper(currentCombination, start + 1, n, result);
            currentCombination.pop_back();
        }
    }
}

vector<vector<int>> generateCombinations(int n) {
    vector<vector<int>> result;
    vector<int> currentCombination;

    generateCombinationsHelper(currentCombination, 0, n, result);

    return result;
}

struct Matrix {
    bool edges[N][N];

    Matrix& operator=(const Matrix& other) {
        memcpy(edges, other.edges, N * N * sizeof(bool));
        return *this;
    }
};

string hashMat(const Matrix& m) {
    const int BYTES_STORED = (N * (N - 1) / 2) / 8 + 1;
    char hashValue[BYTES_STORED];
    memset(hashValue, 0, BYTES_STORED);

    int idx = 0;
    for (int i = 0; i < N - 1; ++i) {
        for (int j = i + 1; j < N; ++j) {
            hashValue[idx/8] |= m.edges[i][j] << (idx % 8);
            ++idx;
        }
    }

    return string(hashValue, BYTES_STORED);
}

string hashMatMap(const Matrix& m, const vector<int>& map) {
    const int BYTES_STORED = (N * (N - 1) / 2) / 8 + 1;
    char hashValue[BYTES_STORED];
    memset(hashValue, 0, BYTES_STORED);

    int idx = 0;
    for (int i = 0; i < N - 1; ++i) {
        for (int j = i + 1; j < N; ++j) {
            hashValue[idx/8] |= m.edges[map[i]][map[j]] << (idx % 8);
            ++idx;
        }
    }

    return string(hashValue, BYTES_STORED);
}

inline int findIsomorhpismClasses() {
    vector<vector<int>> mappings = generateCombinations(N);
    unordered_map<string, bool> visited;
    vector<Matrix> matrices = vector<Matrix>{Matrix()};
    const int MAX_EDGES = N * (N - 1) / 2;
    int count = 1;

    // Do half the iterations since it symmetrical, then double it afterwards.
    for (int edgesLeft = 0; edgesLeft < MAX_EDGES / 2; ++edgesLeft) {
        vector<Matrix> newMatrices;

        // Loop over the previously descovered non-isomorphic graphs.
        for (Matrix& m1 : matrices) {
            for (int i = 0; i < N - 1; ++i) {
                for (int j = i + 1; j < N; ++j) {
                    // Generating new graphs with +1 edge
                    if (m1.edges[i][j]) continue;
                    Matrix m2 = m1;
                    m2.edges[i][j] = true;
                    m2.edges[j][i] = true;

                    // Rooting out visited graphs
                    string m2hash = hashMat(m2);
                    if (visited.find(m2hash) != visited.end()) continue;
                    
                    // Visiting all isomorphisms of unvisited graphs
                    newMatrices.push_back(m2);
                    for (vector<int>& map : mappings) {
                        // This is the most taxing part of the algorithm (2M executions for N = 7)
                        visited[hashMatMap(m2, map)] = true;
                    }
                }
            }
        }

        // Replace old list with graphs that have +1 edge
        matrices = move(newMatrices);
        count += matrices.size();
    }

    // This is where we do the doubling shenanigans
    // If iterations is odd subtract the extra iteration count so there is only one.
    // Remeber... MAX_EDGES is iterations - 1 because of the initial one we added,
    // so its even for MAX_EDGES.
    count = MAX_EDGES % 2 == 0 ? count * 2 - matrices.size() : count * 2;

    return count;
}

int main() {
    vector<vector<int>> mappings = generateCombinations(N);
    unordered_map<string, bool> visited;

    cout << "Calculating isomorphism classes for " << N << " nodes...\n";

    auto start = high_resolution_clock::now();
    int count = findIsomorhpismClasses();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Isomorhpism Classes: " << count << endl;
    cout << duration_cast<milliseconds>(duration).count() << " milliseconds\n";

    return 0;
}
