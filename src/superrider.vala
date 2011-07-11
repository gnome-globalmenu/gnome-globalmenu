namespace Superrider {
	private static Datalist<Class*> classes;
	private static void unref_class(Class* cl) {
		delete cl;
	}
	internal static unowned Class get_class(Type type) {
		unowned Class c = classes.id_get_data(type.qname());
		if(c == null) {
			Class* new_c = new Class(type);
			classes.id_set_data_full(type.qname(), new_c, unref_class);
			return new_c;
		}
		return c;
	}
	/**
	 * offset is the offset of the vfunc in the vtable.
	 * use something like
	 *
	 * [CCode (cname="G_STRUCT_OFFSET(SomeClass, FUNCTION)")]
	 * extern const int FunctionOffset;
	 *
	 * to obtain the offset.
	 */
	public static void superride(Type type, int offset, void* vfunc) {
		unowned Class cl = get_class(type);
		unowned Member m = cl.get_member(offset);
		m.push(vfunc);
	}
	public static void* peek_super(Type type, int offset) {
		unowned Class cl = get_class(type);
		unowned Member m = cl.get_member(offset);
		return m.peek_super();
	}
	public static void* peek_base(Type type, int offset) {
		unowned Class cl = get_class(type.parent());
		unowned Member m = cl.get_member(offset);
		return m.peek_current();
	}

	public static void release_all() {
		classes.clear();
	}
}

/*FIXME: [Compact] Before 600285 is fixed leave this commented */
internal class Superrider.Class {
	public List<Member*> members;
	public Type type;
	public TypeClass klass;
	public Class(Type type) {
		this.type = type;
		klass = type.class_ref();
	}
	~Class() {
		foreach(Member* m in members) {
			delete m;
		}
	}

	private Member* lookup_member(int offset) {
		foreach(Member* m in members) {
			if(m->offset == offset) return m;
		}
		return null;
	}
	public unowned Member get_member(int offset) {
		unowned Member cm = lookup_member(offset);
		if(cm != null) return cm;
		Member* new_cm = new Member(this, offset);
		members.prepend(new_cm);
		return new_cm;
	}

	public List<unowned Class> get_children() {
		Type[] type_children = type.children();
		List<unowned Class> rt = null;
		for(int i = 0; i< type_children.length; i++) {
			rt.prepend(Superrider.get_class(type_children[i]));
		}
		return rt;
	}
}

/*FIXME: [Compact] Before 600285 is fixed leave this commented */
internal class Superrider.Member {
	public List<void*> chain;
	public int offset;
	public unowned Class cl;
	public Member(Class cl, int offset) {
		this.offset = offset;
		this.cl = cl;
	}
	private void** pointer() {
		return (void**)((char*)cl.klass + offset);
	}
	~Member() {
		while(chain!= null) {
			*(pointer()) = chain.data;
			chain.delete_link(chain);
		}
	}
	public void push(void * vfunc) {
		chain.prepend(*(pointer()));
		List<unowned Class> children = cl.get_children();
		foreach(unowned Class ccl in children) {
			unowned Member m = ccl.get_member(offset);
			/* If the subclass does not override this member
			 * function, aka, the subclassis using the
			 * parent vfunc in its vtable,
			 *
			 * then we superride it with the provided vfunc.
			 *
			 * else the subclass will selectively chain up
			 * to the base vfunc, which is already superriden
			 * by the provided vfunc after push() finishes.
			 *
			 * Notice that *pointer still contains the
			 * untainted original vfunc.
			 *
			 * */
			if(m.peek_current() == *(pointer())) {
				m.push(vfunc);
			}
		}
		*(pointer()) = vfunc;
	}
	public void pop() {
		if(chain!= null) {
			*(pointer()) = chain.data;
			chain.delete_link(chain);
		}
	}
	public void* peek_current() {
		return *(pointer());
	}
	public void* peek_super() {
		if(chain != null)
			return chain.data;
		return null;
	}
}

