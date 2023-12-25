# memchk()

Checks that a memory block only containst a specific byte value.

## Syntax

```C
int memchk(
   const void* _Src,
   int _Val,
   size_t _Size
);
```

### Parameters

*`_Src`*\
Supplies a pointer to the start of the memory block to check.

*`_Val`*\
Supplies the byte value to check for.

*`_Size`*\
Supplies the number of bytes to examine, starting from _Src.

## Return value

The function returns 0 if the entire block is made of bytes of value _Val, or a non-zero value otherwise.

When _Size is zero, the function always returns 0 as the empty buffer matches any value.

## Usage

```C
char buffer[128];
memset(buffer, 0, 128);
assert(memchk(buffer, 0, 128) == 0);
```

## Remarks

The function is optimized for speed and uses 128-bit MMX or 256-bit AVX vector instructions when building for x64 using MSVC. The function does not require _Src to be aligned to any particular value.

## Requirements

| Routine | Required header |
|---|---|
| **`memchk()`** | `<memchk.h>` |
