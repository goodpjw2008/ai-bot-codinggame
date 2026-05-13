#include <bits/stdc++.h>
#pragma GCC optimize("Ofast,unroll-loops,inline")
#pragma GCC target("avx2,avx,fma,sse,sse2,sse3,sse4,ssse3")
#define x first
#define y second
using namespace std;
using pii = pair<int,int>;
int me,mxdep=12;
using ld=float;
ld medalvalue[3][4];
using pli=pair<ld,int>;
ld gold=3,silver=1,bronze=-0.001;
struct trackint{
    int stk[1234],top;
    inline void push_back(int x){
        stk[++top]=x;
    }
    inline void pop_back(){
        top--;
    }
    inline int back(){
        return stk[top];
    }
};
struct trackpii{
    pii stk[1234];
    int top;
    inline void push_back(pii x){
        stk[++top]=x;
    }
    inline void pop_back(){
        top--;
    }
    inline pii back(){
        return stk[top];
    }
};
struct tracktuple{
    tuple<int,int,int> stk[1234];
    int top;
    inline void push_back(tuple<int,int,int> x){
        stk[++top]=x;
    }
    inline void pop_back(){
        top--;
    }
    inline tuple<int,int,int> back(){
        return stk[top];
    }
};
inline ld sigmoid(ld x){
    return 1.0/(1.0+exp(-x));
}
struct huddle{
    int p[3],pstuntimer[3],dp[31];
    bool reset;
    trackint pr[3],prstuntimer[3];
    ld temp=3;
    string mp;
    inline ld cost(int a){
        return -dp[p[a]]-pstuntimer[a];
    }
    inline ld prob(int a, int b){
        return sigmoid((cost(a)-cost(b))/temp);
    }
    inline ld score(int mee, int i){
        if(reset)return log(medalvalue[mee][i]);
        if(mp=="GAME_OVER"||p[0]>=mp.size()-1||p[1]>=mp.size()-1||p[2]>=mp.size()-1){
            if(p[mee]>=p[(mee+1)%3]&&p[mee]>=p[(mee+2)%3])return log(gold+medalvalue[mee][i]);
            else if(p[mee]<p[(mee+1)%3]&&p[mee]<p[(mee+2)%3])return log(bronze+medalvalue[mee][i]);
            else return log(silver+medalvalue[mee][i]);
        }
        ld pgold=prob(mee,(mee+1)%3)*prob(mee,(mee+2)%3),pbronze=prob((mee+1)%3,mee)*prob((mee+2)%3,mee),psilver=1-pgold-pbronze;
        return log(medalvalue[mee][i]+pgold*gold+pbronze*bronze+psilver*silver);
    }
    inline void move(int &tp, int &tstuntimer, trackint &tpr, trackint &tprstuntimer, int mv){
        tpr.push_back(tp);
        tprstuntimer.push_back(tstuntimer);
        if(mp=="GAME_OVER"||p[0]>=mp.size()-1||p[1]>=mp.size()-1||p[2]>=mp.size()-1)return;
        if(tstuntimer){
            tstuntimer--;
            return;
        }
        if(mv!=3){
            for(int i = 0; i <= mv; i++){
                if(tp==mp.size()-1)break;
                tp++;
                if(tp<mp.size()&&mp[tp]=='#'){
                    tstuntimer=3;
                    break;
                }
            }
        }
        else{
            tp+=2;
            if(tp<mp.size()&&mp[tp]=='#'){
                tstuntimer=3;
            }
        }
        tp=min(tp,(int)mp.size()-1);
    }
    inline void undo(int &p1, int &stuntimer, trackint &pr1, trackint &pr1stuntimer){
        p1=pr1.back();
        stuntimer=pr1stuntimer.back();
        pr1.pop_back();
        pr1stuntimer.pop_back();
    }
    inline void init(string inmp, int inp1, int inp2, int inp3, int inp1stuntimer, int inp2stuntimer, int inp3stuntimer){
        mp=inmp;
        if(inmp=="GAME_OVER")reset=1;
        else reset=0;
        p[0]=inp1;
        p[1]=inp2;
        p[2]=inp3;
        pstuntimer[0]=inp1stuntimer;
        pstuntimer[1]=inp2stuntimer;
        pstuntimer[2]=inp3stuntimer;
        for(int i = 0; i < 3; i++){
            pr[i].top=0;
            prstuntimer[i].top=0;
        }
        dp[29]=0;
        for(int i = 28; i >= 0; i--){
            dp[i]=1e9;
            if(i<29){
                if(mp[i+1]!='#')dp[i]=min(dp[i],dp[i+1]+1);
                else dp[i]=min(dp[i],dp[i+1]+4);
            }
            if(i<28){
                if(mp[i+2]!='#')dp[i]=min(dp[i],dp[i+2]+1);
                else dp[i]=min(dp[i],dp[i+2]+4);
            }
            if(i<27){
                if(mp[i+1]=='#')dp[i]=min(dp[i],dp[i+1]+4);
                else if(mp[i+2]=='#')dp[i]=min(dp[i],dp[i+2]+4);
                else if(mp[i+3]=='#')dp[i]=min(dp[i],dp[i+3]+4);
                else dp[i]=min(dp[i],dp[i+3]+1);
            }
        }
    }
}hud;
int dx[4]={-1,0,1,0},dy[4]={0,1,0,-1};
struct archery{
    int curtime;
    bool reset;
    pii pos[3];
    string mp;
    trackpii pr[3];
    ld temp=150;
    inline ld cost(int a){
        return -pos[a].x*pos[a].x-pos[a].y*pos[a].y;
    }
    inline ld prob(int a, int b){
        return sigmoid((cost(a)-cost(b))/temp);
    }
    inline ld score(int mee, int i){
        if(reset)return log(medalvalue[mee][i]);
        int dst1=pos[mee].x*pos[mee].x+pos[mee].y*pos[mee].y,dst2=pos[(mee+1)%3].x*pos[(mee+1)%3].x+pos[(mee+1)%3].y*pos[(mee+1)%3].y,dst3=pos[(mee+2)%3].x*pos[(mee+2)%3].x+pos[(mee+2)%3].y*pos[(mee+2)%3].y;
        if(curtime>=mp.size()){
            if(dst1<=dst2&&dst1<=dst3)return log(gold+medalvalue[mee][i]);
            else if(dst1>dst2&&dst1>dst3)return log(bronze+medalvalue[mee][i]);
            else return log(silver+medalvalue[mee][i]);
        }
        ld pgold=prob(mee,(mee+1)%3)*prob(mee,(mee+2)%3),pbronze=prob((mee+1)%3,mee)*prob((mee+2)%3,mee),psilver=1-pgold-pbronze;
        return log(medalvalue[mee][i]+pgold*gold+pbronze*bronze+psilver*silver);
    }
    inline void move(pii &curpos, trackpii &pr1, int mv){
        pr1.push_back(curpos);
        if(mp=="GAME_OVER"||curtime>=mp.size())return;
        curpos.x+=dx[mv]*(mp[curtime]-'0');
        curpos.y+=dy[mv]*(mp[curtime]-'0');
        curpos.x=max(-20,min(20,curpos.x));
        curpos.y=max(-20,min(20,curpos.y));
    }
    inline void undo(pii &curpos, trackpii &pr1){
        curpos=pr1.back();
        pr1.pop_back();
    }
    inline void init(string inmp, pii inp1, pii inp2, pii inp3){
        mp=inmp;
        if(inmp=="GAME_OVER")reset=1;
        else reset=0,curtime=0;
        pos[0]=inp1;
        pos[1]=inp2;
        pos[2]=inp3;
        for(int i = 0; i < 3; i++){
            pr[i].top=0;
        }
    }
}arc;
string moves[4]={"LEFT","DOWN","RIGHT","UP"};
struct skate{
    int p[3],r[3],pstuntimer[3],turnsleft;//pos,risk,pstuntimer
    bool reset;
    trackint pr[3];
    tracktuple prstuntimer[3],prr[3];
    ld temp=8;
    inline ld cost(int a){
        return p[a]-r[a]*2-pstuntimer[a]*2;
    }
    inline ld prob(int a, int b){
        return sigmoid((cost(a)-cost(b))/temp);
    }
    inline ld score(int mee, int i){
        if(reset)return log(medalvalue[mee][i]);
        if(turnsleft<=0){
            if(p[mee]>=p[(mee+1)%3]&&p[mee]>=p[(mee+2)%3])return log(gold+medalvalue[mee][i]);
            else if(p[mee]<p[(mee+1)%3]&&p[mee]<p[(mee+2)%3])return log(bronze+medalvalue[mee][i]);
            else return log(silver+medalvalue[mee][i]);
        }
        ld pgold=prob(mee,(mee+1)%3)*prob(mee,(mee+2)%3),pbronze=prob((mee+1)%3,mee)*prob((mee+2)%3,mee),psilver=1-pgold-pbronze;
        return log(medalvalue[mee][i]+pgold*gold+pbronze*bronze+psilver*silver);
    }
    string mp;
    inline void move(int &idx, int &tp, int &trisk, int &tstuntimer, trackint &tpr, tracktuple &tprrisk, tracktuple &tprstuntimer, int mv, int dep){
        tpr.push_back(tp);
        tprrisk.push_back({r[0],r[1],r[2]});
        tprstuntimer.push_back({pstuntimer[0],pstuntimer[1],pstuntimer[2]});
        if(mp=="GAME_OVER"||turnsleft<=0)return;
        if(tstuntimer){
            tstuntimer--;
            return;
        }
        if(dep>2){
            int r=rand()%4;
            if(r==0){
                tp++;
                trisk=max(0,trisk-1);
            }
            else if(r==1){
                tp+=2;
            }
            else if(r==2){
                tp+=2;
                trisk++;
                if(trisk>=5){
                    tstuntimer=2;
                    trisk=0;
                }
            }
            else{
                tp+=3;
                trisk+=2;
                if(trisk>=5){
                    tstuntimer=2;
                    trisk=0;
                }
            }
        }
        else{
            if(moves[mv][0]==mp[0]){
                tp++;
                trisk=max(0,trisk-1);
            }
            else if(moves[mv][0]==mp[1]){
                tp+=2;
            }
            else if(moves[mv][0]==mp[2]){
                tp+=2;
                trisk++;
                if(trisk>=5){
                    tstuntimer=2;
                    trisk=0;
                }
            }
            else{
                tp+=3;
                trisk+=2;
                if(trisk>=5){
                    tstuntimer=2;
                    trisk=0;
                }
            }
        }
        if(idx==(me+1)%3){
            if(tp%10==p[me]%10){
                if(!pstuntimer[idx])r[idx]+=2;
                if(r[idx]>=5){
                    pstuntimer[idx]=2;
                    r[idx]=0;
                }
                if(!pstuntimer[me])r[me]+=2;
                if(r[me]>=5){
                    pstuntimer[me]=2;
                    r[me]=0;
                }
            }
        }
        else if(idx!=me){
            if(tp%10==p[me]%10&&tp%10==p[(me+1)%3]%10){
                if(!pstuntimer[idx])r[idx]+=2;
                if(r[idx]>=5){
                    pstuntimer[idx]=2;
                    r[idx]=0;
                }
            }
            else if(tp%10==p[me]%10){
                if(!pstuntimer[idx])r[idx]+=2;
                if(r[idx]>=5){
                    pstuntimer[idx]=2;
                    r[idx]=0;
                }
                if(!pstuntimer[me])r[me]+=2;
                if(r[me]>=5){
                    pstuntimer[me]=2;
                    r[me]=0;
                }
            }
            else if(tp%10==p[(me+1)%3]%10){
                if(!pstuntimer[idx])r[idx]+=2;
                if(r[idx]>=5){
                    pstuntimer[idx]=2;
                    r[idx]=0;
                }
                if(!pstuntimer[(me+1)%3])r[(me+1)%3]+=2;
                if(r[(me+1)%3]>=5){
                    pstuntimer[(me+1)%3]=2;
                    r[(me+1)%3]=0;
                }
            }
        }
    }
    inline void undo(int &tp, int &trisk, int &stuntimer, trackint &tpr, tracktuple &tprrisk, tracktuple &tprstuntimer){
        tp=tpr.back();
        tpr.pop_back();
        auto [r0,r1,r2]=tprrisk.back();
        tprrisk.pop_back();
        r[0]=r0,r[1]=r1,r[2]=r2;
        auto [s0,s1,s2]=tprstuntimer.back();
        tprstuntimer.pop_back();
        pstuntimer[0]=s0,pstuntimer[1]=s1,pstuntimer[2]=s2;
    }
    inline void init(string inmp, int inp1, int inp2, int inp3, int inp1risk, int inp2risk, int inp3risk, int inpturnsleft){
        mp=inmp;
        if(inmp=="GAME_OVER")reset=1;
        else reset=0;
        p[0]=inp1;
        p[1]=inp2;
        p[2]=inp3;
        if(inp1risk<0)pstuntimer[0]=-inp1risk,r[0]=0;
        else r[0]=inp1risk, pstuntimer[0]=0;
        if(inp2risk<0)pstuntimer[1]=-inp2risk,r[1]=0;
        else r[1]=inp2risk, pstuntimer[1]=0;
        if(inp3risk<0)pstuntimer[2]=-inp3risk,r[2]=0;
        else r[2]=inp3risk, pstuntimer[2]=0;
        turnsleft=inpturnsleft;
        for(int i = 0; i < 3; i++){
            pr[i].top=0;
            prr[i].top=0;
            prstuntimer[i].top=0;
        }
    }
}skt;
struct diving{
    int curtime,point[3],combo[3];
    string mp;
    bool reset;
    trackint prpoint[3],prcombo[3];
    ld temp=20;
    inline ld cost(int a){
        return point[a]+combo[a]*(mp.size()-curtime)/2.0+(mp.size()-curtime)*(mp.size()-curtime+1)/2.0;
    }
    inline ld prob(int a, int b){
        return sigmoid((cost(a)-cost(b))/temp);
    }
    inline ld score(int mee, int i){
        if(reset)return log(medalvalue[mee][i]);
        if(curtime>=mp.size()){
            if(point[mee]>=point[(mee+1)%3]&&point[mee]>=point[(mee+2)%3])return log(gold+medalvalue[mee][i]);
            else if(point[mee]<point[(mee+1)%3]&&point[mee]<point[(mee+2)%3])return log(bronze+medalvalue[mee][i]);
            else return log(silver+medalvalue[mee][i]);
        }
        ld pgold=prob(mee,(mee+1)%3)*prob(mee,(mee+2)%3),pbronze=prob((mee+1)%3,mee)*prob((mee+2)%3,mee),psilver=1-pgold-pbronze;
        return log(medalvalue[mee][i]+pgold*gold+pbronze*bronze+psilver*silver);
    }
    inline void move(int &point, int &combo, trackint &prpoint, trackint &prcombo, int mv){
        prpoint.push_back(point);
        prcombo.push_back(combo);
        if(curtime>=mp.size())return;
        if(moves[mv][0]==mp[curtime]){
            combo++;
            point+=combo;
        }
        else combo=0;
    }
    inline void undo(int &point, int &combo, trackint &prpoint, trackint &prcombo){
        point=prpoint.back();
        combo=prcombo.back();
        prpoint.pop_back();
        prcombo.pop_back();
    }
    inline void init(string inmp, int inp1, int inp2, int inp3, int inp1combo, int inp2combo, int inp3combo){
        mp=inmp;
        if(inmp=="GAME_OVER")reset=1;
        else reset=0,curtime=0;
        point[0]=inp1;
        point[1]=inp2;
        point[2]=inp3;
        combo[0]=inp1combo;
        combo[1]=inp2combo;
        combo[2]=inp3combo;
        for(int i = 0; i < 3; i++){
            prpoint[i].top=0;
            prcombo[i].top=0;
        }
    }
}divv;
ld etemp=0.5;
pli expectimax(int depth, int turn, ld alpha=-1e9, ld beta=1e9){
    if(mxdep==depth){
        ld ret=hud.score(me,0)+arc.score(me,1)+skt.score(me,2)+divv.score(me,3)-(hud.score((me+1)%3,0)+arc.score((me+1)%3,1)+skt.score((me+1)%3,2)+divv.score((me+1)%3,3)+hud.score((me+2)%3,0)+arc.score((me+2)%3,1)+skt.score((me+2)%3,2)+divv.score((me+2)%3,3))/2;
        return {ret,0};
    }
    if(turn==me){
        ld maxval=-1e9;
        int maxidx=0;
        /*
        for(int i = 0; i < 4; i++){
            hud.move(hud.p[turn],hud.pstuntimer[turn],hud.pr[turn],hud.prstuntimer[turn],i);
            arc.move(arc.pos[turn],arc.pr[turn],i);
            skt.move(turn,skt.p[turn],skt.r[turn],skt.pstuntimer[turn],skt.pr[turn],skt.prr[turn],skt.prstuntimer[turn],i);
            divv.move(divv.point[turn],divv.combo[turn],divv.prpoint[turn],divv.prcombo[turn],i);
            ld val=expectimax(depth+1,(turn+1)%3).x;
            if(val>maxval){
                maxval=val;
                maxidx=i;
            }
            hud.undo(hud.p[turn],hud.pstuntimer[turn],hud.pr[turn],hud.prstuntimer[turn]);
            arc.undo(arc.pos[turn],arc.pr[turn]);
            skt.undo(skt.p[turn],skt.r[turn],skt.pstuntimer[turn],skt.pr[turn],skt.prr[turn],skt.prstuntimer[turn]);
            divv.undo(divv.point[turn],divv.combo[turn],divv.prpoint[turn],divv.prcombo[turn]);
        }
        return {maxval,maxidx};
        */
        pli lst[4]={{0,0},{0,1},{0,2},{0,3}};
        for(int i = 0; i < 4; i++){
            hud.move(hud.p[turn],hud.pstuntimer[turn],hud.pr[turn],hud.prstuntimer[turn],i);
            arc.move(arc.pos[turn],arc.pr[turn],i);
            skt.move(turn,skt.p[turn],skt.r[turn],skt.pstuntimer[turn],skt.pr[turn],skt.prr[turn],skt.prstuntimer[turn],i,depth);
            divv.move(divv.point[turn],divv.combo[turn],divv.prpoint[turn],divv.prcombo[turn],i);
            lst[i]={hud.score(me,0)+arc.score(me,1)+skt.score(me,2)+divv.score(me,3),i};
            hud.undo(hud.p[turn],hud.pstuntimer[turn],hud.pr[turn],hud.prstuntimer[turn]);
            arc.undo(arc.pos[turn],arc.pr[turn]);
            skt.undo(skt.p[turn],skt.r[turn],skt.pstuntimer[turn],skt.pr[turn],skt.prr[turn],skt.prstuntimer[turn]);
            divv.undo(divv.point[turn],divv.combo[turn],divv.prpoint[turn],divv.prcombo[turn]);
        }
        sort(lst,lst+4,greater<pli>());
        for(int i = 0; i < 4-min(2,depth/3); i++){
            hud.move(hud.p[turn],hud.pstuntimer[turn],hud.pr[turn],hud.prstuntimer[turn],lst[i].y);
            arc.move(arc.pos[turn],arc.pr[turn],lst[i].y);
            skt.move(turn,skt.p[turn],skt.r[turn],skt.pstuntimer[turn],skt.pr[turn],skt.prr[turn],skt.prstuntimer[turn],lst[i].y,depth);
            divv.move(divv.point[turn],divv.combo[turn],divv.prpoint[turn],divv.prcombo[turn],lst[i].y);
            ld val=expectimax(depth+1,(turn+1)%3).x;
            if(val>maxval){
                maxval=val;
                maxidx=lst[i].y;
            }
            alpha=max(alpha,val);
            hud.undo(hud.p[turn],hud.pstuntimer[turn],hud.pr[turn],hud.prstuntimer[turn]);
            arc.undo(arc.pos[turn],arc.pr[turn]);
            skt.undo(skt.p[turn],skt.r[turn],skt.pstuntimer[turn],skt.pr[turn],skt.prr[turn],skt.prstuntimer[turn]);
            divv.undo(divv.point[turn],divv.combo[turn],divv.prpoint[turn],divv.prcombo[turn]);
            if(alpha>=beta)break;
        }
        return {maxval,maxidx};
    }
    else{
        pli lst[4]={{0,0},{0,1},{0,2},{0,3}};
        for(int i = 0; i < 4; i++){
            hud.move(hud.p[turn],hud.pstuntimer[turn],hud.pr[turn],hud.prstuntimer[turn],i);
            arc.move(arc.pos[turn],arc.pr[turn],i);
            skt.move(turn,skt.p[turn],skt.r[turn],skt.pstuntimer[turn],skt.pr[turn],skt.prr[turn],skt.prstuntimer[turn],i,depth);
            divv.move(divv.point[turn],divv.combo[turn],divv.prpoint[turn],divv.prcombo[turn],i);
            lst[i]={hud.score(turn,0)+arc.score(turn,1)+skt.score(turn,2)+divv.score(turn,3),i};
            hud.undo(hud.p[turn],hud.pstuntimer[turn],hud.pr[turn],hud.prstuntimer[turn]);
            arc.undo(arc.pos[turn],arc.pr[turn]);
            skt.undo(skt.p[turn],skt.r[turn],skt.pstuntimer[turn],skt.pr[turn],skt.prr[turn],skt.prstuntimer[turn]);
            divv.undo(divv.point[turn],divv.combo[turn],divv.prpoint[turn],divv.prcombo[turn]);
        }
        sort(lst,lst+4,greater<pli>());
        /*
        for(int i = 0; i < 2; i++){
            hud.move(hud.p[turn],hud.pstuntimer[turn],hud.pr[turn],hud.prstuntimer[turn],lst[i].y);
            arc.move(arc.pos[turn],arc.pr[turn],lst[i].y);
            skt.move(turn,skt.p[turn],skt.r[turn],skt.pstuntimer[turn],skt.pr[turn],skt.prr[turn],skt.prstuntimer[turn],lst[i].y,depth);
            divv.move(divv.point[turn],divv.combo[turn],divv.prpoint[turn],divv.prcombo[turn],lst[i].y);
            if(turn==(me+2)%3){
                arc.curtime++;
                divv.curtime++;
                skt.turnsleft--;
            }
            tot+=expectimax(depth+1,(turn+1)%3).x;
            if(turn==(me+2)%3){
                arc.curtime--;
                divv.curtime--;
                skt.turnsleft++;
            }
            hud.undo(hud.p[turn],hud.pstuntimer[turn],hud.pr[turn],hud.prstuntimer[turn]);
            arc.undo(arc.pos[turn],arc.pr[turn]);
            skt.undo(skt.p[turn],skt.r[turn],skt.pstuntimer[turn],skt.pr[turn],skt.prr[turn],skt.prstuntimer[turn]);
            divv.undo(divv.point[turn],divv.combo[turn],divv.prpoint[turn],divv.prcombo[turn]);
        }
        */
        //softmax
        ld mn=1e9;
        for(int i = 0; i < 2; i++){
            hud.move(hud.p[turn],hud.pstuntimer[turn],hud.pr[turn],hud.prstuntimer[turn],lst[i].y);
            arc.move(arc.pos[turn],arc.pr[turn],lst[i].y);
            skt.move(turn,skt.p[turn],skt.r[turn],skt.pstuntimer[turn],skt.pr[turn],skt.prr[turn],skt.prstuntimer[turn],lst[i].y,depth);
            divv.move(divv.point[turn],divv.combo[turn],divv.prpoint[turn],divv.prcombo[turn],lst[i].y);
            if(turn==(me+2)%3){
                arc.curtime++;
                divv.curtime++;
                skt.turnsleft--;
            }
            ld t=expectimax(depth+1,(turn+1)%3,alpha,beta).x;
            mn=min(mn,t);
            beta=min(beta,t);
            if(turn==(me+2)%3){
                arc.curtime--;
                divv.curtime--;
                skt.turnsleft++;
            }
            hud.undo(hud.p[turn],hud.pstuntimer[turn],hud.pr[turn],hud.prstuntimer[turn]);
            arc.undo(arc.pos[turn],arc.pr[turn]);
            skt.undo(skt.p[turn],skt.r[turn],skt.pstuntimer[turn],skt.pr[turn],skt.prr[turn],skt.prstuntimer[turn]);
            divv.undo(divv.point[turn],divv.combo[turn],divv.prpoint[turn],divv.prcombo[turn]);
            if(alpha>=beta)break;
        }
        return {mn,0};
    }
}
int turns;
int main()
{
    ios::sync_with_stdio(0);
    srand(time(0));
    int player_idx,x;
    cin >> player_idx; cin.ignore();
    int nb_games;
    cin >> nb_games; cin.ignore();
    me=player_idx;
    // game loop
    while (1) {
        turns++;
        auto start = chrono::high_resolution_clock::now();
        for(int t = 0; t < 3; t++){
            cin>>x;
            for (int i = 0; i < 4; i++){
                medalvalue[t][i]=1/sqrt(sqrt(turns*turns*turns));
                cin>>x;
                medalvalue[t][i]+=gold*x;
                cin>>x;
                medalvalue[t][i]+=silver*x;
                cin>>x;
                medalvalue[t][i]+=bronze*x;
            }
        }
        int mxi=0,mxscore=-1;
        string gpu;
        int reg0,reg1,reg2,reg3,reg4,reg5,reg6;
        cin>>gpu>>reg0>>reg1>>reg2>>reg3>>reg4>>reg5>>reg6;
        hud.init(gpu,reg0,reg1,reg2,reg3,reg4,reg5);
        cin>>gpu>>reg0>>reg1>>reg2>>reg3>>reg4>>reg5>>reg6;
        arc.init(gpu,{reg0,reg1},{reg2,reg3},{reg4,reg5});
        cin>>gpu>>reg0>>reg1>>reg2>>reg3>>reg4>>reg5>>reg6;
        skt.init(gpu,reg0,reg1,reg2,reg3,reg4,reg5,reg6);
        cin>>gpu>>reg0>>reg1>>reg2>>reg3>>reg4>>reg5>>reg6;
        divv.init(gpu,reg0,reg1,reg2,reg3,reg4,reg5);
        cerr<<medalvalue[0][0]<<" "<<medalvalue[0][1]<<" "<<medalvalue[0][2]<<" "<<medalvalue[0][3]<<endl;
        cerr<<medalvalue[1][0]<<" "<<medalvalue[1][1]<<" "<<medalvalue[1][2]<<" "<<medalvalue[1][3]<<endl;
        cerr<<medalvalue[2][0]<<" "<<medalvalue[2][1]<<" "<<medalvalue[2][2]<<" "<<medalvalue[2][3]<<endl;
        for(int i = 0; i < 4; i++){
            hud.move(hud.p[me],hud.pstuntimer[me],hud.pr[me],hud.prstuntimer[me],i);
            arc.move(arc.pos[me],arc.pr[me],i);
            skt.move(me,skt.p[me],skt.r[me],skt.pstuntimer[me],skt.pr[me],skt.prr[me],skt.prstuntimer[me],i,0);
            divv.move(divv.point[me],divv.combo[me],divv.prpoint[me],divv.prcombo[me],i);
            cerr<<moves[i]<<": "<<hud.score(me,0)<<" "<<arc.score(me,1)<<" "<<skt.score(me,2)<<" "<<divv.score(me,3)<<endl;
            hud.undo(hud.p[me],hud.pstuntimer[me],hud.pr[me],hud.prstuntimer[me]);
            arc.undo(arc.pos[me],arc.pr[me]);
            skt.undo(skt.p[me],skt.r[me],skt.pstuntimer[me],skt.pr[me],skt.prr[me],skt.prstuntimer[me]);
            divv.undo(divv.point[me],divv.combo[me],divv.prpoint[me],divv.prcombo[me]);
        }
        cout << moves[expectimax(0,me).y] << endl;
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
        cerr << "Time taken: " << duration.count() << " milliseconds" << endl;
    }
}