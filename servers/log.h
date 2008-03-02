
#if ENABLE_TRACING >= 1
#define LOG(fmt, args...) g_message("%s::%s:" fmt, __FILE__, __func__, ## args)
#else
#define LOG(fmt, args...) 
#endif
/*
vim:ts=4:sw=4
*/
