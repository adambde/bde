// bdldfp_decimalconvertutil.h                                        -*-C++-*-
#ifndef INCLUDED_BDLDFP_DECIMALCONVERTUTIL
#define INCLUDED_BDLDFP_DECIMALCONVERTUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$")

//@PURPOSE: Provide decimal floating-point conversion functions.
//
//@CLASSES:
//  bdldfp::DecimalConvertUtil: Namespace for decimal FP conversion functions
//
//@SEE ALSO: bdldfp_decimal, bdldfp_decimalplatform
//
//@DESCRIPTION: This component provides namespace,
// 'bdldfp::DecimalConvertUtil', containing functions that are able to convert
// between the native decimal types of the platform and various other possible
// representations, such as binary floating-point, network encoding formats.
//
///Encoding Formats
///----------------
// This utility contains functions to encode decimal values to and from three
// different encoding formats:
//
//: o the IEEE decimal interchange format using decimal encoding for the
//:   significant (also known as the Densely Packed Decimal format, see IEEE
//:   754 - 2008, section 3.5.2, for more details)
//:
//: o the multi-width encoding format, which is a custom format that can encode
//:   subsets of decimal values using a smaller number of bytes
//:
//: o the variable-width encoding format, which is a custom format that is
//:   similar to the multi-width encoding format with the main difference being
//:   that it self describes its own width
//
// 64-bit decimal values encoded by the IEEE decimal interchange format always
// uses 8 bytes, which can be inefficient. The two custom encoding formats
// provided by this to enable more space efficient encoding of values commonly
// encountered by financial applications.
//
// In the full IEEE encoding, 50 bits are used for the trailing bits of the
// mantissa, 13 bit is used for the combination field (exponent + special
// states to indicate NaN and Inf values + leading bits of the mantissa), and 1
// bit is used for the significant.  The basic idea for the custom encoding
// formats is that the mantissa and exponent of many values (in typical
// financial applications) can fit into fewer bits than those provided by the
// full encoding.  We can define a set of narrow formats to encode these
// smaller values without loss of precision.  For example, a ticker values less
// than 100 dollars with a 2 decimal places of precision can be encoded using a
// 2 bytes encoding, using no sign bit, 3 bits for the exponent, and 13 bits
// for the mantissa.
//
///IEEE Decimal Interchange Format
///- - - - - - - - - - - - - - - -
// The IEEE decimal interchange format is defined by the IEEE standard.  64 bit
// decimal values encoded by this format always uses 8 bytes.  The
// 'decimalFromNetwork' and 'decimalToNetwork' functions can be used encode to
// and decode from this format.
//
///Multi-Width Encoding Format
///- - - - - - - - - - - - - -
// The multi-width encoding format uses a set of narrow encoding formats having
// sizes smaller than that used by the for IEEE format.  Each of the narrower
// encoding format is used to encode a subset of values that can be represented
// by the full format.  The following configuration is used to encode 64-bit
// decimal values:
//
//..
// |------|----------|----------|-----|----------|----------------|
// | size | S (bits) | E (bits) |   B | T (bits) | max signficant |
// |------|----------|----------|-----|----------|----------------|
// |   1* |        0 |        1 |  -2 |        7 |            127 |
// |   2  |        0 |        3 |  -4 |       13 |           8191 |
// |   3  |        0 |        3 |  -4 |       21 |        2097151 |
// |   4  |        1 |        5 | -16 |       26 |       67108863 |
// |   5  |        1 |        5 | -16 |       34 |    17179869183 |
// |------|-------------------------------------------------------|
// |    8 |            FULL IEEE INTERCHANGE FORMAT**             |
// |------|-------------------------------------------------------|
//
// S = sign, E = exponent, B = bias, T = significant
//
// * 1 byte encoding will be supported by the decoder but not the encoder. This
//   is done due to the relatively large performance impact of adding the 1
//   byte encoding to the encoder (10%). Perserving the encoding size in the
//   decoder allows us to easily enable this encoding size at a later time.
//
// ** If the value to be encoded can not fit in the 5-byte encoding or is -Inf,
//    +Inf, or Nan, then the full 8-byte IEEE format will be used.
//..
//
// Since the multi-width encoding format consists of subformats having varying
// widths, the size of the subformat used must be supplied long with the
// encoding to the decode function.  This is not required for either the IEEE
// format or the variable-width encoding format.
//
// The 'decimal64ToMultiWidthEncoding' and 'decimal64FromMultiWidthEncoding'
// can be used to encode to and decode from this format.  Currently, only
// 64-bit decimal values are supported by this encoding format.
//
///Variable-Width Encoding Formats
///- - - - - - - - - - - - - - - -
// The variable-width encoding format can encode decimal values using a
// variable number of bytes, similar to the multi-width encoding format.  The
// difference is that the variable-width encoding format can self-describe its
// own size using special state (typically, predicate bits), so the decode
// function does not require the size of the encoding to work.  The following
// configuration is used to encode 64-bit decimal values:
//
//..
// |------|----------|---|---|-----|----|-----------------|
// | size |        P | S | E |   B |  T | max significant |
// |------|----------|---|---|-----|----|-----------------|
// |    2 |        0 | 0 | 2 |  -2 | 13 |            8191 |
// |    3 |       10 | 0 | 3 |  -4 | 19 |          524287 |
// |    4 |       11 | 1 | 5 | -16 | 24 |        16777215 |
// |------|----------|------------------------------------|
// |    9 | 11111111 |        FULL IEEE FORMAT*           |
// |------|----------|------------------------------------|
//
// P = predicate (bit values)
// S = sign (bits), E = exponent (bits), B = bias
// T = significant (bits)
//
// * If the value to be encoded can not fit in the 4-byte encoding or is -Inf,
//   +Inf, or Nan, then the full 8-byte IEEE format will be used prefixed by a
//   1 byte predicate having the value of 0xFF.
//..
//
// The 'decimal64ToVariableWidthEncoding' and
// 'decimal64FromVariableWidthEncoding' can be used to encode to and decode
// from this format.  Currently, only 64-bit decimal values are supported by
// this encoding format.
//
///Usage
///-----
// This section shows the intended use of this component.
//
///Example 1: Sending Decimals As Octets Using Network Format
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose you have two communicating entities (programs) that talk to each
// other using a binary (as opposed to text) protocol.  In such protocol it is
// important to establish a so-called network format, and convert to and from
// that format in the protocol layer.  The sender (suppose that it is an IBM
// server that has just finished an expensive calculation involving millions
// of numbers and needs to send the result to its client) will need to convert
// the data to network format before sending:
//..
//  unsigned char   msgbuffer[256];
//  BDEC::Decimal64 number(BDLDFP_DECIMAL_DD(1.234567890123456e-42));
//  unsigned char   expected[] = {
//                            0x25, 0x55, 0x34, 0xb9, 0xc1, 0xe2, 0x8e, 0x56 };
//
//  unsigned char *next = msgbuffer;
//  next = bdldfp::DecimalConvertUtil::decimalToNetwork(next, number);
//
//  assert(memcmp(msgbuffer, expected, sizeof(number)) == 0);
//..
// The receiver/client shall then restore the number from network format:
//..
//  unsigned char   msgbuffer[] ={
//                            0x25, 0x55, 0x34, 0xb9, 0xc1, 0xe2, 0x8e, 0x56 };
//  BDEC::Decimal64 number;
//  BDEC::Decimal64 expected(BDLDFP_DECIMAL_DD(1.234567890123456e-42));
//
//  unsigned char *next = msgbuffer;
//  next = bdldfp::DecimalConvertUtil::decimalFromNetwork(number, next);
//
//  assert(number == expected);
//..
//
///Example 2: Storing/Sending Decimals In Binary Floating-Point
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose you have two communicating entities (programs) that talk to each
// other using a legacy protocol that employs binary floating-point formats to
// send/receive numbers.  So your application layer will have to store the
// decimal into a binary FP variable, ensure that it can be restored (in other
// words that it has "fit" into the binary type) when sending, and restore the
// decimal number (from the binary type) when receiving:
//..
//  const BDEC::Decimal64 number(BDLDFP_DECIMAL_DD(1.23456789012345e-42));
//
//  typedef bdldfp::DecimalConvertUtil Util;
//  double dbl = Util::decimalToDouble(number);
//
//  if (Util::decimal64FromDouble(dbl) != number) {
//      // Do what is appropriate for the application
//  }
//..
// Note that the above assert would probably be a lot more complicated if
// statement in production code.  It may actually be acceptable to put the
// decimal onto the wire with certain amount of imprecision.
//
// The receiver would then restore the number using the appropriate
// 'decimal64FromDouble' function:
//..
//  BDEC::Decimal64 restored = Util::decimal64FromDouble(dbl);
//
//  assert(number == restored);
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMAL
#include <bdldfp_decimal.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMALUTIL
#include <bdldfp_decimalutil.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMALCONVERTUTIL_DECNUMBER
#include <bdldfp_decimalconvertutil_decnumber.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMALCONVERTUTIL_IBMXLC
#include <bdldfp_decimalconvertutil_ibmxlc.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMALCONVERTUTIL_INTELDFP
#include <bdldfp_decimalconvertutil_inteldfp.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMALIMPUTIL
#include <bdldfp_decimalimputil.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif


