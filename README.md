# Fast delegates implementation

# Summary

Idea of this project is to make fast implementation in C++11/14 that will work on multiple platforms at maximum speed.

Main code is in delegate.h and delegate.cpp, rest is just to be able to test some usages in MAME itself.

So far status is next:

|Compiler | Version     | OS                   | Status                           |
|---------|-------------|----------------------|----------------------------------|
|MinGW GCC| 5.3.0 x64   |Windows               |**DELEGATE_TYPE_INTERNAL**        |
|MinGW GCC| 5.3.0 x86	|Windows               |**DELEGATE_TYPE_INTERNAL**        |
|Clang    | 3.8.0 x64	|Windows               |**DELEGATE_TYPE_INTERNAL**        |
|Clang    | 3.8.0 x86	|Windows               |Compiler crash                    |
|GCC      | 4.9.2 ARM	|Linux (RasPi2)        |**DELEGATE_TYPE_INTERNAL**        |
|GCC      | 5.3.1 x64   |Linux                 |**DELEGATE_TYPE_INTERNAL**        |
|Clang    | 3.7.0 x64	|Linux                 |**DELEGATE_TYPE_INTERNAL**        |
|Clang Apple | 7.3.0 x64 | OSX                 |**DELEGATE_TYPE_INTERNAL**        |
|VS2015   | x64         |Windows               |**DELEGATE_TYPE_MSVC** crash at Num=5 |
|VS2015   | x86	        |Windows               |**DELEGATE_TYPE_MSVC** crash at Num=0 |
|GCC      | 5.3.1 ARM64	|Linux (Odroid-C2)     |**DELEGATE_TYPE_INTERNAL** |
|GCC      | 4.9.2 MIPSEL|Linux (Creator Ci20)  |**DELEGATE_TYPE_INTERNAL** |
|Emscripten    | 1.35.0 |               |**DELEGATE_TYPE_INTERNAL**        |




At the end **DELEGATE_TYPE_COMPATIBLE** should only be used on pnacl and similar platforms.

# Benchmarks

Doing 100000000 to a virtual method

         typedef delegate<void(int j)> callback_delegate;
         callback_delegate cb = callback_delegate(FUNC(MyClass2::docount), &mc);

         typedef std::function<void(int)> callback_delegate_std;
         callback_delegate_std  cb_std = std::bind(&MyClass2::docount, &mc, std::placeholders::_1);

Please note that times will be different from run to run, but values are near

|Compiler | Version     | OS                   | Time fast delegates native (ns)                 |Time std::function/bind (ns)     |
|---------|-------------|----------------------|---------------------------------:|--------------------------------:|
|MinGW GCC| 5.3.0 x64   |Windows               | 131547400                        | 216178100                       |
|MinGW GCC| 5.3.0 x86	|Windows               | 131160000                        | 285218800                       |
|Clang    | 3.8.0 x64	|Windows               | 100766900                        | 219475700                       |
|Clang    | 3.8.0 x86	|Windows               |                                  |                                 |
|GCC      | 4.9.2 ARM	|Linux (RasPi2)        | 1120924321                       | 4146617167                      |
|GCC      | 5.3.1 x64   |Linux                 | 139180356                        | 205068909                       |
|Clang    | 3.7.0 x64	|Linux                 | 140548960                        | 182060144                       |
|Clang Apple    | 7.3.0 x64	|OSX                   | 125145702                        | 262906798                       |
|VS2015   | x64         |Windows               |                                  |                                 |
|VS2015   | x86	        |Windows               |                                  |                                 |
|GCC      | 5.3.1 ARM64	|Linux (Odroid-C2)     | 654185671                        | 1370827564                      |
|GCC      | 4.9.2 MIPSEL|Linux (Creator Ci20)  | 1002793705                       | 3341533518                      |

Windows machine Intel i7-4790K @4.00GHz

# Description
There are many implementations of delegate-like functionality for
C++ code, but none of them is a perfect drop-in fit for use in MAME.
In order to be useful in MAME, we need the following properties:

* No significant overhead; we want to use these for memory
  accessors, and memory accessor overhead is already the dominant
  performance aspect for most drivers.

* Existing static functions need to be bound with an additional
  pointer parameter as the first argument. All existing
  implementations that allow static function binding assume the
  same signature as the member functions.

* We must be able to bind the function separately from the
  object. This is to allow configurations to bind functions
  before the objects are created.

Thus, the implementations below are based on existing works but are
really a new implementation that is specific to MAME.

--------------------------------------------------------------------

The "compatible" version of delegates is based on an implementation
from Sergey Ryazanov, found here:

[http://www.codeproject.com/KB/cpp/ImpossiblyFastCppDelegate.aspx](http://www.codeproject.com/KB/cpp/ImpossiblyFastCppDelegate.aspx)

These delegates essentially generate a templated static stub function
for each target function. The static function takes the first
parameter, uses it as the object pointer, and calls through the
member function. For static functions, the stub is compatible with
the signature of a static function, so we just set the stub directly.

Pros:
	* should work with any modern compiler
	* static bindings are just as fast as direct calls

Cons:
	* lots of little stub functions generated
	* double-hops on member function calls means more overhead
	* calling through stub functions repackages parameters

--------------------------------------------------------------------

The "internal" version of delegates makes use of the internal
structure of member function pointers in order to convert them at
binding time into simple static function pointers. This only works
on platforms where object->func(p1, p2) is equivalent in calling
convention to func(object, p1, p2).

Most of the information on how this works comes from Don Clugston
in this article:

[http://www.codeproject.com/KB/cpp/FastDelegate.aspx](http://www.codeproject.com/KB/cpp/FastDelegate.aspx)

Pros:
	* as fast as a standard function call in static and member cases
	* no stub functions or double-hops needed

Cons:
	* requires internal knowledge of the member function pointer
	
License (BSD 3-clause)
-----------------------------------------------------------------------

<a href="http://opensource.org/licenses/BSD-3-Clause" target="_blank">
<img align="right" src="http://opensource.org/trademarks/opensource/OSI-Approved-License-100x137.png">
</a>

	Copyright 2010-2021 Aaron Giles,Couriersud,Miodrag Milanovic. All rights reserved.
	
	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:
	
	   1. Redistributions of source code must retain the above copyright notice,
	      this list of conditions and the following disclaimer.
	
	   2. Redistributions in binary form must reproduce the above copyright
	      notice, this list of conditions and the following disclaimer in the
	      documentation and/or other materials provided with the distribution.
	
	THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDER ``AS IS'' AND ANY EXPRESS OR
	IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
	EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
	INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
	THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
