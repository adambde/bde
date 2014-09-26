// bsls_byteorderutil_impl.t.cpp                                      -*-C++-*-

#include <bsls_byteorderutil_impl.h>

#include <bsls_bsltestutil.h>    // for testing only

#include <cstdlib>
#include <cstdio>

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// [3] TESTING CONSISTENCY OF COMPILE-TIME FLAGS
// [2] TESTING 'mySwapBytes*'
// [1] TESTING 'genericSwap*' & SOUNDNESS OF TABLE
//-----------------------------------------------------------------------------
// [3] 'BSLS_BYTEORDERUTIL_IMPL_CUSTOM_*'
// [2] 'mySwapBytes[16,32,64}'
// [1] 'genericSwap{16,32,64}'
//-----------------------------------------------------------------------------

using namespace BloombergLP;
using std::printf;
using std::fprintf;

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

#define PHEX(X)  printf(#X " = 0x%llx\n", (Uint64) (X));
#define PHEX_(X) printf(#X " = 0x%llx\t", (Uint64) (X));                     \
                 bsls::BslTestUtil::flush();


#if defined(BSLS_PLATFORM_CMP_MSVC) ||                                        \
    (defined(BSLS_PLATFORM_CPU_POWERPC) && defined(BSLS_PLATFORM_CPU_32_BIT))
#define BYTEORDERUTIL_SIZEOF_WCHAR_T 2
#else
#define BYTEORDERUTIL_SIZEOF_WCHAR_T 4
#endif

#if defined(BSLS_PLATFORM_CPU_64_BIT) && defined(BSLS_PLATFORM_OS_UNIX)
#define BYTEORDERUTIL_SIZEOF_LONG 8
#else
#define BYTEORDERUTIL_SIZEOF_LONG 4
#endif

//=============================================================================
//                         GLOBAL TYPEDEF FOR TESTING
//-----------------------------------------------------------------------------

typedef BloombergLP::bsls::Types::Int64  Int64;
typedef BloombergLP::bsls::Types::Uint64 Uint64;

typedef BloombergLP::bsls::ByteOrderUtil_Impl_Concrete Concrete;

//=============================================================================
//                    GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

template <class TYPE>
void swapBytesInPlace(TYPE *value)
    // Swap the byte order of the specified integral object '*value'.
{
    char *tail = reinterpret_cast<char *>(value + 1) - 1;
    char *head = reinterpret_cast<char *>(value);

    for (; head < tail; ++head, --tail) {
        char tmp = *head;
        *head = *tail;
        *tail = tmp;
    }
}

template <class T>
T mySwapBytes(T x)
{
    return bsls::ByteOrderUtil_Impl<sizeof(x)>::swapBytes(x);
}

//=============================================================================
//                                TABLE DATA
//-----------------------------------------------------------------------------

static const struct {
    int            d_lineNum;  // source line number
    unsigned short d_value;
    unsigned short d_swapped;
} data16[] = {
    //LINE  VALUE   EXPECTED
    //----  ------  ------

    { L_,   0x0000, 0x0000 },
    { L_,   0xff00, 0x00ff },
    { L_,   0x00ff, 0xff00 },
    { L_,   0xffff, 0xffff },
    { L_,   0x8000, 0x0080 },
    { L_,   0x0100, 0x0001 },
    { L_,   0x0080, 0x8000 },
    { L_,   0x0001, 0x0100 },
    { L_,   0x1001, 0x0110 },
    { L_,   0x2002, 0x0220 },
    { L_,   0xc00d, 0x0dc0 },
    { L_,   0x1234, 0x3412 },
    { L_,   0x3412, 0x1234 },
    { L_,   0x1000, 0x0010 },
    { L_,   0x0800, 0x0008 },
    { L_,   0x7fff, 0xff7f },
    { L_,   0xfeff, 0xfffe },
    { L_,   0xefff, 0xffef },
    { L_,   0xf7ff, 0xfff7 },
};
enum { k_NUM_DATA16 = sizeof data16 / sizeof *data16 };

