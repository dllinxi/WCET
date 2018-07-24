/*
 *	Thread class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2011-16, IRIT UPS.
 *
 *	OTAWA is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	OTAWA is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OTAWA; if not, write to the Free Software
 *	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef ELM_SYSTEM_THREAD_H_
#define ELM_SYSTEM_THREAD_H_

#include <elm/sys/SystemException.h>
#include <elm/util/GenValue.h>
#include <elm/util/MessageException.h>

namespace elm { namespace sys {

// pre-declaration
class Thread;

// ThreadException class
class ThreadException: public MessageException {
public:
	inline ThreadException(const string& message): MessageException(message) { }
};


// Runnable class
class Runnable {
	friend class Thread;
public:
	static Runnable null;
	Runnable(void);
	virtual ~Runnable(void);

	virtual void run(void);

	inline Thread *thread(void) const { return thr; }
	inline static Runnable& current(void);

protected:
	void stop(void);
private:
	Thread *thr;
};


// Thread class
class Thread {
	friend class Runnable;

public:
	virtual ~Thread(void);
	inline Runnable& runnable(void) const { return *_runnable; }

	static Thread *make(Runnable& runnable);
	static Thread *current(void);
	static void setRootRunnable(Runnable& runnable);

	virtual void start(void) throw(ThreadException) = 0;
	virtual void join(void) throw(ThreadException) = 0;
	virtual void kill(void) throw(ThreadException) = 0;
	virtual bool isRunning(void) = 0;

protected:
	Runnable *_runnable;
	Thread(Runnable& runnable);
	virtual void stop(void) = 0;
};

inline Runnable& Runnable::current(void) { return Thread::current()->runnable(); }


class Mutex {
public:
	static Mutex *make(void) throw(SystemException);
	virtual ~Mutex(void);
	virtual void lock(void) throw(SystemException) = 0;
	virtual void unlock(void) throw(SystemException) = 0;
	virtual bool tryLock(void) throw(SystemException) = 0;
};

} }	// elm::sys

#endif /* ELM_SYSTEM_THREAD_H_ */
