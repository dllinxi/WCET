/*
 *	JobScheduler class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2015, IRIT UPS.
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
#ifndef ELM_SYS_JOBSCHEDULER_H_
#define ELM_SYS_JOBSCHEDULER_H_

#include <elm/sys/Thread.h>

namespace elm { namespace sys {

class Job: public Runnable {
};

class JobProducer {
public:
	virtual ~JobProducer(void) { }
	virtual Job *next(void) = 0;
	virtual void harvest(Job *job) { }
};

template <class T, class I>
class IteratorProducer: public JobProducer {
public:
	IteratorProducer(const I& iter): i(iter) { }
	virtual Job *next(void) { if(i.ended()) return 0; else { T *r = i.item(); i.next(); return r; } }
	virtual void harvest(Job *job) { harvest(static_cast<T *>(job)); }
	virtual void harvest(T *job) { }
private:
	I i;
};

class JobScheduler: private Runnable {
public:
	JobScheduler(void) throw(SystemException);
	JobScheduler(JobProducer& producer) throw(SystemException);
	~JobScheduler(void);

	// accessors
	inline JobProducer& producer(void) const { return *prod; }
	void setProducer(JobProducer& producer);
	inline int threadCount(void) const { return cnt; }
	void setThreadCount(int count);

	// commands
	void start(void);
	void stop(void);

private:
	void init(void) throw(SystemException);
	virtual void run(void);
	JobProducer *prod;
	Mutex *mutex;
	int cnt;
	Thread **thds;
	enum {
		WAIT = 0,
		RUN = 1,
		STOP = 2,
		EXN = 3
	} state;
	string err;
};

} }	// elm::sys

#endif /* ELM_SYS_JOBSCHEDULER_H_ */
