///////////////////////////////////////////////////////
// SPDX-License-Identifier: MIT
// https://github.com/axelriet/memchk
// Copyright © Axel Rietschin.  All rights reserved.
//

#ifdef _MSC_VER
#include <sal.h>
#define VECTORCALL __vectorcall
#else
#ifdef _In_reads_bytes_
#undef _In_reads_bytes_
#endif
#define _In_reads_bytes_(s)
#ifdef _In_
#undef _In_
#endif
#define _In_
#define VECTORCALL
#endif

#include <stddef.h>

//
// Verifies that a block of memory is filled with a given value.
// 
// Returns 0 if yes, or nonzero if the content does not match.
//

int VECTORCALL memchk(_In_reads_bytes_(_Size) const void* _Src,
    _In_ int _Val,
    _In_ size_t _Size);
