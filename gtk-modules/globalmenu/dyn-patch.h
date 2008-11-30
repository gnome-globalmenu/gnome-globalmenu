
#define DEFINE_FUNC(ret, t_ype, method, para) \
	ret ( * _old_ ## t_ype ## _ ## method ) para = NULL; \
	static ret ( _ ## t_ype ## _ ## method) para 

#define OVERRIDE( klass, t_ype, method ) \
	_old_ ## t_ype ## _ ## method = klass->method; \
	klass->method =  _ ## t_ype ## _ ## method;
