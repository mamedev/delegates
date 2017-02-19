#include <stdio.h>
#include <chrono>
#include <cassert>
#include "src/delegate.h"
#include <functional>

class simple_dummy_class
{
public:
	simple_dummy_class() {}
};

// Declare some functions of varying complexity...
void SimpleStaticFunction(simple_dummy_class &, int num) {
	printf("In SimpleStaticFunction. Num=%d\n", num);
}

class CBaseClass {
protected:
	const char *m_name;
public:
	CBaseClass(const char *name) : m_name(name) {}
	virtual ~CBaseClass() { }
	void SimpleMemberFunction(int num) {
		printf("In SimpleMemberFunction in %s. Num=%d\n", m_name, num);
	}
	void ConstSimpleMemberFunction(int num) const
	{
		printf("In ConstSimpleMemberFunction in %s. Num=%d\n", m_name, num);
	}
	void ConstMemberFunction(int num) const {
		printf("In ConstMemberFunction in %s. Num=%d\n", m_name, num);
	}
	virtual void SimpleVirtualFunction(int num) {
		printf("In SimpleVirtualFunction in %s. Num=%d\n", m_name, num);
	}
	static void StaticMemberFunction(simple_dummy_class &, int num) {
		printf("In StaticMemberFunction. Num=%d\n", num);
	}
};

class COtherClass {
	double rubbish; // to ensure this class has non-zero size.
public:
	virtual ~COtherClass() { }
	virtual void UnusedVirtualFunction(void) { }
	virtual void TrickyVirtualFunction(int num) = 0;
};

class VeryBigClass {
	int letsMakeThingsComplicated[400];
};

// This declaration ensures that we get a convoluted class heirarchy.
class CDerivedClass : public VeryBigClass, virtual public COtherClass, virtual public CBaseClass
{
	double m_somemember[8];
public:
	CDerivedClass() : CBaseClass("Base of Derived") { m_somemember[0] = 1.2345; }
	void SimpleDerivedFunction(int num) { printf("In SimpleDerived. Num=%d\n", num); }
	virtual void AnotherUnusedVirtualFunction(int) {}
	virtual void TrickyVirtualFunction(int num) override {
		printf("In Derived TrickyMemberFunction. Num=%d\n", num);
	}
};


typedef delegate<void(int)> MyDelegate;

template<class _FunctionClass>
void dump_mfp(_FunctionClass *object, void (_FunctionClass::*mfp)(int num))
{
	unsigned long *ptr = (unsigned long *)&mfp;
	int size = sizeof(mfp) / sizeof(*ptr);
	(object->*mfp)(1);
	printf("Size = %d bytes:\n", (int)sizeof(mfp));
	for (int i = 0; i < size; i++)
		printf("  %08X\n", (std::uint32_t)ptr[i]);
}

void dump_vtable(const char *name, void *ptr)
{
	void **vtable = *reinterpret_cast<void ***>(ptr);
	printf("%s: vtable @ %p = %p %p %p %p\n", name, *vtable, vtable[0], vtable[1], vtable[2], vtable[3]);
}

void print_num_std_function(int i)
{
	printf("In std:function. Num=%d\n",i);
}

