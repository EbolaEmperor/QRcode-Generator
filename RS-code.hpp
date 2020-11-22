#include<algorithm>

namespace RScode
{
    unsigned short flog[256];   //a^i 对应的二进制数
    unsigned short iflog[256];   //二进制数对应的a的幂次

    //GP(2^8)的本原多项式：x^8 + x^4 + x^3 + x^2 + 1
    //285 = BIN(100011101), 是本原多项式的系数
    namespace GaloisSP{int mx;}
    struct Galois
    {
        unsigned short x;    //在Galois域中，这个数可以表示为a^x，x=65535表示这个数为0
        Galois(unsigned short _x=0){x=iflog[_x];}    //将一个二进制数转换为Galois域中的a^x表示法
        unsigned short toint(){return x==65535?0:flog[x];}
        Galois operator + (const Galois &b)
        {
            if(x==65535) return b;
            else if(b.x==65535) return (*this);
            else return Galois(flog[x]^flog[b.x]);
        }
        Galois operator - (const Galois &b)
        {
            return (*this)+b;
        }
        Galois operator * (const Galois &b)
        {
            if(x==65535||b.x==65535)
                return Galois(0);
            Galois res;
            res.x=(x+b.x)%GaloisSP::mx;
            return res;
        }
        Galois operator / (const Galois &b)
        {
            if(x==65535) return Galois(0);
            Galois res;
            res.x=(x-b.x+GaloisSP::mx)%GaloisSP::mx;
            return res;
        }
    };

    void Galois_init(int origin,int len)
    //初始化一个Galois域: GP(2^len),  其中origin是本原多项式系数的二进制表示
    {
        flog[0]=1;
        iflog[1]=0;
        iflog[0]=65535;  //65535表示0值，此时不能被表示为a^x的形式
        for(int i=1;i<(1<<len)-1;i++)
        {
            flog[i]=flog[i-1]<<1;
            if(flog[i]>>len) flog[i]^=origin;
            iflog[flog[i]]=i;
        }
        flog[(1<<len)-1]=0;
        GaloisSP::mx=(1<<len)-1;
    }

    Galois* Gauss(Galois** A,int m,int n,bool& fail)
    //高斯消元求解Galois域中的线性方程组
    {
        Galois* ans;
        for(int i=0;i<n;i++)
        {
            int p=i;
            while(p<m&&A[p][i].x==65535) p++;
            if(p==m) return fail=1,ans;            
            if(p!=i)
                for(int j=0;j<=n;j++)
                    std::swap(A[i][j],A[p][j]);
            for(int j=0;j<m;j++)
            {
                if(i==j||A[j][i].x==65535) continue;
                Galois t=A[j][i]/A[i][i];
                for(int k=i;k<=n;k++)
                    A[j][k]=A[j][k]-t*A[i][k];
            }
        }
        ans=new Galois[n];
        for(int i=0;i<n;i++)
            ans[i]=A[i][n]/A[i][i];
        return fail=0,ans;
    }
    Galois* Gauss(Galois** A,int n,bool& fail)
    {
        return Gauss(A,n,n,fail);
    }

    unsigned short* encode(unsigned short* msg,int msg_len,int rscode_len)
    //生成RS纠错码
    //注意信息的排列顺序是Msg=(mn,...,m2,m1)，RS码的排列顺序是RSC=(Qk,...,Q2,Q1)，因此要注意数组翻转问题
    {
        Galois** A;
        A=new Galois* [rscode_len];
        for(int i=0;i<rscode_len;i++)
            A[i]=new Galois[rscode_len+1];
        for(int i=0;i<rscode_len;i++)
        {
            Galois t,w(1);t.x=i;
            for(int j=0;j<rscode_len;j++)
                A[i][j]=w,w=w*t;
            A[i][rscode_len]=Galois(0);
            for(int j=0;j<msg_len;j++)
            {
                A[i][rscode_len]=A[i][rscode_len]+w*Galois(msg[msg_len-1-j]);
                w=w*t;
            }
        }
        bool fail;  //事实上，这里不可能fail
        Galois* ans=Gauss(A,rscode_len,fail);
        for(int i=0;i<rscode_len;i++)
            delete A[i];
        delete A;
        unsigned short* res=new unsigned short[rscode_len];
        for(int i=0;i<rscode_len;i++)
            res[i]=ans[rscode_len-1-i].toint();
        delete ans;
        return res;
    }

