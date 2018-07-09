// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2013 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_TEST_BIGNUM_H
#define BITCOIN_TEST_BIGNUM_H

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <vector>

#include <openssl/bn.h>

class bignum_error : public std::runtime_error
{
public:
    explicit bignum_error(const std::string& str) : std::runtime_error(str) {}
};


/** C++ wrapper for BIGNUM (OpenSSL bignum) */
class CBigNum : public BIGNUM
{
public:
    CBigNum()
    {
        BN_init(this);
    }

    CBigNum(const CBigNum& b)
    {
        BN_init(this);
        if (!BN_copy(this, &b))
        {
            BN_clear_free(this);
            throw bignum_error("CBigNum::CBigNum(const CBigNum&): BN_copy failed");
        }
    }

    CBigNum& operator=(const CBigNum& b)
    {
        if (!BN_copy(this, &b))
            throw bignum_error("CBigNum::operator=: BN_copy failed");
        return (*this);
    }

    ~CBigNum()
    {
        BN_clear_free(this);
    }

    CBigNum(long long n)          { BN_init(this); setint64(n); }

    explicit CBigNum(const std::vector<unsigned char>& vch)
    {
        BN_init(this);
        setvch(vch);
    }

    int getint() const
    {
        BN_ULONG n = BN_get_word(this);
        if (!BN_is_negative(this))
            return (n > (BN_ULONG)std::numeric_limits<int>::max() ? std::numeric_limits<int>::max() : n);
        else
            return (n > (BN_ULONG)std::numeric_limits<int>::max() ? std::numeric_limits<int>::min() : -(int)n);
    }

    void setint64(int64_t sn)
    {
        unsigned char pch[sizeof(sn) + 6];
        unsigned char* p = pch + 4;
        bool fNegative;
        uint64_t n;

        if (sn < (int64_t)0)
        {
            // Since the minimum signed integer cannot be represented as positive so long as its type is signed,
            // and it's not well-defined what happens if you make it unsigned before negating it,
            // we instead increment the negative integer by 1, convert it, then increment the (now positive) unsigned integer by 1 to compensate
            n = -(sn + 1);
            ++n;
            fNegative = true;
        } else {
            n = sn;
            fNegative = false;
        }

        bool fLeadingZeroes = true;
        for (int i = 0; i < 8; i++)
        {
            unsigned char c = (n >> 56) & 0xff;
            n <<= 8;
            if (fLeadingZeroes)
            {
                if (c == 0)
                    continue;
                if (c & 0x80)
                    *p++ = (fNegative ? 0x80 : 0);
                else if (fNegative)
                    c |= 0x80;
                fLeadingZeroes = false;
            }
            *p++ = c;
        }
        unsigned int nSize = p - (pch + 4);
        pch[0] = (nSize >> 24) & 0xff;
        pch[1] = (nSize >> 16) & 0xff;
        pch[2] = (nSize >> 8) & 0xff;
        pch[3] = (nSize) & 0xff;
        BN_mpi2bn(pch, p - pch, this);
    }

    void setvch(const std::vector<unsigned char>& vch)
    {
        std::vector<unsigned char> vch2(vch.size() + 4);
        unsigned int nSize = vch.size();
        // BIGNUM's byte stream format expects 4 bytes of
        // big endian size data info at the front
        vch2[0] = (nSize >> 24) & 0xff;
        vch2[1] = (nSize >> 16) & 0xff;
        vch2[2] = (nSize >> 8) & 0xff;
        vch2[3] = (nSize >> 0) & 0xff;
        // swap data to big endian
        reverse_copy(vch.begin(), vch.end(), vch2.begin() + 4);
        BN_mpi2bn(&vch2[0], vch2.size(), this);
    }

