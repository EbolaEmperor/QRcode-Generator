#include<iostream>
#include<cstdio>
#include<cstring>
#include "RS-code.hpp"
#include "QR-matrix.hpp"
#include "BCH-code.hpp"
#include "alignmentList.hpp"
#include "versionInfoList.hpp"
#include "blockList.hpp"

namespace QRcode
{
    void addcode(bool* s,int n,const int x)
    {
        for(int i=0;i<n;i++)
            s[i]=(x>>(n-1-i))&1;
    }

    void addcode(bool* s,const char* code)
    {
        int n=strlen(code);
        for(int i=0;i<n;i++)
            s[i]=code[i]-'0';
    }

    unsigned short* encode(int indicator_bits,int bits_len,unsigned char* input)
    {
        static bool* bits;
        static unsigned short* code;
        int code_len=1+indicator_bits/8+bits_len;
        bits=new bool[8*code_len];
        code=new unsigned short[code_len];
        addcode(bits,"0100");
        int n=0;
        while(input[n]) n++;
        for(int i=0;i<indicator_bits;i++)
            bits[3+indicator_bits-i]=(n>>i)&1;
        for(int i=0;i<n;i++)
        {
            int cur=3+indicator_bits+8*(i+1);
            for(int j=0;j<8;j++)
                bits[cur-j]=(input[i]>>j)&1;
        }
        int cur=4+indicator_bits+8*n;
        addcode(bits+cur,"0000");
        cur+=4;
        for(int i=0;i<bits_len-n;i++)
            if(i&1) addcode(bits+cur+i*8,"00010001");
            else addcode(bits+cur+i*8,"11101100");
        for(int i=0;i<code_len;i++)
        {
            code[i]=0;
            for(int j=0;j<8;j++)
                code[i]=(code[i]<<1)|bits[8*i+j];
        }
        return code;
    }

    unsigned short* get_data(unsigned short* code,int block_num,const int* block_len,int rsc_len,int tot_len)
    {
        unsigned short* codeblock[block_num];
        unsigned short* errorblock[block_num];
        for(int i=0,cur=0;i<block_num;i++)
        {
            codeblock[i]=new unsigned short[block_len[i]];
            for(int j=0;j<block_len[i];j++)
                codeblock[i][j]=code[cur++];
            errorblock[i]=RScode::encode(codeblock[i],block_len[i],rsc_len);
        }
        unsigned short* data;
        data=new unsigned short[tot_len];
        int it=0;
        for(int j=0;j<block_len[0];j++)
            for(int i=0;i<block_num;i++)
                data[it++]=codeblock[i][j];
        for(int i=0;i<block_num;i++)
            if(block_len[i]>block_len[0])
                data[it++]=codeblock[i][block_len[i]-1];
        for(int j=0;j<rsc_len;j++)
            for(int i=0;i<block_num;i++)
                data[it++]=errorblock[i][j];
        return data;
    }

    void workL(unsigned char* input,int n)
    {
        using namespace errLevelL;
        if(n>msgLen[39]-3)
        {
            puts("Message is too long!");
            puts("Tip.  Sorry, QR-code can't include your message.");
            std::cin.get();
            fflush(stdin);
            exit(0);
        }
        int idx=0,indiLen=1;
        while(msgLen[idx]-indiLen-1<n)
        {
            idx++;
            if(idx==10) indiLen++;
        }
        RScode::Galois_init(285,8);
        unsigned short* code=encode(8*indiLen,msgLen[idx]-1-indiLen,input);
        int block_num[100];
        int shortLen=msgLen[idx]/blockNum[idx];
        int shortNum=blockNum[idx]*(shortLen+1)-msgLen[idx];
        for(int i=0;i<shortNum;i++) block_num[i]=shortLen;
        for(int i=shortNum;i<blockNum[idx];i++) block_num[i]=shortLen+1;
        unsigned short* data=get_data(code,blockNum[idx],block_num,rscLen[idx],codeLen[idx]);
        printf("version: %d\n",idx+1);
        printf("error correction level: L\n");
        QRmatrix Q(21+4*idx);
        if(idx>=1)
        {
            int center[7];
            int centerNum=alig[idx][0];
            for(int i=0;i<centerNum;i++)
                center[i]=alig[idx][i+1];
            Q.draw_centers(centerNum,center);
        }
        if(idx>=6)
        {
            bool version_msg[18];
            addcode(version_msg,18,versionInfo[idx]);
            Q.draw_version(version_msg);
        }
        Q.draw_data(data,codeLen[idx]);
        int maskid=Q.masking();
        bool format_msg[15];
        addcode(format_msg,5,1<<3|maskid);
        bool* bch=BCHcode::encode(format_msg);
        Q.draw_format(bch);
        Q.draw();
    }

