/*
 *	$Id$
 *	System class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2011, IRIT UPS.
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

#include <elm/assert.h>
#include <elm/sys/Thread.h>
#include <elm/sys/SystemException.h>
#include <elm/string.h>
#if defined(__unix) || defined(__APPLE__)
#	include <pthread.h>
#	include <errno.h>
#	include <string.h>
#elif defined(WIN32) || defined(WIN64)
#	include <windows.h>
#else
#	error "thread: unsupported system"
#endif

namespace elm { namespace sys {

/**
 * @class Runnable
 * A runnable must of the parent of any class representing a computation
 * performed in a thread.
 *
 * The user must overload the run() method in order to provided the computation
 * of the thread and may use the stop() method to stop the thread.
 *
 * Whatever, the end of run method stop also the thread.
 */

/**
 * This method must be overloaded to provide the computation to the thread.
 * As a default, it does nothing and return immediately.
 */
void Runnable::run(void) {
}

/**
 * Runnable that does nothing.
 */
Runnable Runnable::null;

/**
 */
Runnable::Runnable(void): thr(0) {
}

/**
 */
Runnable::~Runnable(void) {
}

/**
 * Cause the thread to stop immediately.
 */
void Runnable::stop(void) {
	thr->stop();
}

/**
 * @fn Thread *Runnable::thread(void) const;
 * Get the current thread executing the runnable.
 * @return	Thread executing the runnable, null if the run()
 * 			has been called out of a thread scope.
 */

/**
 * @fn Runnable& Runnable::current(void);
 * Get the runnable for the current thread.
 * @return	Current thread runnable.
 */


/**
 * @class Thread
 * Portable thread implementation A thread executes
 * in a concurrent execution flow the given @ref Runnable
 * object.
 *
 * The threads execution supports:
 * @li @ref join() at end,
 * @li @ref kill() killing,
 * @li @ref Mutex synchronization.
 */

/**
 */
Thread::Thread(Runnable& runnable): _runnable(&runnable) {
	runnable.thr = this;
}

/**
 */
Thread::~Thread(void) {
}

/**
 * @fn Thread *Thread::make(Runnable& runnable);
 * Build a new portable thread. Notice that the runnable
 * is not executed when the thread is created. It starts only
 * when a call to @ref start() is performed!
 * @return	Built thread or null if there is no more resources.
 */

/**
 * @fn void Thread::start(void) throw(ThreadException);
 * Start the execution of the thread and perform the call
 * to Runnable::run(). The thread is stopped when Runnable::run()
 * returns, when it is killed or when the Runnable::run() performs
 * a call to stop.
 * @throw ThreadException	If no more resources are available for the thread.
 */

/**
 * @fn void Thread::stop(void);
 * Stop the current thread.
 */

/**
 * @fn void Thread::join(void) throw(ThreadException);
 * Wait for the current thread to stop.
 */

/**
 * @fn void Thread::kill(void) throw(ThreadException);
 * Kill the current thread: notice that this method does not
 * allow any synchronization, the thread will be killed but
 * guarantee can be done at which program point it will happen.
 * @throw ThreadException	Thrown if the thread is already dead.
 */

/**
 * @fn bool Thread::isRunning(void);
 * Test if the current thread is running.
 * @return	True if the thread is running, false else.
 */

/**
 * @fn Thread *Thread::current(void);
 * Get the current thread.
 * @return Current thread.
 */

/**
 * @fn void Thread::setRootRunnable(Runnable& runnable);
 * Convenient function to set the runnable of root thread.
 * @param runnable	Runnable to assign to the root thread.
 */


/**
 * @class Mutex
 * System-independent implementation of a mutex.
 */

/**
 */
Mutex::~Mutex(void) { }

/**
 * @fn void Mutex::lock(void);
 * Acquire the mutex. If mutex is not available, block until it becomes available.
 * @throw SystemException	No more place in waiting queue.
 */

/**
 * @fn void Mutex::unlock(void);
 * Release a mutex acquired by the current thread.
 * @throw SystemException	Mutex not owned by the current thread.
 */

/**
 * bool Mutex::tryLock(void);
 * If the mutex is free, acquire it. Else return immediately without blocking.
 * @return	True if the mutex has been acquired, false else.
 * @throw SystemException	No more place in waiting queue.
 */