    std::vector<unsigned char> getvch() const
    {
        unsigned int nSize = BN_bn2mpi(this, NULL);
        if (nSize <= 4)
            return std::vector<unsigned char>();
        std::vector<unsigned char> vch(nSize);
        BN_bn2mpi(this, &vch[0]);
        vch.erase(vch.begin(), vch.begin() + 4);
        reverse(vch.begin(), vch.end());
        return vch;
    }
    unsigned int GetLegacyCompact() const
    {
        unsigned int nSize = BN_num_bytes(this);
        unsigned int nCompact = 0;
        if(nSize <= 3)
          nCompact = BN_get_word(this) << 8*(3-nSize);
        else
        {
          CBigNum bn;
          BN_rshift(&bn, this, 8*(nSize-3));
          nCompact = BN_get_word(&bn);
        }
        // The 0x00800000 bit denotes the sign.
        // Thus, if it is already set, divide the mantissa by 256 and increase the exponent
        if(nCompact & 0x00800000)
        {
          nCompact >>= 8;
          nSize++;
        }
        nCompact |= nSize << 24;
        nCompact |= (BN_is_negative(this) ? 0x00800000 : 0);
        return nCompact;
    }

    // The "compact" format is a representation of a whole
        // number N using an unsigned 32bit number similar to a
        // floating point format.
        // The most significant 8 bits are the unsigned exponent of base 256.
        // This exponent can be thought of as "number of bytes of N".
        // The lower 23 bits are the mantissa.
        // Bit number 24 (0x800000) represents the sign of N.
        // N = (-1^sign) * mantissa * 256^(exponent-3)
        //
        // Satoshi's original implementation used BN_bn2mpi() and BN_mpi2bn().
        // MPI uses the most significant bit of the first byte as sign.
        // Thus 0x1234560000 is compact (0x05123456)
        // and  0xc0de000000 is compact (0x0600c0de)
        // (0x05c0de00) would be -0x40de000000
        //
        // Bitcoin only uses this "compact" format for encoding difficulty
        // targets, which are unsigned 256bit quantities.  Thus, all the
        // complexities of the sign bit and using base 256 are probably an
        // implementation accident.
        //
        // This implementation directly uses shifts instead of going
        // through an intermediate MPI representation.

        CBigNum& SetLegacyCompact(unsigned int nCompact)
        {
          unsigned int nSize = nCompact >> 24;
           bool fNegative     =(nCompact & 0x00800000) != 0;
           unsigned int nWord = nCompact & 0x007fffff;
           if (nSize <= 3)
           {
               nWord >>= 8*(3-nSize);
               BN_set_word(this, nWord);
           }
           else
           {
               BN_set_word(this, nWord);
               BN_lshift(this, this, 8*(nSize-3));
           }
           BN_set_negative(this, fNegative);
           return *this;
        }
        
    friend inline const CBigNum operator-(const CBigNum& a, const CBigNum& b);
};



inline const CBigNum operator+(const CBigNum& a, const CBigNum& b)
{
    CBigNum r;
    if (!BN_add(&r, &a, &b))
        throw bignum_error("CBigNum::operator+: BN_add failed");
    return r;
}

inline const CBigNum operator-(const CBigNum& a, const CBigNum& b)
{
    CBigNum r;
    if (!BN_sub(&r, &a, &b))
        throw bignum_error("CBigNum::operator-: BN_sub failed");
    return r;
}

inline const CBigNum operator-(const CBigNum& a)
{
    CBigNum r(a);
    BN_set_negative(&r, !BN_is_negative(&r));
    return r;
}

inline bool operator==(const CBigNum& a, const CBigNum& b) { return (BN_cmp(&a, &b) == 0); }
inline bool operator!=(const CBigNum& a, const CBigNum& b) { return (BN_cmp(&a, &b) != 0); }
inline bool operator<=(const CBigNum& a, const CBigNum& b) { return (BN_cmp(&a, &b) <= 0); }
inline bool operator>=(const CBigNum& a, const CBigNum& b) { return (BN_cmp(&a, &b) >= 0); }
inline bool operator<(const CBigNum& a, const CBigNum& b)  { return (BN_cmp(&a, &b) < 0); }
inline bool operator>(const CBigNum& a, const CBigNum& b)  { return (BN_cmp(&a, &b) > 0); }



#endif // BITCOIN_TEST_BIGNUM_H
