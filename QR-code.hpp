#include<iostream>
#include<cstdio>
#include<cstring>
#include "RS-code.hpp"
#include "QR-matrix.hpp"
#include "BCH-code.hpp"

namespace QRcode
{
    const int N=10000;

    void addcode(bool* s,const char* code)
    {
        int n=strlen(code);
        for(int i=0;i<n;i++)
            s[i]=code[i]-'0';
    }

    unsigned short* encode(int indicator_bits,int bits_len,char* input)
    {
        static bool bits[N];
        static unsigned short code[N];
        addcode(bits,"0100");
        int n=strlen(input);
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
        int code_len=1+indicator_bits+bits_len;
        for(int i=0;i<code_len;i++)
        {
            code[i]=0;
            for(int j=0;j<8;j++)
                code[i]=(code[i]<<1)|bits[8*i+j];
        }
        return code;
    }

    void version_1L(char* input)
    {
        unsigned short* code=encode(8,17,input);
        int n=19;
        RScode::Galois_init(285,8);
        unsigned short* rsc=RScode::encode(code,19,7);
        unsigned short data[26];
        for(int i=0;i<19;i++) data[i]=code[i];
        for(int i=0;i<7;i++) data[19+i]=rsc[i];
        QRmatrix Q(21);
        Q.draw_data(data,26);
        int maskid=Q.masking();
        bool format_msg[15];
        format_msg[0]=0;
        format_msg[1]=1;
        format_msg[2]=(maskid>>2)&1;
        format_msg[3]=(maskid>>1)&1;
        format_msg[4]=(maskid>>0)&1;
        bool* bch=BCHcode::encode(format_msg);
        Q.draw_format(bch);
        Q.draw();
    }

    void version_10Q(char* input)
    {
    }
}