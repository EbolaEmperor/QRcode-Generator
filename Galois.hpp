#include <algorithm>
#include <iostream>
#include <cstring>

namespace GaloisSP
{
    unsigned short flog[256];   //a^i 对应的二进制数
    unsigned short iflog[256];   //二进制数对应的a的幂次
    int mx;
    
    struct Galois
    {
        unsigned short x;    //在Galois域中，这个数可以表示为a^x，x=65535表示这个数为0
        Galois(unsigned short _x = 0) { x = iflog[_x]; }    //将一个二进制数转换为Galois域中的a^x表示法
        unsigned short toint() const { return x == 65535 ? 0 : flog[x]; }
        
        bool operator == (const Galois &b) const { return x == b.x; }
        bool operator != (const Galois &b) const { return x != b.x; }
        
        Galois operator + (const Galois &b) const
        {
            if (x == 65535) return b;
            else if (b.x == 65535) return (*this);
            else return Galois(flog[x] ^ flog[b.x]);
        }
        Galois operator - (const Galois &b) const
        {
            return (*this) + b;
        }
        Galois operator * (const Galois &b) const
        {
            if (x == 65535 || b.x == 65535)
                return Galois(0);
            Galois res;
            res.x = (x + b.x) % mx;
            return res;
        }
        Galois operator / (const Galois &b) const
        {
            if (x == 65535) return Galois(0);
            Galois res;
            res.x = (x - b.x + mx) % mx;
            return res;
        }

        friend std::ostream& operator << (std::ostream &out, const Galois &g)
        {
            out << g.toint();
            return out;
        }
    };

    void Galois_init(int origin, int len)
    //初始化一个Galois域: GP(2^len),  其中origin是本原多项式系数的二进制表示
    {
        flog[0] = 1;
        iflog[1] = 0;
        iflog[0] = 65535;  //65535表示0值，此时不能被表示为a^x的形式
        for (int i = 1; i < (1 << len) - 1; i++)
        {
            flog[i] = flog[i - 1] << 1;
            if (flog[i] >> len) flog[i] ^= origin;
            iflog[flog[i]] = i;
        }
        flog[(1 << len) - 1] = 0;
        GaloisSP::mx = (1 << len) - 1;
    }

    void Galois_init(int len)
    {
        const int primitive_polynomials[] = {
            3,        // n=1: x + 1
            7,        // n=2: x^2 + x + 1
            11,       // n=3: x^3 + x + 1
            19,       // n=4: x^4 + x^3 + x + 1
            31,       // n=5: x^5 + x^4 + x^3 + x + 1
            57,       // n=6: x^6 + x^5 + x^3 + x + 1
            127,      // n=7: x^7 + x^6 + x^5 + x + 1
            285,      // n=8: x^8 + x^4 + x^3 + x + 1
            511,      // n=9: x^9 + x^8 + x^7 + x^6 + x^5 + x^4 + x^3 + x^2 + x + 1
            1023,     // n=10: x^10 + x^9 + x^8 + x^7 + x^6 + x^5 + x^4 + x^3 + x^2 + x + 1
            1985,     // n=11: x^11 + x^9 + x^8 + x^7 + x^6 + x^5 + x^3 + x + 1
            4095,     // n=12: x^12 + x^11 + x^10 + x^9 + x^8 + x^7 + x^6 + x^5 + x^4 + x^3 + x^2 + x + 1
            8191,     // n=13: x^13 + x^12 + x^11 + x^10 + x^9 + x^8 + x^7 + x^6 + x^5 + x^4 + x^3 + x^2 + x + 1
            16383,    // n=14: x^14 + x^13 + x^12 + x^11 + x^10 + x^9 + x^8 + x^7 + x^6 + x^5 + x^4 + x^3 + x^2 + x + 1
            32767     // n=15: x^15 + x^14 + x^13 + x^12 + x^11 + x^10 + x^9 + x^8 + x^7 + x^6 + x^5 + x^4 + x^3 + x^2 + x + 1
        };
        if (len < 1 || len > 15)
        {
            std::cout << "GF(2^" << len << ") is not supported." << std::endl;
            exit(0);
        }
        Galois_init(primitive_polynomials[len - 1], len);
    }

    void Galois_init()
    {
        // 默认初始化为 GF(2^8)
        Galois_init(285, 8);
    }

    void Galois_clear()
    {
        memset(flog, 0, sizeof(flog));
        memset(iflog, 0, sizeof(iflog));
        mx = 0;
    }
}
