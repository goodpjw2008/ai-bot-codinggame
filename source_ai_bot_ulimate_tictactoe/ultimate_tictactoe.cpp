#include <bits/stdc++.h>
#pragma GCC optimize("Ofast,unroll-loops,inline")
#pragma GCC target("avx2,avx,fma,sse,sse2,sse3,sse4,ssse3")
using namespace std;
bool win[512];
unsigned char zcnt[512],zpos[512][9];
float iv[1100002],rsq[1100002],slg[1100002];
int reali[9][9],mi[81],li[81],globalturns;
struct tictactoe{
    unsigned short small[2][9]={},big[2]={},draw=0;
    unsigned char lw=0,turn=1;
    signed char nxtbig=-1;
    template <typename T>
    inline int getvalidfast(T*arr)const{
        int cnt=0,ubit=big[0]|big[1]|draw;
        if(nxtbig==-1||((ubit>>nxtbig)&1)){
            for(int i = 0; i < 9; i++){
                if((ubit>>i)&1)continue;
                int occ=small[0][i]|small[1][i];
                for(int j = 0; j < zcnt[occ]; j++){
                    arr[cnt++]=reali[i][zpos[occ][j]];
                }
            }
        }
        else{
            int b=nxtbig,occ=small[0][b]|small[1][b];
            for(int j = 0; j < zcnt[occ]; j++){
                arr[cnt++]=reali[b][zpos[occ][j]];
            }
        }
        return cnt;
    }
    inline int getleft()const{
        int cnt=0,ubit=big[0]|big[1]|draw;
        for(int i = 0; i < 9; i++){
            if((ubit>>i)&1)continue;
            cnt+=zcnt[small[0][i]|small[1][i]];
        }
        return cnt;
    }
    inline void apply(int mv){
        int p=turn-1,b=mi[mv],l=li[mv];
        small[p][b]|=1<<l;
        if(win[small[p][b]]){
            big[p]|=1<<b;
            if(win[big[p]])lw=turn;
        }
        else if((small[0][b]|small[1][b])==0x1ff){
            draw|=1<<b;
        }
        int ubit=big[0]|big[1]|draw;
        if(lw==0&&ubit==0x1ff){
            int s1=__builtin_popcount(big[0]),s2=__builtin_popcount(big[1]);
            if(s1>s2)lw=1;
            else if(s1<s2)lw=2;
            else lw=3;
        }
        if((ubit>>l)&1)nxtbig=-1;
        else nxtbig=l;
        turn=3-turn;
    }
};
struct splitmix64{
    uint64_t x;
    splitmix64(uint64_t x) : x(x) {}
    inline uint64_t operator()(){
        uint64_t z = (x += 0x9e3779b97f4a7c15);
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
        z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
        return z ^ (z >> 31);
    }
};
inline int getrandvalid(const tictactoe &st,splitmix64 &sp){
    int ubit=st.big[0]|st.big[1]|st.draw;
    if(st.nxtbig!=-1&&!((ubit>>st.nxtbig)&1)){
        int b=st.nxtbig,occ=st.small[0][b]|st.small[1][b];
        return reali[b][zpos[occ][sp()%zcnt[occ]]];
    }
    int cnt=0;
    for(int b = 0; b < 9; b++){
        if((ubit>>b)&1)continue;
        cnt+=zcnt[st.small[0][b]|st.small[1][b]];
    }
    int k=sp()%cnt;
    for(int b = 0; b < 9; b++){
        if((ubit>>b)&1)continue;
        int occ=st.small[0][b]|st.small[1][b],cur=zcnt[occ];
        if(k>=cur){
            k-=cur;
            continue;
        }
        return reali[b][zpos[occ][k]];
    }
    return -1;
}
struct mcts{
    static constexpr int node_limit=1100000;
    static constexpr int max_nodes=node_limit+2;
    static constexpr int chkstride=32;
    static constexpr int midlim=40;
    static constexpr int middep=2;
    struct node{
        int pari,mv;
        float win;
        int vis;
        int child[81];
        unsigned char untried[81],child_sz=0,untried_sz=0,turn=0;
        node(int p,int m,const tictactoe &st):pari(p),mv(m),win(0),vis(0),turn(st.turn){
            untried_sz=st.getvalidfast(untried);
        }
        node()=default;
    }tree[max_nodes];
    splitmix64 sp=splitmix64(chrono::steady_clock::now().time_since_epoch().count());
    int sz=0,it=0,root=-1;
    float time_limit=0.096;
    inline int probe(tictactoe st,int dep,int a,int b){
        if(st.lw)return st.lw==3?0:-1;
        if(dep==0)return 0;
        int arr[81],cnt=st.getvalidfast(arr),best=-1;
        for(int i = 0; i < cnt; i++){
            tictactoe nxt=st;
            nxt.apply(arr[i]);
            int val=-probe(nxt,dep-1,-b,-a);
            if(val>best)best=val;
            if(val>a)a=val;
            if(a>=b||best==1)break;
        }
        return best;
    }
    inline void reset(const tictactoe &st){
        root=0;
        sz=1;
        tree[0]=node(-1,-1,st);
    }
    inline void recycle(){
        if(root<=0||root*8<=sz)return;
        static int ord[max_nodes],pos[max_nodes],rev[max_nodes];
        int cnt=1;
        ord[0]=root;
        pos[root]=root;
        for(int i = 0; i < cnt; i++){
            int v=ord[i];
            for(int j = 0; j < tree[v].child_sz; j++){
                int c=tree[v].child[j];
                pos[c]=c;
                ord[cnt++]=c;
            }
        }
        for(int i = 0; i < cnt; i++)rev[i]=-1;
        for(int i = 0; i < cnt; i++)rev[ord[i]]=ord[i];
        for(int i = 0; i < cnt; i++){
            int need=ord[i],cur=pos[need];
            if(cur==i)continue;
            int oi=rev[i],oc=rev[cur];
            swap(tree[i],tree[cur]);
            rev[i]=oc;
            pos[oc]=i;
            rev[cur]=oi;
            if(oi!=-1)pos[oi]=cur;
        }
        for(int i = 0; i < cnt; i++){
            if(i==0)tree[i].pari=-1;
            else tree[i].pari=pos[tree[i].pari];
            for(int j = 0; j < tree[i].child_sz; j++){
                tree[i].child[j]=pos[tree[i].child[j]];
            }
        }
        root=0;
        sz=cnt;
    }
    inline void applyreal(int mv,const tictactoe &st){
        if(root==-1){
            reset(st);
            return;
        }
        for(int i = 0; i < tree[root].child_sz; i++){
            int c=tree[root].child[i];
            if(tree[c].mv==mv){
                root=c;
                tree[root].pari=-1;
                return;
            }
        }
        for(int i = 0; i < tree[root].untried_sz; i++){
            if(tree[root].untried[i]==mv&&sz<node_limit){
                tree[sz]=node(root,mv,st);
                swap(tree[root].untried[i],tree[root].untried[tree[root].untried_sz-1]);
                tree[root].untried_sz--;
                tree[root].child[tree[root].child_sz++]=sz;
                root=sz++;
                tree[root].pari=-1;
                return;
            }
        }
        reset(st);
    }
    inline int getbest(const tictactoe &cur){
        auto st=chrono::steady_clock::now();
        auto ed=st+chrono::duration_cast<chrono::steady_clock::duration>(chrono::duration<float>(time_limit));
        if(root==-1)reset(cur);
        if(root>0&&sz>node_limit*9/10)recycle();
        it=0;
        while(1){
            if(it&&((it&(chkstride-1))==0)){
                if(chrono::steady_clock::now()>=ed)break;
            }
            it++;
            int nodei=root;
            tictactoe stt=cur;
            while(tree[nodei].untried_sz==0&&tree[nodei].child_sz){
                float best=-1e9;
                int id=-1;
                float vval=0.4f*slg[tree[nodei].vis];
                for(int i = 0; i < tree[nodei].child_sz; i++){
                    int c=tree[nodei].child[i],vis=tree[c].vis;
                    float val=tree[c].win*iv[vis]+vval*rsq[vis];
                    if(val>best){
                        best=val;
                        id=c;
                    }
                }
                nodei=id;
                stt.apply(tree[nodei].mv);
            }
            if(tree[nodei].untried_sz&&stt.lw==0&&sz<node_limit){
                int idx=sp()%tree[nodei].untried_sz,mv=tree[nodei].untried[idx];
                stt.apply(mv);
                tree[sz++]=node(nodei,mv,stt);
                swap(tree[nodei].untried[idx],tree[nodei].untried[tree[nodei].untried_sz-1]);
                tree[nodei].untried_sz--;
                tree[nodei].child[tree[nodei].child_sz++]=sz-1;
                nodei=sz-1;
            }
            while(stt.lw==0){
                stt.apply(getrandvalid(stt,sp));
            }
            float res=0;
            if(stt.lw==1)res=1;
            else if(stt.lw==2)res=0;
            else res=0.5;
            int v=nodei;
            while(v!=-1){
                tree[v].vis++;
                if(tree[v].pari!=-1){
                    if(tree[v].turn!=1)tree[v].win+=res;
                    else tree[v].win+=(1-res);
                }
                v=tree[v].pari;
            }
        }
        int bestmv=-1;
        float bestrate=-1;
        float bestwr=0;
        if(cur.getleft()<=midlim){
            int arr[81],cnt=cur.getvalidfast(arr),rvis[81]={},bestchk=-2;
            float rwr[81]={};
            for(int i = 0; i < tree[root].child_sz; i++){
                int c=tree[root].child[i],mv=tree[c].mv;
                rvis[mv]=tree[c].vis;
                rwr[mv]=tree[c].vis?tree[c].win*iv[tree[c].vis]:0;
            }
            for(int i = 0; i < cnt; i++){
                int mv=arr[i];
                tictactoe nxt=cur;
                nxt.apply(mv);
                int chk=-probe(nxt,middep-1,-1,1);
                if(chk>bestchk||(chk==bestchk&&rvis[mv]>bestrate)){
                    bestchk=chk;
                    bestrate=rvis[mv];
                    bestmv=mv;
                    bestwr=rvis[mv]?rwr[mv]:(chk==1?1:chk==0?0.5f:0);
                }
            }
            cerr<<"MCTS iters: "<<it<<", nodes: "<<sz<<"\nwinrate: "<<fixed<<setprecision(6)<<bestwr<<'\n';
            return bestmv;
        }
        for(int i = 0; i < tree[root].child_sz; i++){
            int c=tree[root].child[i];
            float val=tree[c].vis;
            if(val>bestrate){
                bestrate=val;
                bestmv=tree[c].mv;
                bestwr=tree[c].vis?tree[c].win*iv[tree[c].vis]:0;
            }
        }
        cerr<<"MCTS iters: "<<it<<", nodes: "<<sz<<"\nwinrate: "<<fixed<<setprecision(6)<<bestwr<<'\n';
        return bestmv;
    }
}solver;
int h[8]={0x007,0x38,0x1C0,0x49,0x92,0x124,0x111,0x54};
signed main(){
    for(int i = 0; i < 512; i++){
        for(auto &t:h){
            if((i&t)==t)win[i]=1;
        }
        for(int j = 0; j < 9; j++){
            if(!((i>>j)&1))zpos[i][zcnt[i]++]=j;
        }
    }
    for(int i = 1; i < 1100002; i++){
        iv[i]=1.0f/i;
        rsq[i]=1.0f/sqrt((double)i);
        slg[i]=sqrt(log((double)i));
    }
    for(int i = 0; i < 81; i++){
        int r=i/9,c=i%9;
        mi[i]=r/3*3+c/3;
        li[i]=r%3*3+c%3;
        reali[mi[i]][li[i]]=i;
    }
    ios::sync_with_stdio(0);cin.tie(0);
    tictactoe game;
    while(1){
        globalturns++;
        int oprow,opcol;
        cin>>oprow>>opcol;
        if(globalturns==1){
            solver.time_limit=0.996;
        }
        else{
            solver.time_limit=0.096;
        }
        if(oprow!=-1&&opcol!=-1){
            game.apply(oprow*9+opcol);
            solver.applyreal(oprow*9+opcol,game);
        }
        int a,b,c;
        cin>>a;
        for(int i = 0; i < a; i++)cin>>b>>c;
        int mv=solver.getbest(game);
        game.apply(mv);
        solver.applyreal(mv,game);
        cout<<mv/9<<' '<<mv%9<<endl;
    }
}