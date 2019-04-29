#include <bits/stdc++.h>
using namespace std;
 
#define MAXN 100005
#define pb push_back
 
int N, M;
int D[MAXN], deg[MAXN];
int order[MAXN];
vector <int> con[MAXN];
 
int main()
{
    scanf("%d%d", &N, &M);
    for (int i=1;i<=M;i++){
        int a, b; scanf("%d%d", &a, &b);
        deg[++a]++; deg[++b]++;
        con[a].pb(b); con[b].pb(a);
    }
    for (int i=1;i<=N;i++) order[i] = i;
    sort(order+1, order+N+1, [](const int &a, const int &b){
        return deg[a] < deg[b];
    });
    for (int i=1;i<=N;i++) D[i] = 1;
    for (int i=1;i<=N;i++){
        int n = order[i];
        for (int t: con[n]) if (deg[n] < deg[t])
            D[t] = max(D[t], D[n]+1);
        }
    int ans = 0;
    for (int i=1;i<=N;i++) ans = max(ans, D[i]);
    printf("%d\n", ans);
}