    void workM(unsigned char* input,int n)
    {
        using namespace errLevelM;
        if(n>msgLen[39]-3)
        {
            puts("Message is too long!");
            puts("Tip.  You can try a lower error correction level. (L)");
            std::cin.get();
            fflush(stdin);
            exit(0);
        }
        int idx=0,indiLen=1;
        while(msgLen[idx]-indiLen-1<n)
        {
            idx++;
            if(idx==10) indiLen++;
        }
        RScode::Galois_init(285,8);
        unsigned short* code=encode(8*indiLen,msgLen[idx]-1-indiLen,input);
        int block_num[100];
        int shortLen=msgLen[idx]/blockNum[idx];
        int shortNum=blockNum[idx]*(shortLen+1)-msgLen[idx];
        for(int i=0;i<shortNum;i++) block_num[i]=shortLen;
        for(int i=shortNum;i<blockNum[idx];i++) block_num[i]=shortLen+1;
        unsigned short* data=get_data(code,blockNum[idx],block_num,rscLen[idx],codeLen[idx]);
        printf("version: %d\n",idx+1);
        printf("error correction level: M\n");
        QRmatrix Q(21+4*idx);
        if(idx>=1)
        {
            int center[7];
            int centerNum=alig[idx][0];
            for(int i=0;i<centerNum;i++)
                center[i]=alig[idx][i+1];
            Q.draw_centers(centerNum,center);
        }
        if(idx>=6)
        {
            bool version_msg[18];
            addcode(version_msg,18,versionInfo[idx]);
            Q.draw_version(version_msg);
        }
        Q.draw_data(data,codeLen[idx]);
        int maskid=Q.masking();
        bool format_msg[15];
        addcode(format_msg,5,0<<3|maskid);
        bool* bch=BCHcode::encode(format_msg);
        Q.draw_format(bch);
        Q.draw();
    }

    void workQ(unsigned char* input,int n)
    {
        using namespace errLevelQ;
        if(n>msgLen[39]-3)
        {
            puts("Message is too long!");
            puts("Tip.  You can try a lower error correction level. (L or M)");
            std::cin.get();
            fflush(stdin);
            exit(0);
        }
        int idx=0,indiLen=1;
        while(msgLen[idx]-indiLen-1<n)
        {
            idx++;
            if(idx==10) indiLen++;
        }
        RScode::Galois_init(285,8);
        unsigned short* code=encode(8*indiLen,msgLen[idx]-1-indiLen,input);
        int block_num[100];
        int shortLen=msgLen[idx]/blockNum[idx];
        int shortNum=blockNum[idx]*(shortLen+1)-msgLen[idx];
        for(int i=0;i<shortNum;i++) block_num[i]=shortLen;
        for(int i=shortNum;i<blockNum[idx];i++) block_num[i]=shortLen+1;
        unsigned short* data=get_data(code,blockNum[idx],block_num,rscLen[idx],codeLen[idx]);
        printf("version: %d\n",idx+1);
        printf("error correction level: Q\n");
        QRmatrix Q(21+4*idx);
        if(idx>=1)
        {
            int center[7];
            int centerNum=alig[idx][0];
            for(int i=0;i<centerNum;i++)
                center[i]=alig[idx][i+1];
            Q.draw_centers(centerNum,center);
        }
        if(idx>=6)
        {
            bool version_msg[18];
            addcode(version_msg,18,versionInfo[idx]);
            Q.draw_version(version_msg);
        }
        Q.draw_data(data,codeLen[idx]);
        int maskid=Q.masking();
        bool format_msg[15];
        addcode(format_msg,5,3<<3|maskid);
        bool* bch=BCHcode::encode(format_msg);
        Q.draw_format(bch);
        Q.draw();
    }

    void workH(unsigned char* input,int n)
    {
        using namespace errLevelH;
        if(n>msgLen[39]-3)
        {
            puts("Message is too long!");
            puts("Tip.  You can try a lower error correction level. (L, M, or Q)");
            std::cin.get();
            fflush(stdin);
            exit(0);
        }
        int idx=0,indiLen=1;
        while(msgLen[idx]-indiLen-1<n)
        {
            idx++;
            if(idx==10) indiLen++;
        }
        RScode::Galois_init(285,8);
        unsigned short* code=encode(8*indiLen,msgLen[idx]-1-indiLen,input);
        int block_num[100];
        int shortLen=msgLen[idx]/blockNum[idx];
        int shortNum=blockNum[idx]*(shortLen+1)-msgLen[idx];
        for(int i=0;i<shortNum;i++) block_num[i]=shortLen;
        for(int i=shortNum;i<blockNum[idx];i++) block_num[i]=shortLen+1;
        unsigned short* data=get_data(code,blockNum[idx],block_num,rscLen[idx],codeLen[idx]);
        printf("version: %d\n",idx+1);
        printf("error correction level: H\n");
        QRmatrix Q(21+4*idx);
        if(idx>=1)
        {
            int center[7];
            int centerNum=alig[idx][0];
            for(int i=0;i<centerNum;i++)
                center[i]=alig[idx][i+1];
            Q.draw_centers(centerNum,center);
        }
        if(idx>=6)
        {
            bool version_msg[18];
            addcode(version_msg,18,versionInfo[idx]);
            Q.draw_version(version_msg);
        }
        Q.draw_data(data,codeLen[idx]);
        int maskid=Q.masking();
        bool format_msg[15];
        addcode(format_msg,5,2<<3|maskid);
        bool* bch=BCHcode::encode(format_msg);
        Q.draw_format(bch);
        Q.draw();
    }
}