static const struct {
    int            d_lineNum;    // source line number
    unsigned int   d_value;
    unsigned int   d_swapped;
} data32[] = {
    //LINE  VALUE        EXPECTED
    //----  ----------   ----------

    { L_,            0,           0 },
    { L_,   0xff000000,  0x000000ff },
    { L_,   0x00ff0000,  0x0000ff00 },
    { L_,   0x0000ff00,  0x00ff0000 },
    { L_,   0x000000ff,  0xff000000 },
    { L_,   0xffffffff,  0xffffffff },
    { L_,   0x80000000,  0x00000080 },
    { L_,   0x01000000,  0x00000001 },
    { L_,   0x00800000,  0x00008000 },
    { L_,   0x00010000,  0x00000100 },
    { L_,   0x00008000,  0x00800000 },
    { L_,   0x11000011,  0x11000011 },
    { L_,   0x10000001,  0x01000010 },
    { L_,   0xab0000cd,  0xcd0000ab },
    { L_,   0x00abcd00,  0x00cdab00 },
    { L_,   0xabcd0000,  0x0000cdab },
    { L_,   0x00000100,  0x00010000 },
    { L_,   0x00000080,  0x80000000 },
    { L_,   0x00000001,  0x01000000 },
    { L_,   0x12345678,  0x78563412 },
    { L_,   0x78563412,  0x12345678 },
    { L_,   0x10000000,  0x00000010 },
    { L_,   0x08000000,  0x00000008 },
    { L_,   0x7fffffff,  0xffffff7f },
    { L_,   0xfeffffff,  0xfffffffe },
    { L_,   0xefffffff,  0xffffffef },
    { L_,   0xf7ffffff,  0xfffffff7 },
};
enum { k_NUM_DATA32 = sizeof data32 / sizeof *data32 };

