#pragma once
#include <boost\thread\recursive_mutex.hpp>

class xhThreadSafe
{
public:
	xhThreadSafe(void);
	virtual ~xhThreadSafe(void);

private:
	static boost::recursive_mutex		mux_Thread_;
	boost::recursive_mutex::scoped_lock lock_;
};

