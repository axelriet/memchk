///////////////////////////////////////////////////////
// SPDX-License-Identifier: MIT
// https://github.com/axelriet/memchk
// Copyright © Axel Rietschin.  All rights reserved.
//

#include <memchk.h>

#if defined(_MSC_VER)
#include <intrin.h>
#pragma optimize("gt", on)
#endif

//
// Verifies that a block of memory is filled with a given byte value.
//
// Returns 0 if yes, or nonzero if the content does not match.
//

int VECTORCALL memchk(_In_reads_bytes_(_Size) const void* _Src, _In_ int _Val,
                      _In_ size_t _Size)
{
    //
    // Explicitly do NOT check the _Src pointer,
    // if they passed a null, the function will
    // crash and this is the best way to let them
    // know.
    //
    // The empty buffer matches all values.
    //

    if (!_Size)
    {
        return 0;
    }

    const unsigned char v = (const unsigned char) _Val;
    const unsigned char* p = (const unsigned char*) _Src;

#if defined(__INTRIN_H_)

    _mm_prefetch((const char*) p, _MM_HINT_T0);

#if defined(__AVX__)

#pragma message("memchk() compiled with 256-bit vector instructions.")

    if (_Size >= 32)
    {
        //
        // Check for 32-alignement first.
        //

        size_t misalign = (((intptr_t) p) & 0x1f);

        if (misalign)
        {
            misalign = 32 - misalign;

            _Size -= misalign;

            if (misalign >= 16)
            {
                misalign -= 16;

                while (misalign--)
                {
                    if (*p++ != v)
                    {
                        return -1;
                    }
                }

                //
                // We are 16-aligned now.
                //

                __m128i v128 = _mm_set1_epi8(v);
                __m128i* p128 = (__m128i*) p;

                if (_mm_movemask_epi8(_mm_cmpeq_epi8(*p128, v128)) != 0xffff)
                {
                    return -1;
                }

                p += 16;
            }
            else
            {
                while (misalign--)
                {
                    if (*p++ != v)
                    {
                        return -1;
                    }
                }
            }
        }

        //
        // We are 32-aligned now.
        //
        // Fast-compare 32 bytes at a time.
        //

        __m256i v256 = _mm256_set1_epi8(v);
        __m256i* p256 = (__m256i*) p;

        while (_Size >= 32)
        {
            if (_mm256_movemask_epi8(_mm256_cmpeq_epi8(*p256, v256)) !=
                0xffffffff)
            {
                return -1;
            }

            ++p256;

            _Size -= 32;
        }

        if (!_Size)
        {
            return 0;
        }

        p = (const unsigned char*) p256;
    }

#else

#pragma message("memchk() compiled with 128-bit vector instructions.")

#endif

    if (_Size >= 16)
    {
        //
        // Check for 16-alignement first.
        //

        size_t misalign = (size_t)(((intptr_t) p) & 0xf);

        if (misalign)
        {
            misalign = 16 - misalign;

            _Size -= misalign;

            while (misalign--)
            {
                if (*p++ != v)
                {
                    return -1;
                }
            }
        }

        //
        // We are 16-aligned now.
        //
        // Fast-compare 16 bytes at a time.
        //

        __m128i v128 = _mm_set1_epi8((char)v);
        __m128i* p128 = (__m128i*)p;

        while (_Size >= 16)
        {
            if (_mm_movemask_epi8(_mm_cmpeq_epi8(*p128, v128)) != 0xffff)
            {
                return -1;
            }

            ++p128;

            _Size -= 16;
        }

        if (!_Size)
        {
            return 0;
        }

        p = (const unsigned char*) p128;
    }

#endif

    //
    // Compare a byte-at-a-time.
    //
    // Long, long time ago, repe scasb *used* to be faster
    //

    while (_Size--)
    {
        if (*p++ != v)
        {
            return -1;
        }
    }

    return 0;
}