static const struct {
    int    d_lineNum;
    Uint64 d_value;
    Uint64 d_swapped;
} data64[] = {
    //LINE  VALUE                  EXPECTED
    //----  ---------------------  ---------------------

    { L_,                       0,                     0 },
    { L_,   0xff00000000000000ULL, 0x00000000000000ffULL },
    { L_,   0x00ff000000000000ULL, 0x000000000000ff00ULL },
    { L_,   0x0000ff0000000000ULL, 0x0000000000ff0000ULL },
    { L_,   0x000000ff00000000ULL, 0x00000000ff000000ULL },
    { L_,   0x00000000ff000000ULL, 0x000000ff00000000ULL },
    { L_,   0x0000000000ff0000ULL, 0x0000ff0000000000ULL },
    { L_,   0x000000000000ff00ULL, 0x00ff000000000000ULL },
    { L_,   0x00000000000000ffULL, 0xff00000000000000ULL },
    { L_,   0xffffffffffffffffULL, 0xffffffffffffffffULL },
    { L_,   0x8000000000000000ULL, 0x0000000000000080ULL },
    { L_,   0x8000000000000008ULL, 0x0800000000000080ULL },
    { L_,   0x1000000000000001ULL, 0x0100000000000010ULL },
    { L_,   0x1100000000000011ULL, 0x1100000000000011ULL },
    { L_,   0x0100000000000000ULL, 0x0000000000000001ULL },
    { L_,   0x0080000000000000ULL, 0x0000000000008000ULL },
    { L_,   0x0001000000000000ULL, 0x0000000000000100ULL },
    { L_,   0x0000800000000000ULL, 0x0000000000800000ULL },
    { L_,   0x0000010000000000ULL, 0x0000000000010000ULL },
    { L_,   0x0000008000000000ULL, 0x0000000080000000ULL },
    { L_,   0x0000000100000000ULL, 0x0000000001000000ULL },
    { L_,   0x0000000080000000ULL, 0x0000008000000000ULL },
    { L_,   0x0000000001000000ULL, 0x0000000100000000ULL },
    { L_,   0x0000000000800000ULL, 0x0000800000000000ULL },
    { L_,   0x0000000000010000ULL, 0x0000010000000000ULL },
    { L_,   0x0000000000008000ULL, 0x0080000000000000ULL },
    { L_,   0x0000000000000100ULL, 0x0001000000000000ULL },
    { L_,   0x0000000000000080ULL, 0x8000000000000000ULL },
    { L_,   0x0000000000000001ULL, 0x0100000000000000ULL },
    { L_,   0x1234567890abcdefULL, 0xefcdab9078563412ULL },
    { L_,   0xefcdab9078563412ULL, 0x1234567890abcdefULL },
    { L_,   0x8000000000000000ULL, 0x0000000000000080ULL },
    { L_,   0x1000000000000000ULL, 0x0000000000000010ULL },
    { L_,   0x0800000000000000ULL, 0x0000000000000008ULL },
    { L_,   0x0100000000000000ULL, 0x0000000000000001ULL },
    { L_,   0x7fffffffffffffffULL, 0xffffffffffffff7fULL },
    { L_,   0xfeffffffffffffffULL, 0xfffffffffffffffeULL },
    { L_,   0xefffffffffffffffULL, 0xffffffffffffffefULL },
    { L_,   0xf7ffffffffffffffULL, 0xfffffffffffffff7ULL },
};
enum { k_NUM_DATA64 = sizeof data64 / sizeof *data64 };

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // TESTING CONSISTENCY OF COMPILE-TIME FLAGS
        //
        // Concerns:
        //: 1 When run in 'veryVerbose' moded, display which compile-time flags
        //:   are defined and which are not.
        //:
        //: 2 Verify that 'customSwapNN' and 'customSwapPNN' are never both
        //:   set for any value of 'NN' on any platform (note that it is
        //:   permissible for neither to be set).
        //
        // Plan:
        //: 1 Do '#ifdef'-driven output (C-1).
        //:
        //: 2 Construct an '#if' statement to check the consistency of the
        //:   macros being set.
        //
        // Testing:
        //   'BSLS_BYTEORDERUTIL_IMPL_CUSTOM_*'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING CONSISTENCY OF COMPILE-TIME FLAGS\n"
                              "=========================================\n");

        if (veryVerbose) {
#ifdef BSLS_BYTEORDERUTIL_IMPL_CUSTOM_16
            Q(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_16 defined);
#else
            Q(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_16 not defined);
#endif

#ifdef BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P16
            Q(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P16 defined);
#else
            Q(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P16 not defined);
#endif

#ifdef BSLS_BYTEORDERUTIL_IMPL_CUSTOM_32
            Q(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_32 defined);
#else
            Q(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_32 not defined);
#endif

#ifdef BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P32
            Q(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P32 defined);
#else
            Q(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P32 not defined);
#endif

#ifdef BSLS_BYTEORDERUTIL_IMPL_CUSTOM_64
            Q(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_64 defined);
#else
            Q(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_64 not defined);
#endif

#ifdef BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P64
            Q(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P64 defined);
#else
            Q(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P64 not defined);
#endif
        }

        // Check that no two custom flags are simultaneously set for how to
        // implement swapping for the same word width.

#if    (defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_16) &&                         \
        defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P16))                          \
    || (defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_32) &&                         \
        defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P32))                          \
    || (defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_64) &&                         \
        defined(BSLS_BYTEORDERUTIL_IMPL_CUSTOM_P64))
