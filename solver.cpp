#include <bits/stdc++.h>
#include <iostream>
using namespace std;
int a[9][9];    //-1 for empty; 0~8 for 1~9
int bits[9][9]; // bitmask for possible numbers
// define < for cell
struct cell {
  int x, y;
  bool operator<(const cell &rhs) const {
    if (x != rhs.x)
      return x < rhs.x;
    return y < rhs.y;
  }
};
vector<cell> effecting_cells[9][9];
vector<cell> effecting_cells_sep[9][9][3]; // row, col, block, self excluded
#define ENCODE_CANDIDATE(x, y, z) ((x) * 81 + (y) * 9 + (z))
#define DECODE_X(x) ((x) / 81)
#define DECODE_Y(x) ((x) / 9 % 9)
#define DECODE_Z(x) ((x) % 9)
#define LOWBIT(x) ((x) & (-(x)))
#define FOR_GRID(i, j)                                                         \
  for (int i = 0; i < 9; i++)                                                  \
    for (int j = 0; j < 9; j++)
#define NODES 9 * 9 * 9 * 2
#define ENCODE_CANDIDATE_WEAK_SIDE(x, y, z) ((x) * 81 + (y) * 9 + (z))
#define ENCODE_CANDIDATE_STRONG_SIDE(x, y, z)                                  \
  ((x) * 81 + (y) * 9 + (z) + 9 * 9 * 9)
#define DECODE_SIDE(x) ((x) > (9 * 9 * 9)) // 0 for weak, 1 for strong
int count_candidates() {
  int cnt = 0;
  FOR_GRID(i, j) { cnt += __builtin_popcount(bits[i][j]); }
  return cnt;
}
char effect_types_string[3][10] = {"row", "column", "block"};
std::ofstream dataout("data.in");
void visualize() {
  // for (int i = 0; i < 9; i++) {
  //   if (i % 3 == 0)
  //     cout << "+---+---+---+" << endl;
  //   for (int j = 0; j < 9; j++) {
  //     if (j % 3 == 0)
  //       cout << "|";
  //     if (a[i][j] == -1) {
  //       cout << " ";
  //     } else {
  //       cout << a[i][j] + 1;
  //     }
  //   }
  //   cout << "|" << endl;
  // }
  // cout << "+---+---+---+" << endl;
  int tmp[27][27];
  FOR_GRID(i, j) {
    for (int k = 0; k < 9; k++) {
      if (a[i][j] != -1) {
        tmp[i * 3 + k / 3][j * 3 + k % 3] = a[i][j] + 1;
      } else if (bits[i][j] & (1 << k)) {
        tmp[i * 3 + k / 3][j * 3 + k % 3] = k + 1;
      } else
        tmp[i * 3 + k / 3][j * 3 + k % 3] = 0;
    }
  }
  for (int i = 0; i < 27; i++) {
    if (i % 9 == 0)
      cout << "+===+===+===+===+===+===+===+===+===+" << endl;
    else if (i % 3 == 0)
      cout << "|---+---+---|---+---+---|---+---+---|" << endl;
    for (int j = 0; j < 27; j++) {
      if (j % 9 == 0)
        cout << "|";
      else if (j % 3 == 0)
        cout << ".";

      if (tmp[i][j] == 0)
        cout << " ";
      else
        cout << tmp[i][j];
    }
    cout << "|";
    cout << endl;
  }
  cout << "+===+===+===+===+===+===+===+===+===+" << endl;
}

