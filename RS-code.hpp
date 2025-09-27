#include<algorithm>
#include "Galois.hpp"
#include "Matrix.hpp"

namespace RScode
{
    using namespace GaloisSP;

    unsigned short* encode(unsigned short* msg, int msg_len, int rscode_len)
    //生成RS纠错码
    //注意信息的排列顺序是Msg=(mn,...,m2,m1)，RS码的排列顺序是RSC=(Qk,...,Q2,Q1)，因此要注意数组翻转问题
    {
        Matrix<Galois> A(rscode_len, rscode_len);
        std::vector<Galois> b(rscode_len);
        
        for (int i = 0; i < rscode_len; i++)
        {
            Galois t, w(1);
            t.x = i;
            for (int j = 0; j < rscode_len; j++)
            {
                A(i, j) = w;
                w = w * t;
            }
            b[i] = Galois(0);
            for (int j = 0; j < msg_len; j++)
            {
                b[i] = b[i] + w * Galois(msg[msg_len - 1 - j]);
                w = w * t;
            }
        }
        auto ans = A.solve(b);
        
        unsigned short* res = new unsigned short[rscode_len];
        for (int i = 0; i < rscode_len; i++)
            res[i] = ans[rscode_len - 1 - i].toint();
        
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
        Poly() { n = 0; c = nullptr; }
        Poly(int _n)
        {
            n = _n;
            c = new Galois[n];
            for (int i = 0; i < n; i++)
                c[i] = Galois(0);
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
        Matrix<Galois> A(error_num, error_num);
        std::vector<Galois> b(error_num);
        
        for (int i = 0; i < error_num; i++)
        {
            for (int j = 0; j < error_num; j++)
                A(i, j) = S[i + j];
            b[i] = S[i + error_num];
        }
        
        std::vector<Galois> lam;
        try {
            lam = A.solve(b);
        }
        catch (const std::exception& e) {
            fail = true;
            return corrected;  //错误太多
        }

        Poly Lambda(error_num + 1);
        Lambda.c[0] = Galois(1);
        for (int i = 1; i <= error_num; i++)
            Lambda.c[i] = lam[error_num - i];
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
        Matrix<Galois> B(rsc_len, cnt);
        std::vector<Galois> b_vec(rsc_len);
        
        for (int j = 0; j < cnt; j++)
        {
            Galois w(1);
            for (int i = 0; i < rsc_len; i++)
            {
                B(i, j) = w;
                w = w * errorp[j];
            }
        }
        for (int i = 0; i < rsc_len; i++)
            b_vec[i] = S[i];
        
        std::vector<Galois> errorv;
        try {
            errorv = B.solve(b_vec);
        }
        catch (const std::exception& e) {
            fail = true;
            delete S;
            delete errorp;
            return corrected;
        }
        
        delete S;
        
        for (int i = 0; i < cnt; i++)
            corrected[code_len - 1 - errorp[i].x] = (r.c[errorp[i].x] - errorv[i]).toint();
        delete errorp;
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