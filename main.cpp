#include <stdio.h>
#include <chrono>
#include <assert.h>
#include "src/osdcomm.h"
#include "src/delegate.h"

class running_machine
{
public:
	running_machine() {}
};

// Declare some functions of varying complexity...
void SimpleStaticFunction(running_machine &machine, int num) {
	printf("In SimpleStaticFunction. Num=%d\n", num);
}

void SimpleVoidFunction(running_machine &machine) {
	printf("In SimpleVoidFunction with no parameters.\n");
}

class CBaseClass {
protected:
	const char *m_name;
public:
	CBaseClass(const char *name) : m_name(name) {};
	void SimpleMemberFunction(int num) {
		printf("In SimpleMemberFunction in %s. Num=%d\n", m_name, num);
	}
	int SimpleMemberFunctionReturnsInt(int num) {
		printf("In SimpleMemberFunction in %s. Num=%d\n", m_name, num); return -1;
	}
	void ConstMemberFunction(int num) const {
		printf("In ConstMemberFunction in %s. Num=%d\n", m_name, num);
	}
	virtual void SimpleVirtualFunction(int num) {
		printf("In SimpleVirtualFunction in %s. Num=%d\n", m_name, num);
	}
	static void StaticMemberFunction(running_machine &machine, int num) {
		printf("In StaticMemberFunction. Num=%d\n", num);
	}
};

class COtherClass {
	double rubbish; // to ensure this class has non-zero size.
public:
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
	void SimpleDerivedFunction(int num) { printf("In SimpleDerived. num=%d\n", num); }
	virtual void AnotherUnusedVirtualFunction(int num) {}
	virtual void TrickyVirtualFunction(int num) override {
		printf("In Derived TrickyMemberFunction. Num=%d\n", num);
	}
};
#define FUNC(x) &x, #x

typedef delegate<void(void)> VoidDelegate;
typedef delegate<void(int)> MyDelegate;

template<class _FunctionClass>
void dump_mfp(_FunctionClass *object, void (_FunctionClass::*mfp)(int num))
{
	unsigned long *ptr = (unsigned long *)&mfp;
	int size = sizeof(mfp) / sizeof(*ptr);
	(object->*mfp)(1);
	printf("Size = %d bytes:\n", (int)sizeof(mfp));
	for (int i = 0; i < size; i++)
		printf("  %08X\n", (UINT32)ptr[i]);
}

void dump_vtable(const char *name, void *ptr)
{
	void **vtable = *reinterpret_cast<void ***>(ptr);
	printf("%s: vtable @ %p = %p %p %p %p\n", name, vtable, vtable[0], vtable[1], vtable[2], vtable[3]);
}

int main(int argc, char* argv[])
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

	running_machine machine;
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
	funclist[0] = MyDelegate(FUNC(CBaseClass::SimpleMemberFunction), &a);

	// You can also bind static (free) functions
	funclist[1] = MyDelegate(FUNC(SimpleStaticFunction),&machine);
	// and static member functions
	funclist[2] = MyDelegate(FUNC(CBaseClass::StaticMemberFunction), &machine);
	// and virtual member functions
	funclist[3] = MyDelegate(FUNC(CBaseClass::SimpleVirtualFunction), &b);
	funclist[4] = MyDelegate(FUNC(CBaseClass::SimpleVirtualFunction),(CBaseClass *)&d);
	funclist[5] = MyDelegate(FUNC(CDerivedClass::TrickyVirtualFunction),&c);
	funclist[6] = MyDelegate(FUNC(COtherClass::TrickyVirtualFunction),(COtherClass*)&c);
	funclist[7] = MyDelegate(FUNC(CDerivedClass::SimpleDerivedFunction),&c);
	fflush(stdout);
	for (int i = 0; i<8; i++) {
		if (!funclist[i].isnull()) {
			funclist[i](i);
			fflush(stdout);
		}
	}
	printf("Done\n");
	return 0;

}

