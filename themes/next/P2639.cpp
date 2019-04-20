#include<iostream>
#include<algorithm>
using namespace std;

const int MAXN = 500 + 5;
const int MAXM = 45000 + 5;

int f[MAXM], w[MAXN];

int main() {
    int n, m;

    cin >> m >> n;
    for(int i = 1; i <= n; i++) {
        cin >> w[i];
    }

    for(int i = 1; i <= n; i++) {
        for(int j = m; j >= w[i]; j--) {
            f[j] = max(f[j], f[j - w[i]] + w[i]);
        }
    }

    cout << f[m] << endl;
}