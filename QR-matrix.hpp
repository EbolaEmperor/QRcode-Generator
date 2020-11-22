#include<cstring>
#include<cstdio>
#include "png.hpp"
#include "mask.hpp"

struct QRmatrix
{
    int n;
    bool** A;
    bool** notdata;
    QRmatrix(){}
    void forbid(int x,int y,int r,int c);
    void draw_position(int x,int y);
    void nextpos(int &x,int &y,int &p);
    void draw_timing();
    void draw_format(bool* format_msg);
    void draw_data(unsigned short* data,int len);
    void mask_with(bool calc(int,int));
    int masking();
    int evaluate();
    void draw();
    QRmatrix(int _n=0)
    {
        n=_n;
        A=new bool* [n];
        notdata=new bool* [n];
        for(int i=0;i<n;i++)
        {
            A[i]=new bool[n];
            notdata[i]=new bool[n];
            for(int j=0;j<n;j++)
                A[i][j]=notdata[i][j]=0;
        }
        draw_position(0,0);
        draw_position(0,n-7);
        draw_position(n-7,0);
        forbid(0,0,9,9);
        forbid(0,n-8,9,8);
        forbid(n-8,0,8,9);
        draw_timing();
    }
    void display();
};

void QRmatrix::draw_position(int x,int y)
{
    for(int i=0;i<7;i++)
        A[x+0][y+i]=A[x+6][y+i]=A[x+i][y+0]=A[x+i][y+6]=1;
    for(int i=0;i<3;i++)
        A[x+2][y+2+i]=A[x+2+i][y+2]=A[x+4][y+2+i]=A[x+2+i][y+4]=1;
    A[x+3][y+3]=1;
}

void QRmatrix::draw_timing()
{
    bool p=1;
    for(int i=8;i<n-8;i++)
    {
        A[i][6]=A[6][i]=p,p^=1;
        notdata[i][6]=notdata[6][i]=1;
    }
}

void QRmatrix::forbid(int x,int y,int r,int c)
{
    for(int i=0;i<r;i++)
        for(int j=0;j<c;j++)
            notdata[x+i][y+j]=1;
}

void QRmatrix::display()
{
    for(int i=0;i<n;i++,putchar('\n'))
        for(int j=0;j<n;j++)
            if(A[i][j]) putchar('#');
            else putchar(' ');
}

void QRmatrix::nextpos(int &x,int &y,int &p)
{
    if(x==9&&y==7) y-=2,p=3;
    else if(p==1) y--,p=2;
    else if(p==2)
    {
        if(x>0) x--,y++,p=1;
        else y--,p=3;
    }
    else if(p==3) y--,p=4;
    else if(p==4)
    {
        if(x<n-1) x++,y++,p=3;
        else y--,p=1;
    }
}

void QRmatrix::draw_data(unsigned short* data,int len)
{
    int curx=n-1,cury=n-1,curp=1;
    for(int i=0;i<len;i++)
        for(int j=7;j>=0;j--)
        {
            int x=(data[i]>>j)&1;
            while(notdata[curx][cury])
                nextpos(curx,cury,curp);
            A[curx][cury]=x;
            if(!(i==len-1&&j==0))
                nextpos(curx,cury,curp);
        }
}

void QRmatrix::draw_format(bool* format_msg)
{
    const bool format_mask[]={1,0,1,0,1,0,0,0,0,0,1,0,0,1,0};
    for(int i=0;i<15;i++)
        format_msg[i]^=format_mask[i];
    std::reverse(format_msg,format_msg+15);
    A[n-8][8]=1;
    for(int i=0;i<8;i++)
        A[8][n-1-i]=format_msg[i];
    for(int i=0;i<6;i++)
        A[i][8]=format_msg[i];
    A[7][8]=format_msg[6];
    A[8][8]=format_msg[7];
    A[8][7]=format_msg[8];
    for(int i=0;i<7;i++)
        A[n-7+i][8]=format_msg[8+i];
    for(int i=0;i<6;i++)
        A[8][5-i]=format_msg[9+i];
}

void QRmatrix::draw()
{
    const int PW=5;
    unsigned short** P;
    P=new unsigned short* [PW*n];
    for(int i=0;i<PW*n;i++)
        P[i]=new unsigned short[PW*n];
    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++)
        {
            unsigned short x=(A[i][j]?0:255);
            for(int r=0;r<PW;r++)
                for(int c=0;c<PW;c++)
                    P[PW*i+r][PW*j+c]=x;
        }
    png::png_print(PW*n,PW*n,P,P,P);
}

int QRmatrix::evaluate()
{
    int res=0;
    for(int i=0;i<n-1;i++)
        for(int j=0;j<n-1;j++)
        {
            if(A[i][j]==A[i][j+1]) res++;
            if(A[i][j]==A[i+1][j]) res++;
        }
    return res;
}

void QRmatrix::mask_with(bool calc(int,int))
{
    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++)
            if(!notdata[i][j]) A[i][j]^=calc(i,j);
}

int QRmatrix::masking()
{
    mask_with(mask::mask0);
    int score0=evaluate();
    mask_with(mask::mask0);

    mask_with(mask::mask1);
    int score1=evaluate();
    mask_with(mask::mask1);

    mask_with(mask::mask2);
    int score2=evaluate();
    mask_with(mask::mask2);

    mask_with(mask::mask3);
    int score3=evaluate();
    mask_with(mask::mask3);

    mask_with(mask::mask4);
    int score4=evaluate();
    mask_with(mask::mask4);

    mask_with(mask::mask5);
    int score5=evaluate();
    mask_with(mask::mask5);

    mask_with(mask::mask6);
    int score6=evaluate();
    mask_with(mask::mask6);

    mask_with(mask::mask7);
    int score7=evaluate();
    mask_with(mask::mask7);

    int minsc=n*n*2;
    if(score0<minsc) minsc=score0;
    if(score1<minsc) minsc=score1;
    if(score2<minsc) minsc=score2;
    if(score3<minsc) minsc=score3;
    if(score4<minsc) minsc=score4;
    if(score5<minsc) minsc=score5;
    if(score6<minsc) minsc=score6;
    if(score7<minsc) minsc=score7;

    int maskid;
    if(score7==minsc) mask_with(mask::mask7),maskid=7;
    else if(score6==minsc) mask_with(mask::mask6),maskid=6;
    else if(score5==minsc) mask_with(mask::mask5),maskid=5;
    else if(score4==minsc) mask_with(mask::mask4),maskid=4;
    else if(score3==minsc) mask_with(mask::mask3),maskid=3;
    else if(score2==minsc) mask_with(mask::mask2),maskid=2;
    else if(score1==minsc) mask_with(mask::mask1),maskid=1;
    else if(score0==minsc) mask_with(mask::mask0),maskid=0;
    return maskid;
}