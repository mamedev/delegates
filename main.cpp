#include <stdio.h>
#include <chrono>
#include <assert.h>
#include "src\osdcomm.h"
#include "src\delegate.h"

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
	virtual void TrickyVirtualFunction(int num) {
		printf("In Derived TrickyMemberFunction. Num=%d\n", num);
	}
};
#define FUNC(x) &x, #x

typedef delegate<void(void)> VoidDelegate;
typedef delegate<void(int)> MyDelegate;

int main(int argc, char* argv[])
{
	running_machine machine;
	MyDelegate funclist[12]; // delegates are initialized to empty
	CBaseClass a("Base A");
	CBaseClass b("Base B");
	CDerivedClass d;
	CDerivedClass c;

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
	for (int i = 0; i<8; i++) {
		if (!funclist[i].isnull()) {
			funclist[i](i);
		}
	}
	printf("Done\n");
	return 0;

}