namespace BloombergLP {
namespace bdldfp {

#define BDLDFP_DU_INTELDFP_EXPONENT_SHIFT_SMALL64 53
#define BDLDFP_DU_INTELDFP_SPECIAL_ENCODING_MASK64 0x6000000000000000ull

#define BDLDFP_DU_INTELDFP_EXPONENT_MASK64 0x3ff
#define BDLDFP_DU_INTELDFP_SMALL_COEFF_MASK64 0x001fffffffffffffull

                        // ========================
                        // class DecimalConvertUtil
                        // ========================

struct DecimalConvertUtil {
    // This 'struct' provides a namespace for utility functions that convert
    // between the decimal floating-point types of 'bdldfp_decimal' and various
    // other formats.

  private:
#ifdef BDLDFP_DECIMALPLATFORM_DECNUMBER
    typedef DecimalConvertUtil_DecNumber Imp;
#elif defined(BDLDFP_DECIMALPLATFORM_INTELDFP)
    typedef DecimalConvertUtil_IntelDfp  Imp;
#elif defined(BDLDFP_DECIMALPLATFORM_C99_TR)
    typedef DecimalConvertUtil_IbmXlc    Imp;
#else
    BSLMF_ASSERT(false);
#endif

    // PRIVATE CLASS METHODS
    static int decimal64ToUnpackedSpecial(
                                         bool                *isNegative,
                                         int                 *biasedExponent,
                                         bsls::Types::Uint64 *mantissa,
                                         bdldfp::Decimal64    value);
        // If the specified 'value' is NaN, +infinity, -infinity, or its
        // unbiased exponent is 384, return a non-zero value and leave all
        // output parameters unmodified.  Otherwise, partition the specified
        // 'value' into sign, biased exponent, and mantissa compartments, and
        // load the corresponding values into the specified 'isNegative',
        // biasedExponent', and 'mantissa'. Return 0.  Note that a non-zero
        // value does not indicate that 'value' can not be partitioned, just
        // that it can not be partitioned by this function.  Also note that the
        // bias for 'Decimal64' is 398.

