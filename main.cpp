#include<cstdio>
#include<cstring>
#include "QR-code.hpp"

using namespace std;

int main()
{
    static char input[100];
    scanf("%s",input);
    int n=strlen(input);
    if(n<=17) QRcode::version_1L(input);
    else if(n<=154) QRcode::version_10Q(input);
    puts("success! Please open code.png");
    system("pause");
    return 0;
}