int main(int, char**)
{
#if defined(__clang__)
	printf("Clang version: %d.%d.%d\n", __clang_major__, __clang_minor__, __clang_patchlevel__);
#elif defined(_MSC_VER)
	printf("MSC_VER version: %d\n", _MSC_VER);
#elif defined(__GNUC__)
	printf("GCC version: %d.%d.%d\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#else
	printf("Unknown compiler\n");
#endif //

#if defined(__arm__) || defined(_M_ARM)
	printf("ARM\n");
#elif defined(__aarch64__)
	printf("ARM 64bit\n");
#elif defined(__MIPSEL__) || defined(__mips_isa_rev)
	printf("MIPS\n");
#elif defined(__mips64)
	printf("MIPS 64bit\n");
#elif defined(_M_PPC) || defined(__powerpc__)
	printf("PPC\n");
#elif defined(__powerpc64__)
	printf("PPC 64bit\n");
#elif defined(__i386__) || defined(_M_IX86)
	printf("Intel x86\n");
#elif defined(__x86_64__) || defined(_M_X64)
	printf("Intel x64\n");
#else
	printf("Unknown CPU\n");
#endif //
	printf("Pointer size %d\n", (int)sizeof(void *));

#if USE_DELEGATE_TYPE==DELEGATE_TYPE_COMPATIBLE
	printf("Using slow compatible delegates\n");
#elif USE_DELEGATE_TYPE==DELEGATE_TYPE_INTERNAL 
	printf("Using fast internal GCC/CLANG delegates\n");
#elif USE_DELEGATE_TYPE==DELEGATE_TYPE_MSVC
	printf("Using fast internal MSVC delegates\n");
#endif

	simple_dummy_class machine;
	MyDelegate funclist[12]; // delegates are initialized to empty
	CBaseClass a("Base A");
	CBaseClass b("Base B");
	CDerivedClass c;
	CDerivedClass d;

	dump_vtable("A", &a);
	dump_vtable("D", &d);


	dump_mfp(&a, &CBaseClass::SimpleMemberFunction);
	dump_mfp(&a, &CBaseClass::SimpleVirtualFunction);

	dump_mfp(&d, &CDerivedClass::SimpleDerivedFunction);
	dump_mfp(&d, &CDerivedClass::TrickyVirtualFunction);

	// Binding a simple member function
	printf("funclist[0] = MyDelegate(&CBaseClass::SimpleMemberFunction, &a);\n");
	funclist[0] = MyDelegate(&CBaseClass::SimpleMemberFunction, &a);
	printf("funclist[1] = MyDelegate(&CBaseClass::ConstSimpleMemberFunction, &a);\n");
	funclist[1] = MyDelegate(&CBaseClass::ConstSimpleMemberFunction, &a);
	// You can also bind static (free) functions
	printf("funclist[2] = MyDelegate(SimpleStaticFunction,&machine);\n");
	funclist[2] = MyDelegate(&SimpleStaticFunction,&machine);
	// and static member functions
	printf("funclist[3] = MyDelegate(&CBaseClass::StaticMemberFunction, &machine);\n");
	funclist[3] = MyDelegate(&CBaseClass::StaticMemberFunction, &machine);
	// and virtual member functions
	printf("funclist[4] = MyDelegate(&CBaseClass::SimpleVirtualFunction, &b);\n");
	funclist[4] = MyDelegate(&CBaseClass::SimpleVirtualFunction, &b);
	printf("funclist[5] = MyDelegate(&CBaseClass::SimpleVirtualFunction,(CBaseClass *)&d);\n");
	funclist[5] = MyDelegate(&CBaseClass::SimpleVirtualFunction,(CBaseClass *)&d);
	printf("funclist[6] = MyDelegate(&CDerivedClass::TrickyVirtualFunction,&c);\n");
	funclist[6] = MyDelegate(&CDerivedClass::TrickyVirtualFunction,&c);
	printf("funclist[7] = MyDelegate(&COtherClass::TrickyVirtualFunction,(COtherClass*)&c);\n");
	funclist[7] = MyDelegate(&COtherClass::TrickyVirtualFunction,(COtherClass*)&c);
	printf("funclist[8] = MyDelegate(&CDerivedClass::SimpleDerivedFunction,&c);\n");
	funclist[8] = MyDelegate(&CDerivedClass::SimpleDerivedFunction,&c);
	printf("funclist[9] = std::function<void(int)>\n");
	std::function<void(int)> func = print_num_std_function;
	funclist[9] = MyDelegate(func);
	printf("funclist[10] = [](int a) -> void {}\n");
	funclist[10] = MyDelegate([](int a) -> void { printf("In lambda. Num=%d\n",a); });

	fflush(stdout);
	for (int i = 0; i<11; i++) {
		if (!funclist[i].isnull()) {
			funclist[i](i);
			fflush(stdout);
		}
	}

	printf("Done\n");
	return 0;

}

