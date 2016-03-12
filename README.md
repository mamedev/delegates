# Fast delegates implementation

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
	* only works for GCC (for now; MSVC info is also readily available)
	