int eliminate() {
  vector<vector<int>> G;
  G.resize(NODES);
  FOR_GRID(i, j) {
    for (int digit = 0; digit < 9; digit++) {
      if (int digit_mask = 1 << digit; bits[i][j] & digit_mask) {
        // pairs for same digit
        {
          // weak pair: can't be both true
          for (auto it : effecting_cells[i][j]) {
            if (bits[it.x][it.y] & digit_mask) {
              // A weak link pair (i, j) and (it.x, it.y) with digit
              G[ENCODE_CANDIDATE_WEAK_SIDE(i, j, digit)].push_back(
                  ENCODE_CANDIDATE_STRONG_SIDE(it.x, it.y, digit));
              // G[ENCODE_CANDIDATE_WEAK_SIDE(it.x, it.y, digit)].push_back(
              //     ENCODE_CANDIDATE_STRONG_SIDE(i, j, digit));
            }
          }
          // strong pair: only one of must be true
          for (int effect_type = 0; effect_type < 3; effect_type++) {
            int cnt = 0;
            for (auto it : effecting_cells_sep[i][j][effect_type]) {
              if (bits[it.x][it.y] & digit_mask) {
                cnt++;
              }
            }
            if (cnt == 1) {
              for (auto it : effecting_cells_sep[i][j][effect_type]) {
                if (bits[it.x][it.y] & digit_mask) {
                  // A strong link pair (i, j) and (it.x, it.y) with digit
                  G[ENCODE_CANDIDATE_STRONG_SIDE(i, j, digit)].push_back(
                      ENCODE_CANDIDATE_WEAK_SIDE(it.x, it.y, digit));
                  // G[ENCODE_CANDIDATE_STRONG_SIDE(it.x, it.y,
                  // digit)].push_back(
                  //     ENCODE_CANDIDATE_WEAK_SIDE(i, j, digit));
                }
              }
            }
          }
        }
        {
          // pairs for same cell
          int cnt = 0;
          for (int digit_ = 0; digit_ < 9; digit_++) {
            if (digit_ == digit)
              continue;
            if (bits[i][j] & (1 << digit_)) {
              cnt++;
              // A weak link pair (i, j) with digit and digit_
              G[ENCODE_CANDIDATE_WEAK_SIDE(i, j, digit)].push_back(
                  ENCODE_CANDIDATE_STRONG_SIDE(i, j, digit_));
            }
          }
          if (cnt == 1) {
            for (int digit_ = 0; digit_ < 9; digit_++) {
              if (digit_ == digit)
                continue;
              if (bits[i][j] & (1 << digit_)) {
                // A strong link pair (i, j) with digit and digit_
                G[ENCODE_CANDIDATE_STRONG_SIDE(i, j, digit)].push_back(
                    ENCODE_CANDIDATE_WEAK_SIDE(i, j, digit_));
              }
            }
          }
        }
      }
    }
  }

  // Graph Stat
  int edges = 0;
  for (int i = 0; i < NODES; i++) {
    edges += G[i].size();
  }
  cout << "Remain Candidates: " << count_candidates() << " Edges: " << edges
       << endl;
  int changed = 0;
  FOR_GRID(i, j) {
    for (int k = 0; k < 9; k++) {
      int ST = ENCODE_CANDIDATE_WEAK_SIDE(i, j, k);
      int ED = ENCODE_CANDIDATE_STRONG_SIDE(i, j, k);
      // Use BFS to check if there is a path from ST to ED, store the path
      queue<int> q;
      vector<int> pre(NODES, -1);
      q.push(ST);
      while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (auto v : G[u]) {
          if (pre[v] == -1) {
            pre[v] = u;
            q.push(v);
          }
        }
      }
      if (pre[ED] != -1) {
        changed++;
        bits[i][j] &= ~(1 << k);
        // There is a path from ST to ED
        vector<int> path;
        for (int u = ED; u != ST; u = pre[u]) {
          path.push_back(u);
        }
        path.push_back(ST);
        reverse(path.begin(), path.end());
        dataout << path.size() << " ";
        for (auto u : path) {
          dataout << u << " ";
        }
        dataout << endl;
        cout << "Path from " << ST << " to " << ED << ": ";
        for (auto u : path) {
          int side = DECODE_SIDE(u);
          int rest = u % (9 * 9 * 9);
          int x = DECODE_X(rest) + 1, y = DECODE_Y(rest) + 1,
              z = DECODE_Z(rest) + 1;
          cout << "(" << x << ", " << y << ", " << z << ") ";
          if (side == 0)
            cout << "-- ";
          else
            cout << "== ";
        }
        cout << "Thus we eliminate " << k + 1 << " from (" << i + 1 << ", "
             << j + 1 << ")" << endl;
      }
    }
  }
  return changed;
}
void set_new(int x, int y, int z) {
  assert(0 <= x && x < 9);
  assert(0 <= y && y < 9);
  assert(0 <= z && z < 9);
  a[x][y] = z;
  bits[x][y] = 1 << z;
  for (auto it : effecting_cells[x][y]) {
    bits[it.x][it.y] &= ~(1 << z);
  }
}
int already_set(int x, int y) { return a[x][y] != -1; }
int determine_trivial() {
  int determined = 0;
  FOR_GRID(i, j) {
    if (already_set(i, j))
      continue;
    // per cell
    if (__builtin_popcount(bits[i][j]) == 1) {
      for (int k = 0; k < 9; k++) {
        if (bits[i][j] & (1 << k)) {
          cout << "Determined (" << i + 1 << ", " << j + 1 << ") = " << k + 1
               << " due to unique cell" << endl;
          set_new(i, j, k);
          determined++;
          break;
        }
      }
    }

    for (int k = 0; k < 9; k++) {
      if (bits[i][j] & (1 << k)) {
        if (already_set(i, j))
          break;
        for (int effect_type = 0; effect_type < 3; effect_type++) {
          int cnt = 0;
          for (auto it : effecting_cells_sep[i][j][effect_type]) {
            if (bits[it.x][it.y] & (1 << k)) {
              cnt++;
            }
          }
          if (cnt == 0) {
            cout << "Determined (" << i + 1 << ", " << j + 1 << ") = " << k + 1
                 << " due to uniqueness in effect "
                 << effect_types_string[effect_type] << endl;
            set_new(i, j, k);
            determined++;
            break;
          }
        }
      }
    }
  }
  return determined;
}
int determine_trivial_in_turn() {
  int determined = 0;
  while (true) {
    int determined_now = determine_trivial();
    if (determined_now == 0)
      break;
    determined += determined_now;
  }
  return determined;
}
int main() {

  // preprocess
  FOR_GRID(i, j) {
    a[i][j] = 0;
    bits[i][j] = 0x1ff;
  }

  FOR_GRID(i, j) {
    set<cell> tmp;
    for (int k = 0; k < 9; k++) {
      tmp.insert({i, k});
      tmp.insert({k, j});
      int xx = i / 3 * 3 + k / 3, yy = j / 3 * 3 + k % 3;
      tmp.insert({xx, yy});
      if (k != j)
        effecting_cells_sep[i][j][0].push_back({i, k});
      if (k != i)
        effecting_cells_sep[i][j][1].push_back({k, j});
      if (xx != i || yy != j)
        effecting_cells_sep[i][j][2].push_back({xx, yy});
    }
    assert(effecting_cells_sep[i][j][0].size() == 8);
    assert(effecting_cells_sep[i][j][1].size() == 8);
    assert(effecting_cells_sep[i][j][2].size() == 8);
    tmp.erase({i, j});
    for (auto it : tmp) {
      effecting_cells[i][j].push_back(it);
    }
  }

  // read
  FOR_GRID(i, j) {
    cin >> a[i][j];
    a[i][j]--;
    if (a[i][j] != -1) {
      set_new(i, j, a[i][j]);
    }
  }
  determine_trivial_in_turn();
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      dataout << a[i][j] + 1 << " ";
    }
    dataout << endl;
  }
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      dataout << bits[i][j] << " ";
    }
    dataout << endl;
  }
  for (int iter = 0;; iter++) {
    cout << "Iteration " << iter << endl;
    visualize();
    int changed = eliminate();
    int determined = determine_trivial_in_turn();
    if (changed == 0)
      break;
    else {
      cout << "Eliminated " << changed << " candidates and determined "
           << determined << " cells" << endl;
    }
  }
}
/*
Sample out.txt:
8 0 5 0 0 0 2 0 0
9 0 0 0 0 3 0 1 0
0 0 4 0 0 7 0 5 0
0 4 2 0 0 0 0 0 0
0 0 0 9 0 5 0 0 0
1 0 0 0 8 6 0 0 4
0 0 0 0 5 0 7 3 0
3 0 8 0 0 0 0 0 9
0 0 0 7 0 0 0 0 0
*/