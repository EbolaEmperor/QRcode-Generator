#include<iostream>
#include<cstdio>
#include<cstring>
#include "RS-code.hpp"
#include "QR-matrix.hpp"
#include "BCH-code.hpp"

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

    void version_1L(unsigned char* input)
    {
        unsigned short* code=encode(8,17,input);
        RScode::Galois_init(285,8);
        const int block_num[]={19};
        unsigned short* data=get_data(code,1,block_num,7,26);
        QRmatrix Q(21);
        Q.draw_data(data,26);
        int maskid=Q.masking();
        bool format_msg[15];
        addcode(format_msg,5,1<<3|maskid);
        bool* bch=BCHcode::encode(format_msg);
        Q.draw_format(bch);
        Q.draw();
    }

    void version_7H(unsigned char *input)
    {
        unsigned short* code=encode(8,64,input);
        RScode::Galois_init(285,8);
        const int block_num[]={13,13,13,13,14};
        unsigned short* data=get_data(code,5,block_num,26,196);
        QRmatrix Q(45);
        const int center[]={6,22,38};
        Q.draw_centers(3,center);
        const bool version_msg[]={0,0,0,1,1,1,1,1,0,0,1,0,0,1,0,1,0,0};
        Q.draw_version(version_msg);
        Q.draw_data(data,196);
        int maskid=Q.masking();
        bool format_msg[15];
        addcode(format_msg,5,2<<3|maskid);
        bool* bch=BCHcode::encode(format_msg);
        Q.draw_format(bch);
        Q.draw();
    }

    void version_10Q(unsigned char* input)
    {
        unsigned short* code=encode(16,151,input);
        RScode::Galois_init(285,8);
        const int block_num[]={19,19,19,19,19,19,20,20};
        unsigned short* data=get_data(code,8,block_num,24,346);
        QRmatrix Q(57);
        const int center[]={6,28,50};
        Q.draw_centers(3,center);
        const bool version_msg[]={0,0,1,0,1,0,0,1,0,0,1,1,0,1,0,0,1,1};
        Q.draw_version(version_msg);
        Q.draw_data(data,346);
        int maskid=Q.masking();
        bool format_msg[15];
        addcode(format_msg,5,3<<3|maskid);
        bool* bch=BCHcode::encode(format_msg);
        Q.draw_format(bch);
        Q.draw();
    }

    void version_15Q(unsigned char* input)
    {
        unsigned short* code=encode(16,292,input);
        RScode::Galois_init(285,8);
        const int block_num[]={24,24,24,24,24,25,25,25,25,25,25,25};
        unsigned short* data=get_data(code,12,block_num,30,655);
        QRmatrix Q(77);
        const int center[]={6,26,48,70};
        Q.draw_centers(4,center);
        const bool version_msg[]={0,0,1,1,1,1,1,0,0,1,0,0,1,0,1,0,0,0};
        Q.draw_version(version_msg);
        Q.draw_data(data,655);
        int maskid=Q.masking();
        bool format_msg[15];
        addcode(format_msg,5,3<<3|maskid);
        bool* bch=BCHcode::encode(format_msg);
        Q.draw_format(bch);
        Q.draw();
    }

    void version_40M(unsigned char* input)
    {
        unsigned short* code=encode(16,2331,input);
        RScode::Galois_init(285,8);
        int block_num[49];
        for(int i=0;i<18;i++) block_num[i]=47;
        for(int i=18;i<49;i++) block_num[i]=48;
        unsigned short* data=get_data(code,49,block_num,28,3706);
        QRmatrix Q(177);
        const int center[]={6,30,58,86,114,142,170};
        Q.draw_centers(7,center);
        const bool version_msg[]={1,0,1,0,0,0,1,1,0,0,0,1,1,0,1,0,0,1};
        Q.draw_version(version_msg);
        Q.draw_data(data,3706);
        int maskid=Q.masking();
        bool format_msg[15];
        addcode(format_msg,5,maskid);
        bool* bch=BCHcode::encode(format_msg);
        Q.draw_format(bch);
        Q.draw();
    }
}