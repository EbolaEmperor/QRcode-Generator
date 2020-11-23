#include<cstdio>
#include<cstring>
#include "QR-code.hpp"

using namespace std;

int main()
{
    static char input[3000];
    freopen("input.txt","r",stdin);
    char c=getchar();
    int n=0;
    while(c!=EOF)
        input[n++]=c,c=getchar();
    freopen("CON","r",stdin);
    if(n<=17) QRcode::version_1L(input);
    else if(n<=64) QRcode::version_7H(input);
    else if(n<=151) QRcode::version_10Q(input);
    else if(n<=292) QRcode::version_15Q(input);
    else QRcode::version_40M(input);
    puts("success! Please open code.png");
    system("pause");
    return 0;
}