#error inconsistent compiler flags
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'mySwapBytes*'
        //
        // Concerns:
        //: 1 That all incarnations of 'customSwapNN' or 'customSwapPNN' or
        //:   'genericSwapNN' work.
        //
        // Plan:
        //: 1 Create the functions 'mySwapBytesNN' (before 'main', above),
        //:   which, if called on all platforms will result in all incarnations
        //:   of the functions under tests being tested.
        //:
        //: 2 Use the tables 'data16', 'data32', and 'data64', defined at file
        //:   scope above, to provide test data and its swapped values.
        //:
        //: 3 For word widths 16, 32, and 64
        //:   A Test swapping signed and unsigned types with 'mySwapBytesNN'.
        //:   B Test swapping the unsigned type with 'genericSwapNN'
        //:   C Use '#ifdef's to determine if 'long' or 'wchar_t' have the
        //:     designated word width under test, and if so, test it.
        //
        // Testing:
        //   'mySwapBytes[16,32,64}'
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'mySwapBytes'\n"
                              "=====================\n");

        bool wchar_t_tested = false;
        bool long_tested    = false;

        if (verbose) printf("Testing 16 Bit\n");
        {
            for (int ti = 0; ti < (1 << 16) + k_NUM_DATA16; ++ti) {
                unsigned short uValue   = (unsigned short) ti;
                unsigned short uSwapped = (unsigned short) ti;

                swapBytesInPlace(&uSwapped);

                short iValue   = (short) ti;
                short iSwapped = (short) ti;

                swapBytesInPlace(&iSwapped);

                if (ti >= (1 << 16)) {
                    uValue   = data16[ti - (1 << 16)].d_value;
                    uSwapped = data16[ti - (1 << 16)].d_swapped;
                    iValue   = data16[ti - (1 << 16)].d_value;
                    iSwapped = data16[ti - (1 << 16)].d_swapped;
                }

                if (veryVerbose) { PHEX_(uValue); PHEX(uSwapped); }

                LOOP3_ASSERT(ti, uSwapped, mySwapBytes(uValue),
                                              uSwapped == mySwapBytes(uValue));
                LOOP3_ASSERT(ti, uSwapped, Concrete::genericSwap16(uValue),
                                  uSwapped == Concrete::genericSwap16(uValue));

                LOOP_ASSERT(ti, uValue   == mySwapBytes(uSwapped));
                LOOP_ASSERT(ti, uValue   == Concrete::genericSwap16(uSwapped));

                LOOP_ASSERT(ti, iSwapped == mySwapBytes(iValue));
                LOOP_ASSERT(ti, iValue   == mySwapBytes(iSwapped));

                LOOP3_ASSERT(ti, iSwapped,
                                       (short) Concrete::genericSwap16(iValue),
                          iSwapped == (short) Concrete::genericSwap16(iValue));
                LOOP_ASSERT(ti,
                          iValue == (short) Concrete::genericSwap16(iSwapped));

#if 2 == BYTEORDERUTIL_SIZEOF_WCHAR_T
                ASSERT(2 == sizeof(wchar_t));

                const wchar_t wValue   = (wchar_t) uValue;
                const wchar_t wSwapped = (wchar_t) uSwapped;

                LOOP_ASSERT(ti, wSwapped == mySwapBytes(wValue));
                LOOP_ASSERT(ti, wValue   == mySwapBytes(wSwapped));

                wchar_t_tested = true;
#endif
            }
        }

        if (verbose) printf("Testing 32 Bit\n");
        {
            for (int ti = 0; ti < k_NUM_DATA32; ++ti) {
                const int    line     = data32[ti].d_lineNum;
                unsigned int uValue   = data32[ti].d_value;
                unsigned int uSwapped = data32[ti].d_swapped;
                int          iValue   = data32[ti].d_value;
                int          iSwapped = data32[ti].d_swapped;

                if (veryVerbose) { PHEX_(uValue); PHEX(uSwapped); }

                LOOP_ASSERT(line, uSwapped == mySwapBytes(uValue));
                LOOP_ASSERT(line, uValue   == mySwapBytes(uSwapped));

                LOOP_ASSERT(line, uSwapped == Concrete::genericSwap32(uValue));
                LOOP_ASSERT(line, uValue   ==
                                            Concrete::genericSwap32(uSwapped));

                LOOP_ASSERT(line, iSwapped == mySwapBytes(iValue));
                LOOP_ASSERT(line, iValue   == mySwapBytes(iSwapped));

#if 4 == BYTEORDERUTIL_SIZEOF_WCHAR_T
                ASSERT(4 == sizeof(wchar_t));

                const wchar_t wValue   = (wchar_t) uValue;
                const wchar_t wSwapped = (wchar_t) uSwapped;

                LOOP_ASSERT(line, wSwapped == mySwapBytes(wValue));
                LOOP_ASSERT(line, wValue   == mySwapBytes(wSwapped));

                wchar_t_tested = true;
#endif

#if 4 == BYTEORDERUTIL_SIZEOF_LONG
                ASSERT(4 == sizeof(long));

                const long          iLValue   = (long) uValue;
                const long          iLSwapped = (long) uSwapped;

                const unsigned long uLValue   = (unsigned long) uValue;
                const unsigned long uLSwapped = (unsigned long) uSwapped;

                LOOP_ASSERT(line, iLSwapped == mySwapBytes(iLValue));
                LOOP_ASSERT(line, iLValue   == mySwapBytes(iLSwapped));

                LOOP_ASSERT(line, iLValue   == mySwapBytes(iLSwapped));
                LOOP_ASSERT(line, iLSwapped == mySwapBytes(iLValue));

                LOOP_ASSERT(line, uLSwapped == mySwapBytes(uLValue));
                LOOP_ASSERT(line, uLValue   == mySwapBytes(uLSwapped));

                LOOP_ASSERT(line, uLValue   == mySwapBytes(uLSwapped));
                LOOP_ASSERT(line, uLSwapped == mySwapBytes(uLValue));

                long_tested = true;
#endif
            }
        }

        ASSERT(wchar_t_tested);

        if (verbose) printf("Testing 64 Bit\n");
        {
            for (int ti = 0; ti < k_NUM_DATA64; ++ti) {
                const int    line     = data64[ti].d_lineNum;
                const Uint64 uValue   = data64[ti].d_value;
                const Uint64 uSwapped = data64[ti].d_swapped;
                const Int64  iValue   = data64[ti].d_value;
                const Int64  iSwapped = data64[ti].d_swapped;

                if (veryVerbose) { PHEX_(uValue); PHEX(uSwapped); }

                LOOP_ASSERT(line, uSwapped == mySwapBytes(uValue));
                LOOP_ASSERT(line, uValue   == mySwapBytes(uSwapped));

                LOOP_ASSERT(line, uSwapped == Concrete::genericSwap64(uValue));
                LOOP_ASSERT(line, uValue   ==
                                            Concrete::genericSwap64(uSwapped));

                LOOP_ASSERT(line, iSwapped == mySwapBytes(iValue));
                LOOP_ASSERT(line, iValue   == mySwapBytes(iSwapped));

#if 8 == BYTEORDERUTIL_SIZEOF_LONG
                ASSERT(8 == sizeof(long));

                const long          iLValue   = (long) uValue;
                const long          iLSwapped = (long) uSwapped;

                const unsigned long uLValue   = (unsigned long) uValue;
                const unsigned long uLSwapped = (unsigned long) uSwapped;

                LOOP_ASSERT(line, iLSwapped == mySwapBytes(iLValue));
                LOOP_ASSERT(line, iLValue   == mySwapBytes(iLSwapped));

                LOOP_ASSERT(line, iLValue   == mySwapBytes(iLSwapped));
                LOOP_ASSERT(line, iLSwapped == mySwapBytes(iLValue));

                LOOP_ASSERT(line, uLValue   == mySwapBytes(uLSwapped));
                LOOP_ASSERT(line, uLSwapped == mySwapBytes(uLValue));

                LOOP_ASSERT(line, uLValue   == mySwapBytes(uLSwapped));
                LOOP_ASSERT(line, uLSwapped == mySwapBytes(uLValue));

                long_tested = true;
#endif
            }
        }

        ASSERT(long_tested);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'genericSwap*' & SOUNDNESS OF TABLES
        //
        // Concerns:
        //: 1 Test that the tables 'data16', 'data32', and 'data64', accurately
        //:   provide swapped values for their types.
        //:
        //: 2 Test that the tables are of significant length (> 10 entries).
        //:
        //: 3 Test that 'genericSwapNN' works correctly.
        //
        // Plan:
        //: 1 Create a template function 'swapBytesInPlace' that, given a
        //:   pointer to an object of any integral type, will reverse the byte
        //:   order of the object.
        //:
        //: 2 For each table:
        //:   o Assert that the table length is >10.
        //:   o Traverse the table, apply 'swapBytesInPlace' to verify the
        //:     expected swapped value matches the swapped value.
        //:   o Call 'Concrete::genericSwapNN' on signed and unsigned values
        //:     and confirm the result matches the expected value.
        //
        // Testing:
        //   'genericSwap{16,32,64}'
        // --------------------------------------------------------------------

        if (verbose) printf(
                          "\nTESTING 'genericSwap*' & SOUNDNESS OF TABLE\n"
                            "=============================================\n");

        if (verbose) printf("Testing 16 Bit\n");
        {
            ASSERT(k_NUM_DATA16 > 10);

            for (int ti = 0; ti < k_NUM_DATA16; ++ti) {
                const int            line     = data16[ti].d_lineNum;
                const unsigned short uValue   = data16[ti].d_value;
                const unsigned short uSwapped = data16[ti].d_swapped;
                const short          iValue   = data16[ti].d_value;
                const short          iSwapped = data16[ti].d_swapped;

                if (veryVerbose) { PHEX_(uValue); PHEX(uSwapped); }

                unsigned short uVerify = uValue;
                swapBytesInPlace(&uVerify);
                ASSERT(uSwapped == uVerify);

                LOOP_ASSERT(line, uSwapped == Concrete::genericSwap16(uValue));
                LOOP_ASSERT(line, static_cast<unsigned short>(iSwapped) ==
                                              Concrete::genericSwap16(iValue));
            }
        }

        if (verbose) printf("Testing 32 Bit\n");
        {
            ASSERT(k_NUM_DATA32 > 10);

            for (int ti = 0; ti < k_NUM_DATA32; ++ti) {
                const int          line     = data32[ti].d_lineNum;
                const unsigned int uValue   = data32[ti].d_value;
                const unsigned int uSwapped = data32[ti].d_swapped;
                const int          iValue   = data32[ti].d_value;
                const int          iSwapped = data32[ti].d_swapped;

                if (veryVerbose) { PHEX_(uValue); PHEX(uSwapped); }

                unsigned int uVerify = uValue;
                swapBytesInPlace(&uVerify);
                ASSERT(uSwapped == uVerify);

                LOOP_ASSERT(line, uSwapped == Concrete::genericSwap32(uValue));
                LOOP_ASSERT(line, iSwapped ==
                                        (int) Concrete::genericSwap32(iValue));
            }
        }

        if (verbose) printf("Testing 64 Bit\n");
        {
            ASSERT(k_NUM_DATA64 > 10);

            for (int ti = 0; ti < k_NUM_DATA64; ++ti) {
                const int    line     = data64[ti].d_lineNum;
                const Uint64 uValue   = data64[ti].d_value;
                const Uint64 uSwapped = data64[ti].d_swapped;
                const Int64  iValue   = data64[ti].d_value;
                const Int64  iSwapped = data64[ti].d_swapped;

                if (veryVerbose) { PHEX_(uValue); PHEX(uSwapped); }

                Uint64 uVerify = uValue;
                swapBytesInPlace(&uVerify);
                ASSERT(uSwapped == uVerify);

                LOOP_ASSERT(line, uSwapped == Concrete::genericSwap64(uValue));
                LOOP_ASSERT(line, iSwapped ==
                                      (Int64) Concrete::genericSwap64(iValue));
            }
        }
      } break;
      default: {
        std::fprintf(stderr, "WARNING: CASE '$d' NOT FOUND.\n");
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        std::fprintf(stderr, "Error, non-zero test status = %d.\n",testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2014
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
