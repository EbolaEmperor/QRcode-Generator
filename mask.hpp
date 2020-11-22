namespace mask
{
    bool mask0(int x,int y)
    {
        return (x+y)%2==0;
    }

    bool mask1(int x,int y)
    {
        return x%2==0;
    }

    bool mask2(int x,int y)
    {
        return y%3==0;
    }

    bool mask3(int x,int y)
    {
        return (x+y)%3==0;
    }

    bool mask4(int x,int y)
    {
        return (x/2+y/3)%2==0;
    }

    bool mask5(int x,int y)
    {
        return (x*y)%2+(x*y)%3==0;
    }

    bool mask6(int x,int y)
    {
        return ((x*y)%2+(x*y)%3)%2==0;
    }

    bool mask7(int x,int y)
    {
        return ((x*y)%2+(x+y)%3)%2==0;
    }
}