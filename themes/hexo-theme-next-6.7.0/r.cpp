#include<iostream>
using namespace std;

const int MAXN = 20 + 5;

int f[MAXN][MAXN];

int s(int k) {
    int ans = 0;

    while(k != 0) {
        if(k % 2 == 1) {
            ans += k % 10;
        }
        k /= 10;
    }

    return ans;
}

int e(int k) {
    int ans = 0;

    while(k != 0) {
        if(k % 2 == 0) {
            ans += k % 10;
        }
        k /= 10;
    }

    return ans;
}

int main() {
    int ans = -1;

    for(int i = 1; i <= 2010; i++) {
        for(int j = 1; j <= 2010; j++) {
            if(13 * i + 20 * j == 2010) {
                if(2 * i + 2 * j > ans) {
                    ans = 2 * i + 2 * j;
                }
            }
        }
    }

    cout << ans << endl;

    return 0;
}