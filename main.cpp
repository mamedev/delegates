#include <stdio.h>
#include <chrono>
#include "mame/emu.h"
#include "src/delegate.h"
#include "mame/devdelegate.h"
#include <functional>


#define BENCHMARK 1

class running_machine
{
public:
	running_machine() {}
};

// Declare some functions of varying complexity...
void SimpleStaticFunction(running_machine &, int num) {
	printf("In SimpleStaticFunction. Num=%d\n", num);
}

void SimpleVoidFunction(running_machine &) {
	printf("In SimpleVoidFunction with no parameters.\n");
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
	int SimpleMemberFunctionReturnsInt(int num) {
		printf("In SimpleMemberFunction in %s. Num=%d\n", m_name, num); return -1;
	}
	void ConstMemberFunction(int num) const {
		printf("In ConstMemberFunction in %s. Num=%d\n", m_name, num);
	}
	virtual void SimpleVirtualFunction(int num) {
		printf("In SimpleVirtualFunction in %s. Num=%d\n", m_name, num);
	}
	static void StaticMemberFunction(running_machine &, int num) {
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
	void SimpleDerivedFunction(int num) { printf("In SimpleDerived. num=%d\n", num); }
	virtual void AnotherUnusedVirtualFunction(int) {}
	virtual void TrickyVirtualFunction(int num) override {
		printf("In Derived TrickyMemberFunction. Num=%d\n", num);
	}
};


class MyClass
{
public:
	MyClass() { i = 0; }
	virtual ~MyClass() { }

	void docount2(int j) { i += j; }
	virtual void docount(int j) { i+=j; }
	int get() { return i; }
private:
	int i;
};

class MyClass2 : public MyClass
{
public:
	MyClass2() : MyClass() { }
	virtual ~MyClass2() { }
	virtual void docount(int) {  }
};

#define FUNC(x) &x, #x

typedef delegate<void(void)> VoidDelegate;
typedef delegate<void(int)> MyDelegate;

typedef device_delegate<int()> read_line_delegate;
typedef device_delegate<void(int)> write_line_delegate;

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

template<class _FunctionClass>
void dump_addr(const char *, _FunctionClass *, void (_FunctionClass::*)(int))
{
	//printf("%s Addr = %08x\n", text, (void*)(object->*mfp));
}


void dump_vtable(const char *name, void *ptr)
{
	void **vtable = *reinterpret_cast<void ***>(ptr);
	printf("%s: vtable @ %p = %p %p %p %p\n", name, *vtable, vtable[0], vtable[1], vtable[2], vtable[3]);
}
void print_num(int i)
{
	printf("print_num\n");
}

int read_num()
{
	return 10;
}

int st = 0;
void static_count(int j)
{
	st += j;
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

	running_machine machine;
	MyDelegate funclist[12]; // delegates are initialized to empty
	CBaseClass a("Base A");
	CBaseClass b("Base B");
	CDerivedClass c;
	CDerivedClass d;
#if !BENCHMARK	
	dump_vtable("A", &a);
	dump_vtable("D", &d);


	dump_mfp(&a, &CBaseClass::SimpleMemberFunction);
	dump_mfp(&a, &CBaseClass::SimpleVirtualFunction);

	dump_mfp(&d, &CDerivedClass::SimpleDerivedFunction);
	dump_mfp(&d, &CDerivedClass::TrickyVirtualFunction);

	dump_addr("CBaseClass::SimpleMemberFunction",&a, &CBaseClass::SimpleMemberFunction);
	dump_addr("CBaseClass::SimpleVirtualFunction",&b, &CBaseClass::SimpleVirtualFunction);
	dump_addr("CDerivedClass::TrickyVirtualFunction",&c, &CDerivedClass::TrickyVirtualFunction);
	dump_addr("CDerivedClass::SimpleDerivedFunction",&c, &CDerivedClass::SimpleDerivedFunction);
	// Binding a simple member function
	printf("funclist[0] = MyDelegate(&CBaseClass::SimpleMemberFunction, &a);\n");
	funclist[0] = MyDelegate(&CBaseClass::ConstSimpleMemberFunction, &a);

	// You can also bind static (free) functions
	printf("funclist[1] = MyDelegate(SimpleStaticFunction,&machine);\n");
	funclist[1] = MyDelegate(&SimpleStaticFunction,&machine);
	// and static member functions
	printf("funclist[2] = MyDelegate(&CBaseClass::StaticMemberFunction, &machine);\n");
	funclist[2] = MyDelegate(&CBaseClass::StaticMemberFunction, &machine);
	// and virtual member functions
	printf("funclist[3] = MyDelegate(&CBaseClass::SimpleVirtualFunction, &b);\n");
	funclist[3] = MyDelegate(&CBaseClass::SimpleVirtualFunction, &b);
	printf("funclist[4] = MyDelegate(&CBaseClass::SimpleVirtualFunction,(CBaseClass *)&d);\n");
	funclist[4] = MyDelegate(&CBaseClass::SimpleVirtualFunction,(CBaseClass *)&d);
	printf("funclist[5] = MyDelegate(&CDerivedClass::TrickyVirtualFunction,&c);\n");
	funclist[5] = MyDelegate(&CDerivedClass::TrickyVirtualFunction,&c);
	printf("funclist[6] = MyDelegate(&COtherClass::TrickyVirtualFunction,(COtherClass*)&c);\n");
	funclist[6] = MyDelegate(&COtherClass::TrickyVirtualFunction,(COtherClass*)&c);
	printf("funclist[7] = MyDelegate(&CDerivedClass::SimpleDerivedFunction,&c);\n");
	funclist[7] = MyDelegate(&CDerivedClass::SimpleDerivedFunction,&c);
	printf("funclist[8] = std::function<void(int)>\n");
	std::function<void(int)> func = print_num;
	funclist[8] = MyDelegate(func);
	printf("funclist[9] = MyDelegate(&CDerivedClass::SimpleDerivedFunction,&c);\n");
	funclist[9] = MyDelegate([](int a) -> void { printf("lambda %d\n",a); });

	fflush(stdout);
	for (int i = 0; i<10; i++) {
		if (!funclist[i].isnull()) {
			funclist[i](i);
			fflush(stdout);
		}
	}

	printf("Checking devdelegates:\n");

	device_t root("root");
	device_t sub1("sub1");
	device_t sub2("sub2");
	root.add_device("sub1", &sub1);
	root.add_device("sub2", &sub2);

	write_line_delegate write_del = write_line_delegate(FUNC(device_t::write), &root);
	read_line_delegate read_del = read_line_delegate(FUNC(device_t::read), &root);
	std::function<int()> func2 = read_num;
	
	read_line_delegate read_del_delegate = read_line_delegate(read_num, "test");
	write_del(100);
	printf("read_line_delegate : %d\n", read_del());
	printf("read_line_delegate : %d\n", read_del_delegate());

	write_line_delegate sub1_write_del = write_line_delegate(FUNC(device_t::write), "sub1", (device_t*)nullptr);
	read_line_delegate sub1_read_del = read_line_delegate(FUNC(device_t::read), "sub1", (device_t*)nullptr);
	read_line_delegate sud2_read_del_delegate = read_line_delegate([]() -> int { printf("in read delegate lambda\n"); return 4; }, "test");
	sub1_write_del.bind_relative_to(root);
	sub1_read_del.bind_relative_to(root);
	sub1_write_del(200);
	
	printf("read_line_delegate sub 1: %d\n", sub1_read_del());
	printf("read_line_delegate sub 1: %d\n", sud2_read_del_delegate());
#else
	typedef delegate<void(int j)> driver_callback_delegate;

	MyClass2 mc;
	driver_callback_delegate md = driver_callback_delegate(&MyClass2::docount, &mc);
	
	printf("Benchmarking virtual call : ");
	{
		auto before = std::chrono::high_resolution_clock::now(); ;
		for (int i = 0; i < 100000000; i++)
		{
			md(i);
		}
		auto after = std::chrono::high_resolution_clock::now(); ;
		auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(after - before);
		printf("%lld\n", elapsed.count());
	}
	
	int i = 0;
	driver_callback_delegate pda = driver_callback_delegate([&](int j) {  i += j; });

	printf("Benchmarking lambda call : ");
	{
		auto before = std::chrono::high_resolution_clock::now(); ;
		for (int i = 0; i < 100000000; i++)
		{
			pda(i);
		}
		auto after = std::chrono::high_resolution_clock::now(); ;
		auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(after - before);
		printf("%lld\n", elapsed.count());
	}

	driver_callback_delegate md2 = driver_callback_delegate(&static_count);

	printf("Benchmarking static call : ");
	{
		auto before = std::chrono::high_resolution_clock::now(); ;
		for (int i = 0; i < 100000000; i++)
		{
			md2(i);
		}
		auto after = std::chrono::high_resolution_clock::now(); ;
		auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(after - before);
		printf("%lld\n", elapsed.count());
	}

	driver_callback_delegate md3 = driver_callback_delegate(std::function<void(int j)>(static_count));
	printf("Benchmarking std::function call : ");
	{
		auto before = std::chrono::high_resolution_clock::now(); ;
		for (int i = 0; i < 100000000; i++)
		{
			md3(i);
		}
		auto after = std::chrono::high_resolution_clock::now(); ;
		auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(after - before);
		printf("%lld\n", elapsed.count());
	}


#endif
	printf("Done\n");
	return 0;

}

