// bsls_byteorderutil.h                                               -*-C++-*-
#ifndef INCLUDED_BSLS_BYTEORDERUTIL
#define INCLUDED_BSLS_BYTEORDERUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide byte-order swapping functions.
//
//@CLASSES:
//   bsls::ByteOrderUtil: namespace for byte-order swapping functions
//
//@SEE ALSO: bsls_byteorder
//
//@DESCRIPTION: This component provides a utility 'struct',
// 'bsls::ByteOrderUtil', that contains a suite of static functions for
// reversing the byte order of integral types.  The functions
// 'swapByteOrder{16,32,64}' reverse the byte order of words having the
// indicated widths (in bits), while the overloaded function 'swapBytes' will
// swap the bytes of any integral type passed to it, returning the same type it
// is passed.
//
///Usage
///-----
// In this example we demonstrate the use of different overloads of the
// 'swapBytes' function.
//
// First we 'typedef' a shorthand to the namespace 'struct':
//..
//  typedef bsls::ByteOrderUtil Util;
//..
// Then, we demonstrate reversing the bytes of an 'unsigned short':
//..
//  unsigned short us = 0x1234;
//  assert(0x3412 == Util::swapBytes(us));
//..
// Next, we do a signed 'short:
//..
//  short ss = 0x4321;
//  assert(0x2143 == Util::swapBytes(ss));
//..
// Then, we reverse an 'unsigned int':
//..
//  unsigned int ui = 0x01020304;
//  assert(0x04030201 == Util::swapBytes(ui));
//..
// Next, we reverse the bytes of a 32-bit signed integer:
//..
//  int si = 0x11223344;
//  assert(0x44332211 == Util::swapBytes(si));
//..
// Now, we perform the transform on a 64-bit unsigned:
//..
//  bsls::Types::Uint64 ui64 = 0x0102030405060708ULL;
//  assert(0x0807060504030201ULL == Util::swapBytes(ui64));
//..
// Finally, we do a 64-bit signed integer:
//..
//  bsls::Types::Int64 i64 = 0x0a0b0c0d0e0f0102LL;
//  assert(0x02010f0e0d0c0b0aLL == Util::swapBytes(i64));
//..

#ifndef INCLUDED_BSLS_BYTEORDERUTIL_IMPL
#include <bsls_byteorderutil_impl.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {
namespace bsls {

                            // ====================
                            // struct ByteOrderUtil
                            // ====================

struct ByteOrderUtil {
    // This utility struct provides a namespace for functions used for
    // reversing the byte order of values having integral type.

    // CLASS METHODS
    static bool           swapBytes(bool           x);
    static char           swapBytes(char           x);
    static unsigned char  swapBytes(unsigned char  x);
    static signed char    swapBytes(signed char    x);
    static wchar_t        swapBytes(wchar_t        x);
    static short          swapBytes(short          x);
    static unsigned short swapBytes(unsigned short x);
    static int            swapBytes(int            x);
    static unsigned int   swapBytes(unsigned int   x);
    static long           swapBytes(long           x);
    static unsigned long  swapBytes(unsigned long  x);
    static Types::Uint64  swapBytes(Types::Uint64  x);
    static Types::Int64   swapBytes(Types::Int64   x);
        // Return the value that results from reversing the order of the bytes
        // in the specified 'x'.

    static unsigned short swapBytes16(unsigned short x);
        // Return the value that results from reversing the order of the bytes
        // in the specified 'x'.

    static unsigned int   swapBytes32(unsigned int   x);
        // Return the value that results from reversing the order of the bytes
        // in the specified 'x'.

    static Types::Uint64  swapBytes64(Types::Uint64  x);
        // Return the value that results from reversing the order of the bytes
        // in the specified 'x'.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                            // --------------------
                            // struct ByteOrderUtil
                            // --------------------

// CLASS METHODS
inline
bool ByteOrderUtil::swapBytes(bool x)
{
    return ByteOrderUtil_Impl<sizeof(x)>::swapBytes(x);
}

inline
char ByteOrderUtil::swapBytes(char x)
{
    return ByteOrderUtil_Impl<sizeof(x)>::swapBytes(x);
}

inline
unsigned char ByteOrderUtil::swapBytes(unsigned char x)
{
    return ByteOrderUtil_Impl<sizeof(x)>::swapBytes(x);
}

inline
signed char ByteOrderUtil::swapBytes(signed char x)
{
    return ByteOrderUtil_Impl<sizeof(x)>::swapBytes(x);
}

inline
wchar_t ByteOrderUtil::swapBytes(wchar_t x)
{
    return ByteOrderUtil_Impl<sizeof(x)>::swapBytes(x);
}

inline
short ByteOrderUtil::swapBytes(short x)
{
    return ByteOrderUtil_Impl<sizeof(x)>::swapBytes(x);
}

inline
unsigned short ByteOrderUtil::swapBytes(unsigned short x)
{
    return ByteOrderUtil_Impl<sizeof(x)>::swapBytes(x);
}

inline
int ByteOrderUtil::swapBytes(int x)
{
    return ByteOrderUtil_Impl<sizeof(x)>::swapBytes(x);
}

inline
unsigned int ByteOrderUtil::swapBytes(unsigned int x)
{
    return ByteOrderUtil_Impl<sizeof(x)>::swapBytes(x);
}

inline
long ByteOrderUtil::swapBytes(long x)
{
    return ByteOrderUtil_Impl<sizeof(x)>::swapBytes(x);
}

inline
unsigned long ByteOrderUtil::swapBytes(unsigned long x)
{
    return ByteOrderUtil_Impl<sizeof(x)>::swapBytes(x);
}

inline
bsls::Types::Uint64 ByteOrderUtil::swapBytes(bsls::Types::Uint64 x)
{
    return ByteOrderUtil_Impl<sizeof(x)>::swapBytes(x);
}

inline
bsls::Types::Int64 ByteOrderUtil::swapBytes(bsls::Types::Int64 x)
{
    return ByteOrderUtil_Impl<sizeof(x)>::swapBytes(x);
}

inline
unsigned short
ByteOrderUtil::swapBytes16(unsigned short x)
{
    BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(2 == sizeof(x));

    return ByteOrderUtil_Impl<2>::swapBytes(x);
}

inline
unsigned int
ByteOrderUtil::swapBytes32(unsigned int x)
{
    BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(4 == sizeof(x));

    return ByteOrderUtil_Impl<4>::swapBytes(x);
}

inline
bsls::Types::Uint64
ByteOrderUtil::swapBytes64(bsls::Types::Uint64 x)
{
    BSLS_BYTEORDERUTIL_IMPL_COMPILE_TIME_ASSERT(8 == sizeof(x));

    return ByteOrderUtil_Impl<8>::swapBytes(x);
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2014
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
