#pragma once

#include <string>
#include <array>
#include <cmath>
#include <sstream>

using BYTE = unsigned char;
using BLOCK = unsigned int;
using BLOCK64 = unsigned long long;

class MD5
{
public:
    MD5()
    {
        init();
    }

    std::string stringHash(std::string message)
    {
        init();
        fillDataBlock(message);
        calculateHash(message);
        return getPrettyHash();
    }

    void setHash(const std::array<BLOCK, 4> hashArr = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476})
    {
        a0 = hashArr[0];
        b0 = hashArr[1];
        c0 = hashArr[2];
        d0 = hashArr[3];
    }

    inline std::array<BLOCK, 4> getHash()const
    {
        return {a0, b0, c0, d0};
    }

    std::string getPrettyHash()const
    {
        auto const hash = getHash();
        std::stringstream ssHash;
        for (BYTE i = 0; i < 4; i ++)
            ssHash << std::hex << hash[i];

        return ssHash.str();
    }

private:
    void initQuarterOfShift(const BYTE nQuarter /*1 - 4*/, const std::array<BYTE,4> data)
    {
        for (BYTE i = 16 * (nQuarter - 1); i < 16 * nQuarter; i++)
            Shifts[i] = data[i % 4];
    }

    void init()
    {
        setHash();

        initQuarterOfShift(1, {7, 12, 17, 22});
        initQuarterOfShift(2, {5,  9, 14, 20});
        initQuarterOfShift(3, {4, 11, 16, 23});
        initQuarterOfShift(4, {6, 10, 15, 21});

        for (BYTE i = 0; i < 64; i++)
            K[i] = std::floor(0x100000000 * std::abs(sin(i + 1)));
    }

    void fillDataBlock(std::string& data)
    {
        data.push_back(0x80);
        const BLOCK64 originalDataLen = data.length();

        // 448 + 64 == 512 bits

        while (data.size() % 56 != 0) // 56 bytes - 448 bits
            data.push_back(0x00);

        for (BYTE i = 0; i < 8; i++) // 8 bytes - 64 bits
            data.push_back((originalDataLen >> ((7 - i) * 8)) % 0x100);

        // 00 00 00 00 00 11 08 f9 1a - originalDataLen

        // one ">>" equals one byte or 8 bits of shifting

        // >> >> >> >> >> >> >> >> 00
        // >> >> >> >> >> >> >> 00 00
        // >> >> >> >> >> >> 00 00 00
        // >> >> >> >> >> 00 00 00 00
        // >> >> >> >> 00 00 00 00 00
        // >> >> >> 00 00 00 00 00 11
        // >> >> 00 00 00 00 00 11 08
        // >> 00 00 00 00 00 11 08 f9
        // 00 00 00 00 00 11 08 f9 1a
        //                         % 0x100
        // 00 00 00 00 00 00 00 00 1a
    }

    static std::array<BLOCK, 16> strToArray(const std::string& strBlock512)
    {
        std::array<BLOCK, 16> block;
        for (BYTE i = 0; i < 16; i++) // 16 - count of mini blocks // 32 - mini block lenght
            std::copy(strBlock512.begin() + i * 32, strBlock512.begin() + (i + 1) * 32, block.begin());

        return block;
    }

    void calculateBlockHash(const std::string& strBlock512)
    {
        const auto block = strToArray(strBlock512);

        BLOCK A = a0;
        BLOCK B = b0;
        BLOCK C = c0;
        BLOCK D = d0;

        for (BYTE i = 0; i < 64; i++)
        {
            BLOCK F(0), g(0);

            if (i < 16)
            {
                F = (B & C) | ((~ B) & D);
                g = i;
            }
            else if (i < 32)
            {
                F = (D & B) | ((~ D) & C);
                g = (5 * i + 1) % 16;
            }
            else if (i < 48)
            {
                F = B ^ C ^ D;
                g = (3*i + 5) % 16;
            }
            else
            {
                F = C ^ (B | (~ D));
                g = (7*i) % 16;
            }

            F = F + A + K[i] + block[g];  // M[g] — 32 битный блок
            A = D;
            D = C;
            C = B;
            B = B + (F << Shifts[i]); // Выполняем битовый сдвиг
        }

        a0 = A;
        b0 = B;
        c0 = C;
        d0 = D;
    }

    void calculateHash(const std::string strBlocks)
    {
        const BLOCK countBlocks = strBlocks.size() % 64; // 512 bits == 64 bytes
        for (BLOCK i = 0; i < countBlocks; i++)
            calculateBlockHash(strBlocks.substr(i*64, 64)); // get sub str (from start block possition , len == 64)
    }

private:
    std::array<BLOCK, 64> K;
    std::array<BYTE, 64> Shifts;
    BLOCK a0, b0, c0, d0;
};