#if defined(__unix) || defined(__APPLE__)

	/**
	 * PThread: implementation on Linux of threads
	 */
	class PThread: public Thread {
		friend class Thread;
	public:

		PThread(void): Thread(Runnable::null), pt(0), running(false) {
			pt = pthread_self();
			int r = pthread_key_create(&_key, NULL);
			if(r < 0)
				throw ThreadException("cannot run threads");
			if(pthread_setspecific(_key, this) < 0)
				throw ThreadException("cannot run threads");
		}

		PThread(Runnable& runnable): Thread(runnable), pt(0), running(false) { }

		virtual ~PThread(void) { }

		// Thread overload
		virtual void start(void) throw(ThreadException) {
			int rc = pthread_create(&pt, NULL, PThread::run, this);
			if(rc < 0)
				throw ThreadException("no more thread available");
		}

		virtual void join(void) throw(ThreadException) {
			int rc = pthread_join(pt, NULL);
			if(rc < 0)
				throw ThreadException(strerror(errno));
		}

		virtual void kill(void) throw(ThreadException) {
			int rc = pthread_cancel(pt);
			if(rc < 0)
				throw ThreadException(strerror(errno));
		}

		virtual bool isRunning(void) {
			return running;
		}

	protected:
		virtual void stop(void) {
			pthread_exit(NULL);
		}

	private:
		pthread_t pt;
		bool running;
		static pthread_key_t _key;

		static void cleanup(void *arg) {
			ASSERT(arg);
			PThread *thread = (PThread *)arg;
			thread->running = false;
		}

		static void *run(void *arg) {
			ASSERT(arg);
			PThread *thread = (PThread *)arg;
			pthread_cleanup_push(cleanup, thread);
			thread->running = true;
			if(pthread_setspecific(_key, thread) < 0)
				throw ThreadException("cannot run threads");
			thread->_runnable->run();
			pthread_cleanup_pop(1);
			return NULL;
		}

	};
	pthread_key_t PThread::_key;

	class PMutex: public Mutex {
	public:

		PMutex(void) throw(SystemException) {
			int r = pthread_mutex_init(&h, 0);
			if(r != 0)
				throw SystemException(errno, "elm::Mutex");
		}

		~PMutex(void) {
			pthread_mutex_destroy(&h);
		}

		virtual void lock(void) throw(SystemException) {
			int r = pthread_mutex_lock(&h);
			if(r)
				switch(errno) {
				case EAGAIN:	throw SystemException(SystemException::NO_MORE_RESOURCE, "no more place in queue");
				case EDEADLK:
				case EINVAL:
				default:		ASSERT(false); break;
				}
		}

		virtual void unlock(void) throw(SystemException) {
			int r = pthread_mutex_unlock(&h);
			if(r != 0)
				switch(errno) {
				case EPERM:		throw SystemException(SystemException::NO_ACCESS, "elm::Mutex: illegal unlock");
				case EINVAL:
				default:		ASSERT(false); break;
				}

		}

		virtual bool tryLock(void) throw(SystemException) {
			int r = pthread_mutex_trylock(&h);
			if(r == 0)
				return true;
			else
				switch(errno) {
				case EBUSY:		return false;
				case EAGAIN:	throw SystemException(SystemException::NO_MORE_RESOURCE, "no more place in queue");
				case EINVAL:
				default:		ASSERT(false); return false;
				}
		}

	private:
		pthread_mutex_t h;
	};

	static PThread root;

	void Thread::setRootRunnable(Runnable& runnable) {
		root._runnable = &runnable;
	}

	Thread *Thread::current(void) {
		return static_cast<Thread *>(pthread_getspecific(PThread::_key));
	}

#elif defined(__WIN32) || defined(__WIN64)

	/**
	 * Thread implementation for Windows.
	 */
	class WinThread: public Thread {
	public:
		WinThread(Runnable& runnable): Thread(runnable), handle(NULL) { }

		static DWORD WINAPI boot(LPVOID param) {
			WinThread *thread = static_cast<WinThread *>(param);
			thread->_runnable.run();
			return 0;
		}

		virtual void start(void) throw(ThreadException) {
			DWORD id;
			handle = CreateThread(
				NULL,	// default security attributes
				0,		// use default stack size
				boot,	// thread function name
				this,	// argument to thread function
				0,		// use default creation flags
				&id);   // returns the thread identifier
			if(handle == NULL)
				throw ThreadException(_ << "cannot create thread: " << GetLastError());
		}

		virtual void join(void) throw(ThreadException) {
			WaitForSingleObject(handle, INFINITE);
		}

		virtual void kill(void) throw(ThreadException) {
			if(TerminateThread(handle, 0))
				throw ThreadException(_ << "cannot terminate thread: " << GetLastError());
		}

		virtual bool isRunning(void) {
			if(handle == NULL)
				return false;
			DWORD status;
			GetExitCodeThread(handle, &status);
			return status == STILL_ACTIVE;
		}

	protected:
		virtual void stop(void) {
			ExitThread(0);
		}

	private:
		HANDLE handle;
	};

	// TODO test it!
	class WinMutex: public Mutex {
	public:

		WinMutex(void) throw(SystemException) {
			h = CreateMutex(NULL, FALSE, NULL);
			ASSERT(h != NULL);
		}

		~WinMutex(void) {
			CloseHandle(h);
		}

		virtual void lock(void) throw(SystemException) {
			WaitForSingleObject(h, INFINITE);
		}

		virtual void unlock(void) throw(SystemException) {
			ReleaseMutex(h);
		}

		virtual bool tryLock(void) throw(SystemException) {
			int r = WaitForSingleObject(h, 0);
			return r != WAIT_TIMEOUT;
		}

	private:
		HANDLE h;
	};

#endif


/**
 * Build a new thread for the given runnable.
 */
Thread *Thread::make(Runnable& runnable) {
#	if defined(__unix) || defined(__APPLE__)
		return new PThread(runnable);
#	elif defined(__WIN32) || defined(__WIN64)
		return new WinThread(runnable);
#	else
#		error "Threads unsupported."
#	endif
}


/**
 * Build a new mutex.
 * @return	Created mutex.
 * @throw SystemException	If the mutex cannot be created.
 */
Mutex *Mutex::make(void) throw(SystemException) {
#	if defined(__unix) || defined(__APPLE__)
		return new PMutex();
#	elif defined(__WIN32) || defined(__WIN64)
		return new WinMutex();
#	else
#		error "Mutex unsupported."
#	endif

}
} }	// elm::sys
