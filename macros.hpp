/*---------------------------------------------------------------------------------
MIT License

Copyright (c) 2024 Helio Nunes Santos

        Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
        copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
        copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
        AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

---------------------------------------------------------------------------------*/

#ifndef _MACROS_HPP_
#define _MACROS_HPP_

#define SET_USING(t, newname)                                                                                          \
    using newname = t;                                                                                                 \
    using newname##_const = const newname;                                                                             \
    using newname##_ptr = newname *;                                                                                   \
    using newname##_const_ptr = const newname *;                                                                       \
    using newname##_reference = newname &;                                                                             \
    using newname##_const_reference = const newname &;                                                                 \
    using newname##_rvalue_reference = newname &&;

#define SET_USING_CLASS(t, newname)                                                                                    \
  public:                                                                                                              \
    SET_USING(t, newname)                                                                                              \
  private:

#define EXTRACT_SUB_USING_T(t, u, v)                                                                                   \
    using v = t::u;                                                                                                    \
    using v##_const = t::u##_const;                                                                                    \
    using v##_ptr = t::u##_ptr;                                                                                        \
    using v##_const_ptr = t::u##_const_ptr;                                                                            \
    using v##_reference = t::u##_reference;                                                                            \
    using v##_const_reference = t::u##_const_reference;                                                                \
    using v##_rvalue_reference = t::u##_rvalue_reference;

#define EXTRACT_SUB_USING_T_CLASS(t, u, v)                                                                             \
  public:                                                                                                              \
    EXTRACT_SUB_USING_T(t, u, v)                                                                                       \
  private:

#define STRINGFY_VAL(s) STRINGFY(s)
#define STRINGFY(s) #s

#define LKERNELFUN __attribute__((section(".text.low#")))
#define LKERNELCLSSFUN __attribute__((section(".text.clss#")))
#define LKERNELRODATA __attribute__((section(".rodata.low#")))
#define LKERNELDATA __attribute__((section(".data.low#")))
#define LKERNELSDATA __attribute__((section(".sdata.low#")))
#define LKERNELBSS __attribute__((section(".bss.low#")))
#define LKERNELSBSS __attribute__((section(".sbss.low#")))

#endif