// bdldfp_binaryintegraldecimalimputil.h                               -*-C++-*-
#ifndef INCLUDED_BDLDFP_BINARYINTEGRALDECIMALIMPUTIL
#define INCLUDED_BDLDFP_BINARYINTEGRALDECIMALIMPUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$")

//@PURPOSE: Utilities for working with Binary Integral Decimal representation.
//
//@CLASSES:
//  bdldfp::BinaryIntegralDecimalImpUtil: namespace for BID functions.
//
//@SEE_ALSO: bdldfp_decimal, bdldfp_decimalplatform,
//  bdldfp_denselypackeddecimalimputil
//
//@DESCRIPTION: This component provides a namespace,
// 'bdldfp::BinaryIntegralDecimalImpUtil', that supplies the necessary types
// for storing a Binary Integral Decimal (BID) representation of a decimal
// floating point value.
//
///Usage
///-----
// This section shows the intended use of this component.
//
///Example 1: none
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMALPLATFORM
#include <bdldfp_decimalplatform.h>
#endif

#ifndef INCLUDED_BDLDFP_UINT128
#include <bdldfp_uint128.h>
#endif

#ifndef INCLUDED_DENSELYPACKEDDECIMALIMPUTIL
#include <bdldfp_denselypackeddecimalimputil.h>
#endif

namespace BloombergLP {
namespace bdldfp {

                        // ==================================
                        // class BinaryIntegralDecimalImpUtil
                        // ==================================

struct BinaryIntegralDecimalImpUtil {
    // This 'struct' provides a namespace for functions that provide common DPD
    // formatted decimal floating point.

    // TYPES
    struct StorageType32  { unsigned           int d_raw; };
    struct StorageType64  { unsigned long long int d_raw; };
    struct StorageType128 { bdldfp::Uint128        d_raw; };

    struct Tuple {
        bool            negative; // Sign bit, which is true if negative
        signed int      exponent;
        bdldfp::Uint128 mantissa;
    };

    // CLASS METHODS
    static Tuple decompose(StorageType32  bid);
    static Tuple decompose(StorageType64  bid);
    static Tuple decompose(StorageType128 bid);

    static
    DenselyPackedDecimalImpUtil::StorageType32  binaryIntegralToDenselyPacked(
                                                           StorageType32  dpd);
    static
    DenselyPackedDecimalImpUtil::StorageType64  binaryIntegralToDenselyPacked(
                                                           StorageType64  dpd);
    static
    DenselyPackedDecimalImpUtil::StorageType128 binaryIntegralToDenselyPacked(
                                                           StorageType128 dpd);
        // TBD

    static StorageType32  denselyPackedToBinaryIntegral(
                              DenselyPackedDecimalImpUtil::StorageType32  dpd);
    static StorageType64  denselyPackedToBinaryIntegral(
                              DenselyPackedDecimalImpUtil::StorageType64  dpd);
    static StorageType128 denselyPackedToBinaryIntegral(
                              DenselyPackedDecimalImpUtil::StorageType128 dpd);
        // TBD
};

namespace detail {
template<unsigned SIZE>
struct DecimalTraits;

template<>
struct DecimalTraits<32> {
    static const unsigned signShift = 31;
    static const unsigned exponentShift = 20;
    static const unsigned mantissaShift = 12;

    static const unsigned exponentSize = signShift - exponentShift;
    static const unsigned exponentMask = (1 << exponentSize) - 1;

    static const unsigned shorterMantissa = exponentSize;
    static const unsigned longerMantissa = exponentSize - 2;

    static const unsigned exponentFieldSize = signShift - exponentShift;

    static const unsigned bias = 101;
};

template<>
struct DecimalTraits<64> {
    static const unsigned signShift = 63;
    static const unsigned exponentShift = 50;
    static const unsigned mantissaShift = 14;

    static const unsigned exponentSize = signShift - exponentShift;
    static const unsigned exponentMask = (1 << exponentSize) - 1;

