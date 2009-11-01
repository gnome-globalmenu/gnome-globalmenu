public class MyBaseClass {
	public virtual int my_function(int a, int b) {
		message("MyBaseClass.my_function %d, %d", a, b);
		return 1;
	}
}
public class MyClass1 : MyBaseClass {
	public override int my_function(int a, int b) {
		message("MyClass1.my_function %d, %d", a, b);
		base.my_function(a, b);
		return 10;
	}
}
public class MyClass2 : MyBaseClass {
	public override int my_function(int a, int b) {
		message("MyClass2.my_function %d, %d", a, b);
		base.my_function(a, b);
		return 20;
	}
}
public static delegate int MyFunction(MyBaseClass* object, int a, int b);
[CCode (cname="G_STRUCT_OFFSET(MyBaseClassClass, my_function)")]
extern const int MyFunctionOffset;
public int main() {
	MyFunction my_super_function = (object, a, b) => {
		message("my_super_function %d, %d", a, b);
		((MyFunction) Superrider.peek_super(typeof(MyBaseClass), MyFunctionOffset))(object, a, b);
		return -1;
	};
	MyClass2 object2 = new MyClass2();
	MyBaseClass object_base = new MyBaseClass();
	Superrider.superride(typeof(MyBaseClass),
		MyFunctionOffset, (void*)my_super_function);
	MyClass1 object1 = new MyClass1();
	assert(object_base.my_function(1, 2) == -1);
	assert(object1.my_function(1, 2) == 10);
	assert(object2.my_function(1, 2) == 20);
	Superrider.release_all();
	assert(object1.my_function(1, 2) == 10);
	assert(object2.my_function(1, 2) == 20);
	assert(object_base.my_function(1, 2) == 1);
	return 0;
}