    static bdldfp::Decimal64 decimal64FromUnpackedSpecial(
                                                bool                isNegative,
                                                bsls::Types::Uint64 mantissa,
                                                int                 exponent);
        // Return a 'Decimal64' object that has the specified 'mantissa',
        // 'exponent', and a sign based on the specified 'isNegative'.  The
        // behavior is undefined unless 'isNegative', 'mantissa', and exponent
        // were originally obtained from 'decimal64ToUnpackedSpecial'.


    static bdldfp::Decimal64 decimal64FromUnpackedSpecial(int mantissa,
                                                          int exponent);
        // Return a 'Decimal64' object that has the specified 'mantissa', and
        // 'exponent'.  The behavior is undefined unless 'mantissa' and
        // exponent were originally obtained from 'decimal64ToUnpackedSpecial'.

  public:
    // CLASS METHODS

                        // decimalToDouble functions

    static double decimal32ToDouble (Decimal32  decimal);
    static double decimal64ToDouble (Decimal64  decimal);
    static double decimal128ToDouble(Decimal128 decimal);
    static double decimalToDouble   (Decimal32  decimal);
    static double decimalToDouble   (Decimal64  decimal);
    static double decimalToDouble   (Decimal128 decimal);
        // Return a 'double' object having the value closest to the value of
        // the specified 'decimal' object following the conversion rules
        // defined by IEEE-754:
        //
        //: o If the 'decimal' object is a NaN, return a NaN.
        //:
        //: o Otherwise if 'decimal' is positive or negative infinity, return
        //:   infinity of the same sign.
        //:
        //: o Otherwise if 'decimal' is positive or negative zero, return zero
        //:   of the same sign.
        //:
        //: o Otherwise if 'decimal' object has an absolute value that is
        //:   larger than 'std::numeric_limits<double>::max()', raise the
        //:   "overflow" floating-point exception and return infinity of the
        //:   same sign as 'decimal'.
        //:
        //: o Otherwise if 'decimal' has an absolute value that is smaller than
        //:   'std::numeric_limits<double>::min()', raise the "underflow"
        //:   floating-point exception and return zero of the same sign as
        //:   'decimal'.
        //:
        //: o Otherwise if 'decimal' has a value that has more significant
        //:   base-10 digits than 'std::numeric_limits<double>::digits10',
        //:   raise the "inexact" floating-point exception, round that value
        //:   according to the *binary* rounding direction setting of the
        //:   floating-point environment, and return the result of that.
        //:
        //: o Otherwise if 'decimal' has a significand that cannot be exactly
        //:   represented using binary floating-point, raise the "inexact"
        //:   floating-point exception, roundthat value according to the
        //:   *binary* rounding direction setting of the environment, and
        //:   return the result of that.
        //:
        //: o Otherwise use the exact value of the 'other' object for the
        //:   initialization if this object.

                        // decimalToFloat functions

