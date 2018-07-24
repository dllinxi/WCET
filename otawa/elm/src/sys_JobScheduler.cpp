/*
 *	JobScheduler class implementation
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

#include <elm/sys/JobScheduler.h>

namespace elm { namespace sys {

/**
 * @class Job;
 * A job is a small task (inheriting from sys::Runnable) that aims to be executed
 * by the sys::JobSheduler class.
 */


/**
 * @class JobProducer
 * Interface used by the sys::JobSheduler class to obtain the list of jobs to execute.
 * When a new job is needed, the method next() is called and the processing stops when
 * a null pointer is returned. Each time a job is ended, harvest() method is called
 * in an exclusive way to exploit results of the job.
 */


/**
 * @fn Job *JobProducer::next(void);
 * Get the next job to be processed.
 * @return	Next job or null.
 */


/**
 * @fn void JobProducer::harvest(Job *job);
 * Called in a mutual exclusive way by the thread that performed the job
 * to let the used process the result of the job.
 */


/**
 * @fn class IteratorProducer;
 * A producer for sys::JboSheduler that obtains the jobs from an iterator.
 * @param T		Actual type of jobs (must extened sys::Job class).
 * @param I		Iterator on jobs of type T.
 *
 */


/**
 * @class JobScheduler
 * The job scheduler distributes a list of jobs on several thread
 * until the jobs are ended.Thread management is transparent for the user
 * and job result harvesting in exclusive way is also supported
 * by the scheduler.
 *
 * To get the list of jobs, an object extending JobProducer must be provided.
 */


/**
 * Initialize the scheduler.
 */
void JobScheduler::init(void) throw(SystemException) {

	// build the mutex
	mutex = Mutex::make();

	// determine the number of cores
	// TODO
	cnt = 4;
}


/**
 * Constructor without producer.
 * @throw SystemException	Lack of OS resources.
 */
JobScheduler::JobScheduler(void) throw(SystemException): prod(0), mutex(0), cnt(0), thds(0), state(WAIT) {
	init();
}


/**
 * Constructor.
 * @param producer	Producer to use.
 * @throw SystemException	Lack of OS resources.
 */
JobScheduler::JobScheduler(JobProducer& producer) throw(SystemException): prod(&producer), mutex(0), cnt(0), thds(0), state(WAIT) {
	init();
}


/**
 */
JobScheduler::~JobScheduler(void) {
	if(mutex)
		delete mutex;
	if(thds)
		for(int i = 0; i < cnt - 1; i++)
			if(thds[i])
				delete thds[i];
}

/**
 * @fn JobProducer& JobScheduler::producer(void) const;
 * Get the current producer.
 * @return	Current producer.
 */


/**
 * Set the current producer.
 * @param producer	Producer to set.
 */
void JobScheduler::setProducer(JobProducer& producer) {
	ASSERTP(state == WAIT, "forbidden operation when scheduler is running");
	prod = &producer;
}


/**
 * @fn inline int JobScheduler::threadCount(void) const;
 * Get the number of used threads.
 */


/**
 * Set the number of threads.
 * @param count		Number of threads.
 */
void JobScheduler::setThreadCount(int count) {
	ASSERTP(state == WAIT, "forbidden operation when scheduler is running");
	cnt = count;
}


/**
 * Start to process the jobs. This function will only when all jobs
 * has been processed or a call to stop() has been performed.
 */
void JobScheduler::start(void) {

	// allocate threads
	thds = new Thread *[cnt - 1];
	for(int i = 0; i < cnt - 1; i++)
		thds[i] = 0;
	for(int i = 0; i < cnt - 1; i++)
		thds[i] = Thread::make(*this);

	// process the jobs
	state = RUN;
	for(int i = 0; i < cnt - 1; i++)
		thds[i]->start();
	run();

	// wait all threads
	for(int i = 0; i < cnt - 1; i++)
		thds[i]->join();

	// clean threads
	for(int i = 0; i < cnt - 1; i++)
		delete thds[i];
	thds = 0;

	// process output state
	if(state == EXN) {
		state = WAIT;
		throw MessageException(err);
	}
	state = WAIT;
}


/**
 * Stop the job processing.
 */
void JobScheduler::stop(void) {
	state = STOP;
}


/**
 * Thread work.
 */
void JobScheduler::run(void) {
	mutex->lock();
	while(state == RUN) {

		// get a new job
		Job *job = prod->next();
		if(!job)
			break;
		mutex->unlock();

		// perform it
		job->run();

		// harvest the result
		mutex->lock();
		try {
			prod->harvest(job);
		}
		catch(elm::Exception& e) {
			err = e.message();
			state = EXN;
			break;
		}
	}
	mutex->unlock();
}

} }	// elm::sys
