#include <gtest/gtest.h>

#include "serializer.h"

TEST(Archiver, check__single_integer) {

    Archive s;

    s << 1;

    const QByteArray &result = s.getData();

    // little-endian of int 1
    const char expected[]{1,0,0,0};

    EXPECT_EQ(sizeof(int), result.length());
    EXPECT_TRUE(std::equal(expected, expected+4, result.constData()));
}

TEST(Archiver, check__single_float) {

    Archive s;

    s << 2.f;

    const QByteArray &result = s.getData();

    // IEEE representation of 2.0f
    const char expected[]{0,0,0,0x40};

    EXPECT_EQ(sizeof(float), result.length());
    EXPECT_TRUE(std::equal(expected, expected+4, result.constData()));
}

TEST(Serializer, check__single_int)
{
   Serializer s;

   int i = 123456;

   Archive a = s.serialize(i);

   const QByteArray &result = a.getData();

   // little-endian of int 123456
   const char expected[]{0x40,0xE2,0x01,0x0};

   EXPECT_EQ(sizeof(int), result.length());
   EXPECT_TRUE(std::equal(expected, expected+4, result.constData()));
}

TEST(Serializer, check__single_float)
{
    Serializer s;

    float f = 123456.1234;

    Archive a = s.serialize(f);

    const QByteArray &result = a.getData();

    // little-endian of float 123456.1234
    const char expected[]{0x10,0x20,0xF1,0x47};

    EXPECT_EQ(sizeof(float), result.length());
    EXPECT_TRUE(std::equal(expected, expected+4, result.constData()));
}

TEST(Serializer, check__single_double)
{
    Serializer s;

    double d = 14.5e-14;

    Archive a = s.serialize(d);

    const QByteArray &result = a.getData();

    // little-endian of double 14.5e-14
    const char expected[]{0x84,0x0F,0x02,0xF2,0x2C,0x68,0x44,0x3d};

    EXPECT_EQ(sizeof(double), result.length());
    EXPECT_TRUE(std::equal(expected, expected+8, result.constData()));
}

TEST(Serializer, check__single_bool)
{
    Serializer s;

    bool b = true;

    Archive a = s.serialize(b);

    const QByteArray &result = a.getData();

    // little-endian of bool
    const char expected[]{1};

    EXPECT_EQ(sizeof(bool), result.length());
    EXPECT_TRUE(std::equal(expected, expected+1, result.constData()));
}

TEST(Serializer, check__single_pointer)
{
    Serializer s;

    int d = 15;
    int *ptr = &d;

    long long address = (long long)ptr;

    Archive a = s.serialize(ptr);

    const QByteArray &data = a.getData();

    std::vector<char> expected((const char*)&address, (const char*)&address + sizeof(ptr));
    std::vector<char> result(data.constBegin(), data.constEnd());

    EXPECT_EQ(sizeof(int*), data.length());
    EXPECT_EQ(expected, result);
}

TEST(Serializer, check_pod_types)
{
    Serializer s;

    struct PodTypes
    {
        unsigned char      uc;
        signed char        sc;
        unsigned short     us;
        signed short       ss;
        unsigned int       ui;
        signed int         si;
        unsigned long long ul;
        signed long long   sl;
        float              f ;
        double             d ;
    };

    union Union
    {
        double             d;
        unsigned char      uc;
    };

    PodTypes podTypes;
    Union unionData;

    podTypes.uc  = 255;
    podTypes.sc  = -128;
    podTypes.us  = 65535;
    podTypes.ss  = -32768;
    podTypes.ui  = 4294967295;
    podTypes.si  = -2147483648;
    podTypes.ul  = 184467440737091615;
    podTypes.sl  = -92233720368545808;
    podTypes.f   = -3.4e-38;
    podTypes.d   = 1.7e+308;

    unionData.d = 0.56e5;

    int *psi = &podTypes.si;
    signed char *psc = &podTypes.sc;
    double *pd = &podTypes.d;
    bool b = true;

    long long apsi = (long long)psi;
    long long apsc = (long long)psc;
    long long apd = (long long)pd;

    Archive a = s.serialize(podTypes, psi, psc, pd, b, unionData);

    const QByteArray &data = a.getData();

    std::vector<char> expected;
    expected.insert(expected.end(), (const char*)&podTypes, (const char*)&podTypes + sizeof(PodTypes));
    expected.insert(expected.end(), (const char*)&apsi, (const char*)&apsi + sizeof(apsi));
    expected.insert(expected.end(), (const char*)&apsc, (const char*)&apsc + sizeof(apsc));
    expected.insert(expected.end(), (const char*)&apd, (const char*)&apd + sizeof(apd));
    expected.insert(expected.end(), b);
    expected.insert(expected.end(), (const char*)&unionData, (const char*)&unionData + sizeof(Union));
    std::vector<char> result(data.constBegin(), data.constEnd());

    EXPECT_EQ(sizeof(PodTypes) +
              sizeof(int*) +
              sizeof(signed char*) +
              sizeof(double*) +
              sizeof(bool) +
              sizeof(Union), data.length());
    EXPECT_EQ(expected, result);
}