    static float decimal32ToFloat (Decimal32  decimal);
    static float decimal64ToFloat (Decimal64  decimal);
    static float decimal128ToFloat(Decimal128 decimal);
    static float decimalToFloat   (Decimal32  decimal);
    static float decimalToFloat   (Decimal64  decimal);
    static float decimalToFloat   (Decimal128 decimal);
        // Return a 'float' object having the value closest to the value of the
        // specified 'decimal' object following the conversion rules defined
        // by IEEE-754:
        //
        //: o If the 'decimal' object is a NaN, return a NaN.
        //:
        //: o Otherwise if 'decimal' is positive or negative infinity, return
        //:   infinity of the same sign.
        //:
        //: o Otherwise if 'decimal' is positive or negative zero, return zero
        //:   of the same sign.
        //:
        //: o Otherwise if 'decimal' object has an absolute value that is
        //:   larger than 'std::numeric_limits<long double>::max()', raise the
        //:   "overflow" floating-point exception and return infinity of the
        //:   same sign as 'decimal'.
        //:
        //: o Otherwise if 'decimal' has an absolute value that is smaller than
        //:   'std::numeric_limits<float>::min()', raise the "underflow"
        //:   floating-point exception and return zero of the same sign as
        //:   'decimal'.
        //:
        //: o Otherwise if 'decimal' has a value that has more significant
        //:   base-10 digits than 'std::numeric_limits<float>::digits10',
        //:   raise the "inexact" floating-point exception, round that value
        //:   according to the *binary* rounding direction setting of the
        //:   floating-point environment, and return the result of that.
        //:
        //: o Otherwise if 'decimal' has a significand that cannot be exactly
        //:   represented using binary floating-point, raise the "inexact"
        //:   floating-point exception, roundthat value according to the
        //:   *binary* rounding direction setting of the environment, and
        //:   return the result of that.
        //:
        //: o Otherwise use the exact value of the 'other' object for the
        //:   initialization if this object.

                        // decimalFromDouble functions

    static Decimal32  decimal32FromDouble (double binary);
    static Decimal64  decimal64FromDouble (double binary);
    static Decimal128 decimal128FromDouble(double binary);
        // Return the original decimal floating-point value stored in the
        // specified 'binary' floating-point value by a call to the
        // corresponding 'decimalToDouble' function earlier.  Thus this
        // function provides a limited decimal-binary-decimal round-trip
        // conversion when used together with 'decimalToDouble'.  The behavior
        // is undefined:
        //
        //: o unless 'std::numeric_limits<long double>::radix == 2'.
        //:
        //: o unless the decimal is read back into the same size decimal type
        //    that was passed as argument to 'decimalToFloat'.
        //:
        //: o unless the decimal is read back from an unchanged 'double'
        //:   returned by 'decimalToFloat'.
        //:
        //: o if the decimal originally stored into the 'long double' had more
        //:   than 'std::numeric_limits<double>::digits10' significant digits.
        //:
        //: o if the absolute value of the decimal originally stored into the
        //:   'long double' was larger than
        //:   'std::numeric_limits<double>::max()'.
        //:
        //: o if the absolute value of the decimal originally stored into the
        //:   'double' was larger than 'std::numeric_limits<double>::min()'.
        //
        // Note that the purpose of this function is to restore a decimal value
        // that has been stored earlier into a base-2 floating-point type and
        // *not* to create a decimal from the exact base-2 value.  Use the
        // conversion constructors when you are not restoring a decimal.

                        // decimalFromFloat functions

    static Decimal32  decimal32FromFloat (float binary);
    static Decimal64  decimal64FromFloat (float binary);
    static Decimal128 decimal128FromFloat(float binary);
        // Return the original decimal floating-point value stored in the
        // specified 'binary' floating-point value by a call to the
        // corresponding 'decimalToFloat' function earlier.  Thus this
        // function provides a limited decimal-binary-decimal round-trip
        // conversion when used together with 'decimalToFloat'.  The behavior
        // is undefined:
        //
        //: o unless 'std::numeric_limits<float>::radix == 2'.
        //:
        //: o unless the decimal is read back into the same size decimal type
        //    that was passed as argument to 'decimalToFloat'.
        //:
        //: o unless the decimal is read back from an unchanged 'float'
        //:   returned by 'decimalToFloat'.
        //:
        //: o if the decimal originally stored into the 'float' had more than
        //:   'std::numeric_limits<float>::digits10' significant digits.
        //:
        //: o if the absolute value of the decimal originally stored into the
        //:   'long double' was larger than
        //:   'std::numeric_limits<float>::max()'.
        //:
        //: o if the absolute value of the decimal originally stored into the
        //:   'float' was larger than 'std::numeric_limits<float>::min()'.
        // Note that the purpose of this function is to restore a decimal value
        // that has been stored earlier into a base-2 floating-point type and
        // *not* to create a decimal from the exact base-2 value.  Use the
        // conversion constructors when you are not restoring a decimal.

                        // decimalToDenselyPacked functions

