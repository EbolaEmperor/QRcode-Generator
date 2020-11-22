#include<algorithm>

namespace BCHcode
{
    const bool G[]={1,1,1,0,1,1,0,0,1,0,1};
    bool* encode(bool* msg)
    {
        bool* bch;
        bch=new bool[15];
        for(int i=0;i<15;i++) bch[i]=0;
        for(int i=0;i<5;i++)
            bch[10+i]=msg[4-i];
        for(int p=4;p>=0;p--)
        {
            if(!bch[10+p]) continue;
            for(int i=0;i<=10;i++)
                bch[p+i]^=G[i];
        }
        for(int i=0;i<5;i++)
            bch[10+i]=msg[4-i];
        std::reverse(bch,bch+15);
        return bch;
    }
}