    static const unsigned shorterMantissa = exponentSize;
    static const unsigned longerMantissa = exponentSize - 2;

    static const unsigned exponentFieldSize = signShift - exponentShift;

    static const unsigned bias = 398;
};

template<>
struct DecimalTraits<128> {
    static const unsigned signShift = 127;
    static const unsigned exponentShift = 110;
    static const unsigned mantissaShift = 18;

    static const unsigned exponentSize = signShift - exponentShift;
    static const unsigned exponentMask = (1 << exponentSize) - 1;

    static const unsigned shorterMantissa = exponentSize;
    static const unsigned longerMantissa = exponentSize - 2;

    static const unsigned exponentFieldSize = signShift - exponentShift;

    static const unsigned bias = 6176;
};

template<typename STORAGE>
inline
BinaryIntegralDecimalImpUtil::Tuple decompose(const STORAGE bid)
{
    BinaryIntegralDecimalImpUtil::Tuple result;

    typedef DecimalTraits<sizeof(STORAGE)> Traits;

    result.negative = bid.d_raw >> Traits::signShift;

    unsigned fullExponentField =
                   (bid.d_raw >> Traits::exponentShift) & Traits::exponentMask;

    unsigned combo = fullExponentField >> (Traits::exponentFieldSize - 5);

    if ((combo >> 3) != 0x3) {
        result.exponent = fullExponentField >> 3;
        result.mantissa =
                     (bid << Traits::longerMantissa) >> Traits::longerMantissa;
    }
    else {
        result.exponent = ((fullExponentField & (Traits::exponentMask >> 2))
                                                                      >> 1);
        result.mantissa = ((bid.d_raw << Traits::shorterMantissa)
                                      >> Traits::shorterMantissa)
                            | (0x8llu << Traits::exponentShift);
    }

    result.exponent -= Traits::bias;

    return result;
}

unsigned
topDigitAsBinary32(unsigned digit)
{
    return digit * 1000000;
}

unsigned
topDigitAsBinary64(unsigned digit)
{
    return digit * 1000000000000000ll;
}

bdldfp::Uint128
topDigitAsBinary128(unsigned digit)
{
    switch (digit) {
      case 0:
        return 0;

      case 1:
        return bdldfp::Uint128(0x0000314dc6448d93llu, 0x38c15b0a00000000llu);

      case 2:
        return bdldfp::Uint128(0x0000629b8c891b26llu, 0x7182b61400000000llu);

      case 3:
        return bdldfp::Uint128(0x000093e952cda8b9llu, 0xaa44111e00000000llu);

      case 4:
        return bdldfp::Uint128(0x0000c5371912364cllu, 0xe3056c2800000000llu);

      case 5:
        return bdldfp::Uint128(0x0000f684df56c3e0llu, 0x1bc6c73200000000llu);

      case 6:
        return bdldfp::Uint128(0x000127d2a59b5173llu, 0x5488223c00000000llu);

      case 7:
        return bdldfp::Uint128(0x000159206bdfdf06llu, 0x8d497d4600000000llu);

      case 8:
        return bdldfp::Uint128(0x00018a6e32246c99llu, 0xc60ad85000000000llu);

      case 9:
        return bdldfp::Uint128(0x0001bbbbf868fa2cllu, 0xfecc335a00000000llu);
    }
}

inline bdldfp::Uint128
multiplyBy10(bdldfp::Uint128 value)
{
    return ( value << 1 ) + ( value << 3 );
}

inline bdldfp::Uint128
multiplyBy100(bdldfp::Uint128 value)
{
    return ( value << 2 ) + ( value << 5 ) + ( value << 6 );
}

inline bdldfp::Uint128
multiplyBy1000(bdldfp::Uint128 value)
{
    return multiplyBy100( multiplyBy10( value ) );
}
} // close detail namespace

inline
BinaryIntegralDecimalImpUtil::Tuple
BinaryIntegralDecimalImpUtil::decompose(
                         const BinaryIntegralDecimalImpUtil::StorageType32 bid)
{
    return detail::decompose(bid);
}

inline
BinaryIntegralDecimalImpUtil::Tuple
BinaryIntegralDecimalImpUtil::decompose(
                         const BinaryIntegralDecimalImpUtil::StorageType64 bid)
{
    return detail::decompose(bid);
}

inline
BinaryIntegralDecimalImpUtil::Tuple
BinaryIntegralDecimalImpUtil::decompose(
                        const BinaryIntegralDecimalImpUtil::StorageType128 bid)
{
    return detail::decompose(bid);
}

inline
DenselyPackedDecimalImpUtil::StorageType32
BinaryIntegralDecimalImpUtil::binaryIntegralToDenselyPacked(
                         const BinaryIntegralDecimalImpUtil::StorageType32 bid)
{
    Tuple tuple = decompose(bid);

    int significand = tuple.mantissa * (tuple.negative ? -1 : +1);

    return DenselyPackedDecimalImpUtil::makeDecimalRaw32(significand,
                                                         tuple.exponent);
}

inline
DenselyPackedDecimalImpUtil::StorageType64
BinaryIntegralDecimalImpUtil::binaryIntegralToDenselyPacked(
                         const BinaryIntegralDecimalImpUtil::StorageType64 bid)
{
    Tuple tuple = decompose(bid);

    long long int significand = tuple.mantissa * (tuple.negative ? -1 : +1);

    return DenselyPackedDecimalImpUtil::makeDecimalRaw64(significand,
                                                         tuple.exponent);
}

inline
DenselyPackedDecimalImpUtil::StorageType128
BinaryIntegralDecimalImpUtil::binaryIntegralToDenselyPacked(
                         const BinaryIntegralDecimalImpUtil::StorageType128 bid)
{
    DenselyPackedDecimalImpUtil::StorageType128 result;
    result.d_raw = 0;

    Tuple tuple = decompose(bid);

    // TODO

    return result;
}

inline
BinaryIntegralDecimalImpUtil::StorageType32
BinaryIntegralDecimalImpUtil::denselyPackedToBinaryIntegral(
                          const DenselyPackedDecimalImpUtil::StorageType32 dpd)
{
    DenselyPackedDecimalImpUtil::StorageType32 result;
    result.d_raw = 0;

    typedef detail::DecimalTraits<32> Traits;

    unsigned fullExponentField =
                   (dpd.d_raw >> Traits::exponentShift) & Traits::exponentMask;

    unsigned combo = fullExponentField >> (Traits::exponentFieldSize - 5);

    unsigned topDigit;
    signed exponent;
    if ((combo & 0x18 == 0x10) || (combo & 0x10 == 0)) {
        topDigit =  combo & 0x7;
        exponent = (combo >> 3) << (Traits::mantissaShift - 6) |
                   (fullExponentField & (Traits::exponentMask >> 5));
    }
    else {
        topDigit = (combo & 1) | 0x8;
        exponent = (((combo >> 1) & 3) << (Traits::mantissaShift - 6))
                 | (fullExponentField   & (Traits::exponentMask >> 5));
    }

    unsigned decimalSignificand = (dpd << Traits::mantissaShift)
                                       >> Traits::mantissaShift;

    unsigned significand = 0;

    for (int i = 0; i < 2; ++i) {
        significand += (1000 * i) *
         DenselyPackedDecimalImpUtil::decodeDeclet(decimalSignificand & 0x3FF);
        decimalSignificand >>= 10;
    }

    significand += result.d_rawdetail::topDigitAsBinary32(topDigit);

    result.d_raw |= significand & 0xFFFFF;

    result.d_raw |= (dpd.d_raw >> Traits::signShift) << Traits::signShift;

    return result;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