    static void decimal32ToDenselyPacked( unsigned char *buffer,
                                          Decimal32      decimal);
    static void decimal64ToDenselyPacked( unsigned char *buffer,
                                          Decimal64      decimal);
    static void decimal128ToDenselyPacked(unsigned char *buffer,
                                          Decimal128     decimal);
    static void decimalToDenselyPacked(   unsigned char *buffer,
                                          Decimal32      decimal);
    static void decimalToDenselyPacked(   unsigned char *buffer,
                                          Decimal64      decimal);
    static void decimalToDenselyPacked(   unsigned char *buffer,
                                          Decimal128     decimal);
        // Populate the specified 'buffer' with the Densely Packed Decimal
        // (DPD) representation of the specified 'decimal' value.  The DPD
        // representations of 'Decimal32', 'Decimal64', and 'Decimal128'
        // require 4, 8, and 16 bytes respectively.  The behavior is undefined
        // unless 'buffer' points to a contiguous sequence of at least
        // 'sizeof(decimal)' bytes.  Note that the DPD representation is
        // defined in section 3.5 of IEEE 754-2008.

                        // decimalFromDenselyPacked functions

    static Decimal32  decimal32FromDenselyPacked( const unsigned char *buffer);
    static Decimal64  decimal64FromDenselyPacked( const unsigned char *buffer);
    static Decimal128 decimal128FromDenselyPacked(const unsigned char *buffer);
        // Return the native implementation representation of the value of the
        // same size base-10 floating-point value stored in Densely Packed
        // Decimal format at the specified 'buffer' address.  The behavior is
        // undefined unless 'buffer' points to a memory area at least
        // 'sizeof(decimal)' in size containing a value in DPD format.

    static void decimalFromDenselyPacked(   Decimal32           *decimal,
                                            const unsigned char *buffer);
    static void decimalFromDenselyPacked(   Decimal64           *decimal,
                                            const unsigned char *buffer);
    static void decimalFromDenselyPacked(   Decimal128          *decimal,
                                            const unsigned char *buffer);
    static void decimal32FromDenselyPacked( Decimal32           *decimal,
                                            const unsigned char *buffer);
    static void decimal64FromDenselyPacked( Decimal64           *decimal,
                                            const unsigned char *buffer);
    static void decimal128FromDenselyPacked(Decimal128          *decimal,
                                            const unsigned char *buffer);
        // Store, into the specified 'decimal', the native implmentation
        // representation of the value of the same size base-10 floating point
        // value represented in Densely Packed Decimal format, at the specified
        // 'buffer' address.  The behavior is undefined unless 'buffer' points
        // to a memory area at least 'sizeof(decimal)' in size containing a
        // value in DPD format.

                        // decimalToNetwork functions

    static unsigned char *decimal32ToNetwork(unsigned char *buffer,
                                             Decimal32      decimal);
    static unsigned char *decimal64ToNetwork(unsigned char *buffer,
                                             Decimal64      decimal);
    static unsigned char *decimal128ToNetwork(unsigned char *buffer,
                                              Decimal128     decimal);
    static unsigned char *decimalToNetwork(unsigned char *buffer,
                                           Decimal32      decimal);
    static unsigned char *decimalToNetwork(unsigned char *buffer,
                                           Decimal64      decimal);
    static unsigned char *decimalToNetwork(unsigned char *buffer,
                                           Decimal128     decimal);
        // Store the specified 'decimal', in network format, into the specified
        // 'buffer' and return the address one past the last byte written into
        // the 'buffer'. The network format is defined as big endian byte order
        // and densely packed base-10 significand encoding.  This corresponds
        // to the way IBM hardware represents these numbers in memory. The
        // behavior is undefined unless 'buffer' points to a memory area at
        // least 'sizeof(decimal)' in size.  Note that these functions always
        // return 'buffer + sizeof(decimal)' on the supported 8-bits-byte
        // architectures.

                        // decimalFromNetwork functions

    static unsigned char *decimal32FromNetwork(Decimal32           *decimal,
                                               const unsigned char *buffer);
    static unsigned char *decimal64FromNetwork(Decimal64           *decimal,
                                               const unsigned char *buffer);
    static unsigned char *decimal128FromNetwork(Decimal128          *decimal,
                                                const unsigned char *buffer);
    static unsigned char *decimalFromNetwork(Decimal32           *decimal,
                                             const unsigned char *buffer);
    static unsigned char *decimalFromNetwork(Decimal64           *decimal,
                                             const unsigned char *buffer);
    static unsigned char *decimalFromNetwork(Decimal128          *decimal,
                                             const unsigned char *buffer);
        // Store into the specified 'decimal', the value of the same size
        // base-10 floating-point value stored in network format at the
        // specified 'buffer' address and return the address one past the last
        // byte read from 'buffer'.  The network format is defined as big
        // endian byte order and densely packed base-10 significand encoding.
        // This corresponds to the way IBM hardware represents these numbers in
        // memory.  The behavior is undefined unless 'buffer' points to a
        // memory area at least 'sizeof(decimal)' in size.  Note that these
        // functions always return 'buffer + sizeof(decimal)' on the supported
        // 8-bits-byte architectures.

