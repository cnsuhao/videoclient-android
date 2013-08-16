#include "xhThreadSafe.h"

boost::recursive_mutex xhThreadSafe::mux_Thread_;
xhThreadSafe::xhThreadSafe(void) : lock_(mux_Thread_)
{
}


xhThreadSafe::~xhThreadSafe(void)
{
	lock_.unlock();
}
