#include <signal.h>

void _kill(unsigned int pid, int sig) {
	kill(pid, sig);
}