    static bsls::Types::size_type decimal64ToMultiWidthEncoding(
                                                   unsigned char     *buffer,
                                                   bdldfp::Decimal64  decimal);
        // Encode the specified 'decimal' in the *Multi-Width Encoding* format,
        // and store the encoding into the specified 'buffer'. Return the
        // number of bytes used by the encoding.  The behavior is undefined
        // unless 'buffer' points to a memory area of at least 8 bytes.

    static Decimal64 decimal64FromMultiWidthEncoding(
                                                unsigned char          *buffer,
                                                bsls::Types::size_type  size);
        // Decode a decimal value in the *Multi-Width Encoding' from the
        // specified 'buffer' having the specified 'size'. Return the decoded
        // value.  The behavior is undefined unless 'buffer' has at least
        // 'size' bytes, and 'size' is a valid encoding size in the
        // 'Multi-Width Encoding' format.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================


// PRIVATE CLASS METHODS

inline
int DecimalConvertUtil::decimal64ToUnpackedSpecial(
                                          bool                *isNegative,
                                          int                 *biasedExponent,
                                          bsls::Types::Uint64 *mantissa,
                                          bdldfp::Decimal64    value)
{
#ifdef BDLDFP_DECIMALPLATFORM_INTELDFP
    bsls::Types::Uint64 bidValue = value.data()->d_raw;
#else
    bsls::Types::Uint64 bidValue = bid_dpd_to_bid64(
                            static_cast<bsls::Types::Uint64>(*(value.data())));
#endif
    // This class method is based on inteldfp 'unpack_BID64' (bid_internal.h),
    // with a non-zero return if 'SPECIAL_ENCODING_MASK64' indicates a special
    // encoding; these are practically non-existent and no need to optimize.

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                (bidValue & BDLDFP_DU_INTELDFP_SPECIAL_ENCODING_MASK64) ==
                              BDLDFP_DU_INTELDFP_SPECIAL_ENCODING_MASK64)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        // punt on special encodings
        return -1;
    }

    *isNegative = (bidValue & 0x8000000000000000ull) ? 1 : 0;

    *biasedExponent = static_cast<int>(
                   (bidValue >> BDLDFP_DU_INTELDFP_EXPONENT_SHIFT_SMALL64) &
                   BDLDFP_DU_INTELDFP_EXPONENT_MASK64);

    *mantissa = bidValue & BDLDFP_DU_INTELDFP_SMALL_COEFF_MASK64;

    return 0;
}

inline
Decimal64 DecimalConvertUtil::decimal64FromUnpackedSpecial(
                                                bool                isNegative,
                                                bsls::Types::Uint64 mantissa,
                                                int                 exponent)
{
#ifdef BDLDFP_DECIMALPLATFORM_INTELDFP
    bdldfp::Decimal64 result;
    result.data()->d_raw = (isNegative ? 0x8000000000000000ull : 0) |
                           (static_cast<BID_UINT64>(exponent + 398)
                            << BDLDFP_DU_INTELDFP_EXPONENT_SHIFT_SMALL64) |
                           mantissa;
    return result;
#else
    if (isNegative) {
        return DecimalImpUtil::makeDecimalRaw64(
                                             -static_cast<long long>(mantissa),
                                             exponent);
    } else {
        return DecimalImpUtil::makeDecimalRaw64(mantissa, exponent);
    }
#endif
}

inline
Decimal64 DecimalConvertUtil::decimal64FromUnpackedSpecial(int mantissa,
                                                           int exponent)
{
#ifdef BDLDFP_DECIMALPLATFORM_INTELDFP
    bdldfp::Decimal64 result;
    result.data()->d_raw = (static_cast<BID_UINT64>(exponent + 398)
                            << BDLDFP_DU_INTELDFP_EXPONENT_SHIFT_SMALL64) |
                            mantissa;
    return result;
#else
    return DecimalUtil::makeDecimalRaw64(mantissa, exponent);
#endif
}

// CLASS METHODS
inline
bsls::Types::size_type DecimalConvertUtil::decimal64ToMultiWidthEncoding(
                                                     unsigned char     *buffer,
                                                     bdldfp::Decimal64  value)
{
    bool                isNegative;
    int                 exponent;
    bsls::Types::Uint64 mantissa;

    // 'exponent' is biased --> biased exponent = exponent + 398

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(0 ==
             decimal64ToUnpackedSpecial(&isNegative,
                                        &exponent,
                                        &mantissa,
                                        value))) {
        if (!isNegative) {
            if (394 <= exponent && exponent < 402) {
                if (mantissa < (1 << 13)) {
                    unsigned short squished = static_cast<unsigned short>(
                        mantissa | (exponent - 394) << 13);

                    unsigned short squishedN = BSLS_BYTEORDER_HTONS(squished);
                    bsl::memcpy(buffer, &squishedN, 2);
                    return 2;
                } else if (mantissa < (1 << 21)) {
                    // On IBM (and Linux to a lesser extent), copying from a
                    // word-aligned source is faster, so we shift an extra the
                    // source by an extra 8 bits.

                    unsigned int squished = static_cast<unsigned int>(
                        (mantissa << 8) | (exponent - 394) << 29);
                    unsigned int squishedN = BSLS_BYTEORDER_HTONL(squished);

                    bsl::memcpy(buffer,
                                reinterpret_cast<unsigned char*>(&squishedN),
                                3);
                    return 3;
                }
            }
        }

        if (382 <= exponent && exponent < 414) {
            if (mantissa < (1 << 26)) {
                unsigned int squished = static_cast<unsigned int>(
                                            mantissa | (exponent - 382) << 26);
                if (isNegative) {
                    squished |= 1ull << 31;
                }
                unsigned int squishedN = BSLS_BYTEORDER_HTONL(squished);
                bsl::memcpy(buffer, &squishedN, 4);
                return 4;
            }
            if (mantissa < (1ull << 34)) {
                bsls::Types::Uint64 squished =
                    static_cast<bsls::Types::Uint64>(
                     (mantissa << 24) |
                     (static_cast<bsls::Types::Uint64>(exponent - 382) << 58));
                if (isNegative) {
                    squished |= 1ull << 63;
                }
                bsls::Types::Uint64 squishedN =
                                               BSLS_BYTEORDER_HTONLL(squished);
                bsl::memcpy(buffer,
                            reinterpret_cast<unsigned char*>(&squishedN),
                            5);
                return 5;
            }
        }
    }
    decimalToNetwork(buffer, value);
    return 8;
}

