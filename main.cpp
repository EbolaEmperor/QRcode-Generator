#include<iostream>
#include<cstdio>
#include<cstring>
#include "QR-code.hpp"

int main()
{
    static unsigned char input[3000];
    FILE *inF;
    inF=fopen("input.txt","r");
    if(inF==NULL)
    {
        puts("input.txt needed.");
        std::cin.get();
        fflush(stdin);
        return 0;
    }
    unsigned char c=getc(inF);
    int n=0;
    while(c!=255)
        input[n++]=c,c=getc(inF);
    fclose(inF);
    inF=NULL;
    printf("text bytes: %d\n",n);

    char errLevel;
    FILE *errF;
    errF=fopen("config.ini","r");
    if(errF==NULL)
    {
        puts("config.ini needed.");
        std::cin.get();
        fflush(stdin);
        return 0;
    }
    errLevel=getc(errF);
    fclose(errF);
    errF=NULL;

    if(errLevel=='L') QRcode::workL(input,n);
    else if(errLevel=='M') QRcode::workM(input,n);
    else if(errLevel=='Q') QRcode::workQ(input,n);
    else if(errLevel=='H') QRcode::workH(input,n);
    else
    {
        puts("Please set a correct error correction level in config.ini.");
        puts("Tip.  The error correction level should be a single letter L, M, Q, or H.");
        std::cin.get();
        fflush(stdin);
        return 0;
    }
    puts("success! Please open code.png");
    std::cin.get();
    fflush(stdin);
    return 0;
}