    bool RScheck(unsigned short* msg,int msg_len,unsigned short* rscode,int rscode_len)
    //检查数据与RS纠错码是否吻合，同样要注意数组翻转问题
    {
        for(int i=0;i<rscode_len;i++)
        {
            Galois t,w(1),s(0);t.x=i;
            for(int j=0;j<rscode_len;j++)
                s=s+w*Galois(rscode[rscode_len-1-j]),w=w*t;
            for(int j=0;j<msg_len;j++)
                s=s+w*Galois(msg[msg_len-1-j]),w=w*t;
            if(s.x!=65535) return 0;
        }
        return 1;
    }

    struct Poly  //系数为Galois域中元素的多项式
    {
        int n;
        Galois *c;
        Poly(){n=0;}
        Poly(int _n)
        {
            n=_n;
            c=new Galois[n];
        }
        Galois get(Galois x)
        {
            Galois res(0),w(1);
            for(int i=0;i<n;i++)
                res=res+w*c[i],w=w*x;
            return res;
        }
    };

    unsigned short* decode(unsigned short* codeword,int code_len,int error_num,bool& fail)
    //RS纠错，这里采用经典的Peterson-Gorenstein-Zierler decoder算法，注意信息序列的翻转问题
    {
        unsigned short* corrected;
        corrected=new unsigned short[code_len];
        std::memcpy(corrected,codeword,sizeof(unsigned short)*(code_len));
        
        //第一步：计算典型值
        Poly r(code_len);
        for(int i=0;i<code_len;i++)
            r.c[i]=Galois(codeword[code_len-1-i]);
        Galois* S;
        int rsc_len=error_num*2;
        S=new Galois[rsc_len];
        Galois w(1),a;a.x=1;
        for(int i=0;i<rsc_len;i++)
            S[i]=r.get(w),w=w*a;
        bool allright=1;
        for(int i=0;i<rsc_len;i++)
            if(S[i].toint()) allright=0;
        if(allright) return fail=0,corrected;
        
        //第二步：计算定位多项式，并确定错误位置
        Galois** A;
        A=new Galois* [error_num];
        for(int i=0;i<error_num;i++)
            A[i]=new Galois[error_num+1];
        for(int i=0;i<error_num;i++)
            for(int j=0;j<=error_num;j++)
                A[i][j]=S[i+j];
        Galois* lam=Gauss(A,error_num,fail);
        for(int i=0;i<error_num;i++)
            delete A[i];
        delete A;
        if(fail) return corrected;  //错误太多

        Poly Lambda(error_num+1);
        Lambda.c[0]=1;
        for(int i=1;i<=error_num;i++)
            Lambda.c[i]=lam[error_num-i];
        delete lam;
        Galois* errorp;
        int cnt=0;
        for(int i=0;i<code_len;i++)
        {
            Galois tmp;
            tmp.x=GaloisSP::mx-i;
            if(Lambda.get(tmp).toint()==0)
                cnt++;
        }
        if(cnt>error_num) return fail=1,corrected;  //错误太多

        errorp=new Galois[cnt];
        for(int i=0,cur=0;i<code_len;i++)
        {
            Galois tmp;
            tmp.x=GaloisSP::mx-i;
            if(Lambda.get(tmp).toint()==0)
                errorp[cur++].x=i;
        }
        delete Lambda.c;

        //第三步：计算错误值
        Galois** B;
        B=new Galois* [rsc_len];
        for(int i=0;i<rsc_len;i++)
            B[i]=new Galois[cnt+1];
        for(int j=0;j<cnt;j++)
        {
            Galois w(1);
            for(int i=0;i<rsc_len;i++)
                B[i][j]=w,w=w*errorp[j];
        }
        for(int i=0;i<rsc_len;i++)
            B[i][cnt]=S[i];
        Galois* errorv=Gauss(B,rsc_len,cnt,fail);
        
        for(int i=0;i<rsc_len;i++)
            delete B[i];
        delete B;
        delete S;
        
        for(int i=0;i<cnt;i++)
            corrected[code_len-1-errorp[i].x]=(r.c[errorp[i].x]-errorv[i]).toint();
        delete errorp;
        delete errorv;
        return corrected;
    }

    unsigned short* correct(unsigned short* codeword,int code_len,int rsc_len,bool& fail)
    {
        for(int i=rsc_len/2;i>=1;i--)
        {
            bool fail=0;
            unsigned short* res=decode(codeword,code_len,i,fail);
            if(!fail) return res;
            else delete res;
        }
        return fail=1,codeword;
    }
}