inline
Decimal64 DecimalConvertUtil::decimal64FromMultiWidthEncoding(
                                                unsigned char          *buffer,
                                                bsls::Types::size_type  size)
{
    BSLS_ASSERT(1 <= size);
    BSLS_ASSERT(size <= 5 || size == 8);

    switch(size) {
      case 2: {
        int exponent = (buffer[0] >> 5) - 4;
        int mantissa = static_cast<int>(((buffer[0] & 0x1F) << 8) |
                                        static_cast<int>(buffer[1]));

        return decimal64FromUnpackedSpecial(mantissa, exponent);
      } break;
      case 3: {
        int exponent = (buffer[0] >> 5) - 4;
        int mantissa = static_cast<int>(((buffer[0] & 0x1F) << 16) |
                                        static_cast<int>(buffer[1]) << 8 |
                                        static_cast<int>(buffer[2]));
        return decimal64FromUnpackedSpecial(mantissa, exponent);
      } break;
      case 4: {
        bool isNegative = buffer[0] >> 7;
        int exponent = ((buffer[0] & 0x7F) >> 2) - 16;
        int mantissa = static_cast<int>(((buffer[0] & 0x03) << 24) |
                                        static_cast<int>(buffer[1]) << 16 |
                                        static_cast<int>(buffer[2]) << 8 |
                                        static_cast<int>(buffer[3]));
        return decimal64FromUnpackedSpecial(isNegative, mantissa, exponent);
      } break;
      case 1: {
        int exponent = (buffer[0] >> 7) - 2;
        int mantissa = static_cast<int>(buffer[0] & 0x7F);
        return decimal64FromUnpackedSpecial(mantissa, exponent);
      } break;
      case 5: {
        bool isNegative = buffer[0] >> 7;
        int exponent = ((buffer[0] & 0x7F) >> 2) - 16;
        bsls::Types::Uint64 mantissa = static_cast<bsls::Types::Uint64>(
                     static_cast<bsls::Types::Uint64>(buffer[0] & 0x03) << 32 |
                     static_cast<bsls::Types::Uint64>(buffer[1]) << 24 |
                     static_cast<bsls::Types::Uint64>(buffer[2]) << 16 |
                     static_cast<bsls::Types::Uint64>(buffer[3]) << 8 |
                     static_cast<bsls::Types::Uint64>(buffer[4]));
        return decimal64FromUnpackedSpecial(isNegative, mantissa, exponent);
      } break;
      default: {
        // This section valid only if size == 8. This condition is assumed
        // based on the precondition.
        Decimal64 value;
        decimalFromNetwork(&value, buffer);
        return value;
      } break;
    }
}

                        // decimalToDouble functions

inline
double DecimalConvertUtil::decimal32ToDouble(Decimal32 decimal)
{
    return Imp::decimalToDouble(decimal);
}

inline
double DecimalConvertUtil::decimal64ToDouble(Decimal64 decimal)
{
    return Imp::decimalToDouble(decimal);
}

inline
double DecimalConvertUtil::decimal128ToDouble(Decimal128 decimal)
{
    return Imp::decimalToDouble(decimal);
}

inline
double DecimalConvertUtil::decimalToDouble(Decimal32 decimal)
{
    return Imp::decimalToDouble(decimal);
}

inline
double DecimalConvertUtil::decimalToDouble(Decimal64 decimal)
{
    return Imp::decimalToDouble(decimal);
}

inline
double DecimalConvertUtil::decimalToDouble(Decimal128 decimal)
{
    return Imp::decimalToDouble(decimal);
}

                        // decimalToFloat functions

inline
float DecimalConvertUtil::decimal32ToFloat(Decimal32 decimal)
{
    return Imp::decimalToFloat(decimal);
}

inline
float DecimalConvertUtil::decimal64ToFloat(Decimal64 decimal)
{
    return Imp::decimalToFloat(decimal);
}

inline
float DecimalConvertUtil::decimal128ToFloat(Decimal128 decimal)
{
    return Imp::decimalToFloat(decimal);
}

inline
float DecimalConvertUtil::decimalToFloat(Decimal32 decimal)
{
    return Imp::decimalToFloat(decimal);
}

inline
float DecimalConvertUtil::decimalToFloat(Decimal64 decimal)
{
    return Imp::decimalToFloat(decimal);
}

inline
float DecimalConvertUtil::decimalToFloat(Decimal128 decimal)
{
    return Imp::decimalToFloat(decimal);
}

                        // decimalToDenselyPacked functions

inline
void DecimalConvertUtil::decimal32ToDenselyPacked(unsigned char *buffer,
                                                  Decimal32      decimal)
{
    Imp::decimalToDenselyPacked(buffer, decimal);
}

inline
void DecimalConvertUtil::decimal64ToDenselyPacked(unsigned char *buffer,
                                                  Decimal64      decimal)
{
    Imp::decimalToDenselyPacked(buffer, decimal);
}

inline
void DecimalConvertUtil::decimal128ToDenselyPacked(unsigned char *buffer,
                                                  Decimal128     decimal)
{
    Imp::decimalToDenselyPacked(buffer, decimal);
}

inline
void DecimalConvertUtil::decimalToDenselyPacked(unsigned char *buffer,
                                                Decimal32      decimal)
{
    Imp::decimalToDenselyPacked(buffer, decimal);
}

inline
void DecimalConvertUtil::decimalToDenselyPacked(unsigned char *buffer,
                                                Decimal64      decimal)
{
    Imp::decimalToDenselyPacked(buffer, decimal);
}

inline
void DecimalConvertUtil::decimalToDenselyPacked(unsigned char *buffer,
                                                Decimal128     decimal)
{
    Imp::decimalToDenselyPacked(buffer, decimal);
}


                        // decimalFromDenselyPacked functions

inline
Decimal32
DecimalConvertUtil::decimal32FromDenselyPacked(const unsigned char *buffer)
{
    return Imp::decimal32FromDenselyPacked(buffer);
}

inline
Decimal64
DecimalConvertUtil::decimal64FromDenselyPacked(const unsigned char *buffer)
{
    return Imp::decimal64FromDenselyPacked(buffer);
}

inline
Decimal128
DecimalConvertUtil::decimal128FromDenselyPacked(const unsigned char *buffer)
{
    return Imp::decimal128FromDenselyPacked(buffer);
}

inline
void
DecimalConvertUtil::decimalFromDenselyPacked(Decimal32           *decimal,
                                             const unsigned char *buffer)
{
    Imp::decimalFromDenselyPacked(decimal, buffer);
}

inline
void
DecimalConvertUtil::decimalFromDenselyPacked(Decimal64           *decimal,
                                             const unsigned char *buffer)
{
    Imp::decimalFromDenselyPacked(decimal, buffer);
}

inline
void
DecimalConvertUtil::decimalFromDenselyPacked(Decimal128          *decimal,
                                             const unsigned char *buffer)
{
    Imp::decimalFromDenselyPacked(decimal, buffer);
}

inline
void
DecimalConvertUtil::decimal32FromDenselyPacked(Decimal32           *decimal,
                                               const unsigned char *buffer)
{
    *decimal = Imp::decimal32FromDenselyPacked(buffer);
}

inline
void
DecimalConvertUtil::decimal64FromDenselyPacked(Decimal64           *decimal,
                                               const unsigned char *buffer)
{
    *decimal = Imp::decimal64FromDenselyPacked(buffer);
}

inline
void
DecimalConvertUtil::decimal128FromDenselyPacked(Decimal128          *decimal,
                                                const unsigned char *buffer)
{
    *decimal = Imp::decimal128FromDenselyPacked(buffer);
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
