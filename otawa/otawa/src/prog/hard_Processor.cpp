/*
 *	$Id$
 *	hard::Processor class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2006-10, IRIT UPS.
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

#include <elm/genstruct/SortedBinMap.h>
#include <elm/serial2/XOMUnserializer.h>
#include <otawa/hard/CacheConfiguration.h>
#include <otawa/hard/Processor.h>
#include <otawa/proc/ProcessorPlugin.h>
#include <otawa/prog/WorkSpace.h>

namespace otawa { namespace hard {

/**
 * @class PipelineUnit
 * A pipeline unit is microprocessor place that used to execute an instruction.
 * It factorize common configuration items of stage and functional unit.
 * @ingroup hard
 */


/**
 * Cloning constructor.
 */
PipelineUnit::PipelineUnit(const PipelineUnit *unit)
:	name(unit->name),
	latency(unit->latency),
	width(unit->width),
	branch(unit->branch),
	mem(unit->mem)
{ }


/**
 * Empty pipeline unit builder.
 */
PipelineUnit::PipelineUnit(void)
:	latency(1),
	width(1),
	branch(false),
	mem(false)
{ }


/**
 * Build a pipeline unit using a maker.
 */
PipelineUnit::PipelineUnit(const Make& maker)
:	name(maker._name),
	latency(maker._latency),
	width(maker._width),
	branch(maker.branch),
	mem(maker.mem)
{ }


/**
 */
PipelineUnit::~PipelineUnit(void) {
}


/**
 * @fn elm::String PipelineUnit::getName(void) const;
 * Get the name of the pipeline unit.
 * @return	Pipeline unit name.
 */


/**
 * @fn int PipelineUnit::getLatency(void) const;
 * Get the latency of the pipeline unit.
 * @return	Latency (in cycles).
 */


/**
 * @fn int PipelineUnit::getWidth(void) const;
 * Get the width of the pipeline instruction, i.e.
 * the number of instruction is can process in parallel.
 * @return	Pipeline unit width (in instructions).
 */


/**
 * @fn bool PipelineUnit::isBranch(void) const;
 * Test if the pipeline unit performs branchs.
 * @return	True if it performs branch, false else.
 */


/**
 * @fn bool PipelineUnit::isMem(void) const;
 * Test if the pipeline unit performs data memory access.
 * @return	True if it performs data memory access, false else.
 */


/**
 * @class Stage
 * @ingroup hard
 *
 * This class represents a stage in a pipeline. The stages have a type:
 * @li @ref otawa::hard::Stage::FETCH -- a stage loading instruction from memory
 * (usually the initial stage of a pipeline).
 * @li @ref otawa::hard::Stage::LAZY -- a simple stage that is traversed by instructions,
 * @li @ref otawa::hard::Stage::EXEC -- a stage that performs execution of instructions
 * (it contains functionnal units that performs the work of the instructions),
 * @li @ref otawa::hard::Stage::COMMIT -- a stage that writes result of an instruction
 * in the processor state (usually the end of the traversal of the instructions).
 *
 * The stages are described by the following attributes:
 * @li the name,
 * @li the width (number of instructions traversing in parallel the stage),
 * @li the latency (number of cycles taken to traverse the stage),
 * @li the order (is the instructions traversal perform in program order or not),
 * @li for the EXEC stage only, a functionnal unit list (@ref otawa::hard::FunctionalUnit)
 * and dispatch list (@ref otawa::hard::Dispatch).
 */


/**
 * Build a simple stage.
 * @param t		Type mask.
 */
Stage::Stage(type_t t): type(t), ordered(false)
	{ }


Stage::Stage(const Make& maker)
:	PipelineUnit(maker),
	type(maker._type),
	ordered(maker._ordered)
{
	if(maker.fus) {
		fus.allocate(maker.fus.count());
		for(int i = 0; i < fus.count(); i++)
			fus[i] = maker.fus[i];
		if(maker.dispatches) {
			dispatch.allocate(maker.dispatches.count());
			for(int i = 0; i < dispatch.count(); i++)
				dispatch[i] = maker.dispatches[i];
		}
	}
}


/**
 * Build a stage by cloning the given one.
 * @param stage		Stage to clone.
 */
Stage::Stage(const Stage *stage)
: PipelineUnit(stage), type(stage->type), ordered(stage->ordered) {
	if(stage->fus.count()) {
		genstruct::SortedBinMap<FunctionalUnit *, FunctionalUnit *> map;
		fus.allocate(stage->fus.count());
		for(int i = 0; i < fus.count(); i++) {
			fus[i] = new FunctionalUnit(stage->fus[i]);
			map.put(stage->fus[i], fus[i]);
		}
		if(stage->dispatch.count()) {
			dispatch.allocate(stage->dispatch.count());
			for(int i = 0; i < dispatch.count(); i++)
				dispatch[i] = new Dispatch(stage->dispatch[i]->getType(), map.get(stage->dispatch[i]->getFU(), 0));
		}
	}
}


/**
 */
Stage::~Stage(void)
	{ }


/**
 * @fn type_t Stage::getType(void) const;
 * Get type of the stage.
 * @return	Stage type.
 */


/**
 * @fn const Table<FunctionalUnit *>& Stage::getFUs(void) const;
 * Get the table of functional units.
 * @return	Functional unit table.
 */


/**
 * @fn const Table<Dispatch *>& Stage::getDispatch(void) const;
 * Get the table of dispatch.
 * @return	Table of dispatch.
 */


/**
 * @fn bool Stage::isOrdered(void) const;
 * Test if the stage is ordered or not.
 * @return	True if the stage is ordered, false else.
 */


/**
 * Select the pipeline unit to execute the given instruction.
 * Works only with stages of type EXEC.
 * @param inst	Instruction to look a functional unit for.
 * @return		Found functional unit or null.
 */
FunctionalUnit *Stage::select(Inst *inst) const {
	ASSERT(dispatch.count() > 0);
	Inst::kind_t kind = inst->kind();
	for(int i = 0; i < dispatch.count(); i++) {
		Inst::kind_t mask = dispatch[i]->getType();
		if((mask & kind) == mask)
			return dispatch[i]->getFU();
	}
	return 0;
}


/**
 * @class Queue
 * @ingroup hard
 *
 * The instructions queues stores instruction that come from one stage to another one.
 * Possibly, an instruction queue may have an internal execution stage that uses
 * the stored instruction to compute their result.
 */


/**
 * Empty queue builder.
 */
Queue::Queue(void)
	: size(0), input(0), output(0) { }


/**
 * Build a queue from a maker.
 */
Queue::Queue(const Make& make)
:	name(make._name),
	size(make._size),
	input(make._input),
	output(make._output)
{
	if(make._intern) {
		intern.allocate(make._intern.count());
		for(int i = 0; i < intern.count(); i++)
			intern[i] = make._intern[i];
	}
}


/**
 */
Queue::~Queue(void) { }


/**
 * @fn elm::String Queue::getName(void) const;
 * Get the queue name.
 * @return Queue name.
 */


/**
 * @fn int Queue::getSize(void) const;
 * Get the queue size.
 * @return	Queue size (in instructions).
 */


/**
 * @fn Stage *Queue::getInput(void) const;
 * Get the stage that put instruction in the queue.
 * @return	Input stage.
 */


/**
 * @fn Stage *Queue::getOutput(void) const;
 * Get the stage that get instruction out of the queue.
 * @return	Output stage.
 */


/**
 * @fn const AllocatedTable<Stage *>& Queue::getIntern(void) const;
 * Get the table if stages that process instruction stored in the queue
 * (most often of out-or-order microprocessor, the queue represents a
 * re-order buffer).
 * @return	Table of stages using the instruction in the buffer.
 */


/**
 * @class FunctionalUnit
 * @ingroup hard
 *
 * A functional unit is specialized in the computation of some kinds of instructions.
 * They are contained in a stage (@ref otawa::hard::Stage) of type EXEC.
 * The selection of a functional unit is performed thanks to the @ref otawa::hard::Dispatch
 * objects provided in the execution stage.
 *
 * A functional unit is defined by:
 * @li its latency (computation time in cycles),
 * @li its width (number of instructions traversing the functional unit in parallel),
 * @li pipeline property indicating that the function unit is pipelined or not
 * (that is it may chain instruction execution cycles after cycles).
 */


/**
 * Build an empty functional unit.
 */
FunctionalUnit::FunctionalUnit(void): pipelined(false) {
}


/**
 * Build a functional unit from a maker.
 */
FunctionalUnit::FunctionalUnit(const Make& maker)
:	PipelineUnit(maker), pipelined(maker._pipelined) { }


/**
 * Builder by cloning.
 */
FunctionalUnit::FunctionalUnit(const FunctionalUnit *fu)
: PipelineUnit(fu), pipelined(fu->pipelined) {
}


/**
 */
FunctionalUnit::~FunctionalUnit(void) {
}


/**
 * @fn bool FunctionalUnit::isPipelined(void) const;
 * Test if the functional unit is pipelined.
 * @return	True if it is pipelined, false else.
 */


/**
 * @class Dispatch
 * @ingroup hard
 *
 * A dispatch object allows to map some kinds of instructions to a functional unit.
 * To find if the dispatch match an instruction, an AND is performed between the
 * instruction kind and the dispatch type and if the result is equal to the dispatch
 * type, this dispatch functional unit is selected. This means that the dispatched
 * functional unit only process instructions that meets the all properties found
 * in the dispatch kind.
 */


/**
 * Simple constructor.
 * @param t		Type mask.
 * @param f		Matching functional unit.
 */
Dispatch::Dispatch(type_t t, FunctionalUnit *f)
	: type(t), fu(f) { }


/**
 * Build an empty dispatch.
 */
Dispatch::Dispatch(void)
	: type(0), fu(0) { }


/**
 */
Dispatch::~Dispatch(void)
	{ }


/**
 * @fn type_t Dispatch::getType(void) const;
 * Get the type mask.
 * @return	Type mask.
 */


/**
 * @fn FunctionalUnit *Dispatch::getFU(void);
 * Get the associated functional unit.
 * @return	Associated functional unit.
 */


/**
 * @class Processor
 * Description of a processor pipeline. A pipeline is viewed as a collection
 * of stages (@ref otawa::hard::Stage) separated by instructions queues
 * (@ref otawa::hard::Queue).
 * @ingroup hard
 */


/**
 */
Processor::Processor(void): AbstractIdentifier(""), frequency(0), _process(0) {
}


/**
 * Build a processor using a processor builder.
 */
Processor::Processor(const Make& m, cstring name)
: AbstractIdentifier(name),
  arch(m._arch),
  model(m._model),
  builder(m._builder),
  frequency(m._frequency),
  _process(0)
{
	if(m.stages) {
		stages.allocate(m.stages.length());
		for(int i = 0; i < m.stages.length(); i++)
			stages[i] = m.stages[i];
		if(queues) {
			queues.allocate(m.queues.length());
			for(int i = 0; i < m.queues.length(); i++)
				queues[i] = m.queues[i];
		}
	}
}


/**
 * Build a clone of the given processor.
 * @param proc	Processor to clone.
 * @param name	Name of the new processor (optional).
 */
Processor::Processor(const Processor& proc, cstring name)
:	AbstractIdentifier(name),
	arch(proc.arch),
	model(proc.model),
	builder(proc.builder),
	frequency(proc.frequency),
	_process(proc._process)
{
	if(proc.stages.count()) {

		// clone stages
		genstruct::SortedBinMap<Stage *, Stage *> map;
		stages.allocate(proc.stages.count());
		for(int i = 0; i < stages.count(); i++) {
			stages[i] = new Stage(proc.stages[i]);
			map.put(proc.stages[i], stages[i]);
		}

		// clone queues
		if(proc.queues.count()) {
			queues.allocate(proc.queues.count());

		}
	}
}


/**
 */
Processor::~Processor(void) {
	/* TODO only for non-static processors
	 for(int i = 0; i < queues.count(); i++)
		delete queues[i];
	for(int i = 0; i < stages.count(); i++)
		delete stages[i];*/
}


/**
 * Null processor: no stage, no queue.
 */
const Processor Processor::null;


/**
 * Load a processor descriptor from the given file.
 * @param path	Path to the file.
 * @throw LoadException 	Thrown if an error is found.
 */
hard::Processor *Processor::load(const elm::system::Path& path) throw(LoadException) {
	Processor *_processor = new Processor();
	try {
		elm::serial2::XOMUnserializer unser(&path);
		unser >> *_processor;
		unser.flush();
		return _processor;
	}
	catch(elm::io::IOException& e) {
		delete _processor;
		throw LoadException(&e.message());
	}
	catch(otawa::MessageException& e) {
		ASSERT(false);
		return 0;
	}
}


/**
 * Load a processor description from an XML element.
 * @param element			XML element to load from.
 * @throw LoadException 	Thrown if an error is found.
 */
hard::Processor *Processor::load(xom::Element *element) throw(LoadException) {
	Processor *_processor = new Processor();
	try {
		elm::serial2::XOMUnserializer unser(element);
		unser >> *_processor;
		unser.flush();
		return _processor;
	}
	catch(elm::Exception& e) {
		delete _processor;
		throw LoadException(&e.message());
	}
}


/**
 * Generate the execution steps to execute the given instruction
 * on the current processor. On VLIW architecture, the given instruction
 * must be the start of bundle and the execute will return the execution
 * steps for the whole bundle.
 *
 * As a default, apply the processor pipeline direct execution
 * using the dispatch map if there is several functional units.
 * This method may be overload to provide customization in the way
 * the execution steps are generated.
 *
 * @param inst	Instruction (or bundle) to generate execution for.
 * @param steps	Vector to receive the steps of the instructions.
 */
void Processor::execute(Inst *inst, genstruct::Vector<Step>& steps) const {
	RegSet regs;
	steps.clear();
	for(table<Stage *>::Iterator stage(stages); stage; stage++) {

		// add first cycle and select unit
		Step step;
		PipelineUnit *unit;
		if(stage->getType() != Stage::EXEC) {
			unit = stage;
			step = Step(stage);
		}
		else {
			FunctionalUnit *fu = stage->select(inst);
			unit = fu;
			step = Step(fu);
		}
		steps.add(step);

		// left queues
		for(table<Queue *>::Iterator queue(queues); queue; queue++)
			if(queue->getOutput() == *stage)
				steps.add(Step(Step::RELEASE, *queue));

		// add read registers
		inst->readRegSet(regs);
		for(RegIter r(regs, _process->platform()); r; r++)
			steps.add(Step(Step::READ, *r));

		// add other cycles
		for(int i = 0; i < unit->getLatency() - 1; i++)
			steps.add(step);

		// entered queues
		for(table<Queue *>::Iterator queue(queues); queue; queue++)
			if(queue->getInput() == *stage)
				steps.add(Step(Step::RELEASE, *queue));

		// add written registers
		inst->writeRegSet(regs);
		for(RegIter r(regs, _process->platform()); r; r++)
			steps.add(Step(Step::WRITE, *r));
	}
}


Processor *Processor::clone(cstring name) const {
	return new Processor(*this, name);
}


/**
 * Perform a copy a copy of the current processor
 * specialized for the given process.
 * @param process		Process to use.
 * @param name			Name of the created entity (optional).
 */
Processor *Processor::instantiate(Process *process, cstring name) const {
	Processor *proc = clone(name);
	proc->_process = process;
	return proc;
}


/**
 * Attempt to obtain a processor pipeline description.
 * It looks the following elements from its configuration property list (in the given order):
 * @li @ref PROCESSOR
 * @li @ref PROCESSOR_ELEMENT (unserialize the processor description from the given  XML element)
 * @li @ref PROCESSOR_PATH (get the processor description from the given file)
n *
 * @par Provided Features
 * @li @ref PROCESSOR_FEATURE
 */
class ProcessorProcessor: public otawa::Processor {
public:
	static p::declare reg;
	ProcessorProcessor(p::declare& r = reg): Processor(r), config(0), xml(0) { }

	virtual void configure(const PropList& props) {
		Processor::configure(props);
		config = otawa::PROCESSOR(props);
		if(!config) {
			id = PROCESSOR_ID(props);
			if(!id) {
				xml = PROCESSOR_ELEMENT(props);
				if(!xml)
					path = PROCESSOR_PATH(props);
			}
		}
	}

protected:
	virtual void processWorkSpace(WorkSpace *ws) {

		// processor provided in configuration
		if(config) {
			hard::PROCESSOR(ws) = config;
			if(logFor(LOG_DEPS))
				log << "\tcustom processor configuration\n";
		}

		// processor from XML node
		else if(xml) {
			hard::Processor *proc = hard::Processor::load(xml);
			track(PROCESSOR_FEATURE, hard::PROCESSOR(ws) = proc);
			if(logFor(LOG_DEPS))
				log << "\tprocessor configuration from XML element\n";
			proc->_process = ws->process();
			config = proc;
		}

		// processor from XML file
		else if(path) {
			if(logFor(LOG_DEPS))
				log << "\tprocessor configuration from \"" << path << "\"\n";
			hard::Processor *proc = hard::Processor::load(path);
			track(PROCESSOR_FEATURE, hard::PROCESSOR(ws) = proc);
			proc->_process = ws->process();
			config = proc;
		}

		// processor from OTAWA names
		else if(id) {
			hard::Processor *orig = static_cast<hard::Processor *>(ProcessorPlugin::getIdentifier(id.toCString()));
			if(!orig)
				throw ProcessorException(*this, _ << "cannot find processor named " << id);
			if(logFor(LOG_DEPS))
				log << "\tprocessor configuration from \"" << id << "\"\n";
			hard::Processor *proc = orig->instantiate(ws->process());
			track(PROCESSOR_FEATURE, hard::PROCESSOR(ws) = proc);
			config = proc;
		}

		// no processor
		else if(logFor(LOG_DEPS))
			log << "\tno processor configuration\n";
	}

private:
	hard::Processor *config;
	xom::Element *xml;
	Path path;
	string id;
};

p::declare ProcessorProcessor::reg = p::init("otawa::ProcessorProcessor", Version(1, 0, 0))
	.provide(PROCESSOR_FEATURE)
	.maker<ProcessorProcessor>();


static SilentFeature::Maker<ProcessorProcessor> maker;
/**
 * This feature ensures we have obtained the memory configuration
 * of the system.
 *
 * @par Properties
 * @li @ref otawa::hard::PROCESSOR
 * @li @ref otawa::hard::PROCESSOR_ID
 */
SilentFeature PROCESSOR_FEATURE("otawa::hard::PROCESSOR_FEATURE", maker);


/**
 * Configuration identifier to select the used processor.
 *
 * @par Hooks
 * @li @ref otawa::WorkSpace
 *
 * @par Features
 * @li @ref otawa::CACHE_CONFIGURATION_FEATURE
 *
 * @par Default Value
 * Cache configuration without any cache (never null).
 */
Identifier<const hard::Processor *> PROCESSOR("otawa::hard::PROCESSOR", &Processor::null);


/**
 * Configuration identifier to select used processor from its identifier
 * (possibly found in an external plugin).
 */
Identifier<string> PROCESSOR_ID("otawa::hard::PROCESSOR_ID");


/**
 * @class Step
 * @ingroup hard
 *
 * An hardware step is a step or an action to perform inside the microprocessor
 * to execute an instruction. Each step may have its own parameter.
 * To get the steps to execute an instruction, one has to use the method
 * @ref Processor::execute() and pass the instruction and vector to store
 * the steps in.
 *
 * Allowed steps follow (notice that all action are done at the start of the cycle):
 * @li @ref NONE (no parameter) -- invalid step (only used for convenience).
 * @li @ref STAGE (@ref Stage) -- stage to enter.
 * @li @ref FU (@ref FunctionalUnit) -- functional to be enter.
 * @li @ref READ (@ref Register) -- register to read.
 * @li @ref WRITE (@ref Register) -- register to write.
 * @li @ref USE (@ref Queue) -- queue entry to allocate.
 * @li @ref RELEASE (@ref Qeueu) -- queue entry to release.
 */

/**
 * @fn Step::Step(void): _kind(NONE);
 * Empty step builder.
 */

/**
 * @fn Step::Step(Stage *stage);
 * Build a @ref STAGE step.
 */

/**
 * @fn Step::Step(FunctionalUnit *unit);
 * Build a @ref FU step.
 */

/**
 * @fn Step::Step(kind_t kind, const hard::Register *reg);
 * Build a register @ref READ or @ref WRITE step.
 */

/**
 * @fn Step::Step(kind_t kind, hard::Queue *queue);
 * Build a queue entry @ref USE or @ref RELEASE.
 */

/**
 * @fn Step::kind_t Step::kind(void) const;
 * Get the step kind.
 * @return	Step kind.
 */

/**
 * @fn Stage *Step::stage(void) const;
 * Only for @ref STAGE step, get the stage.
 * @return Stage.
 */

/**
 * @fn FunctionalUnit *Step::fu(void) const;
 * Only for @ref FU step, get the functional unit.
 * @return	Functional unit.
 */

/**
 * @fn const Register *Step::getReg(void) const;
 * Only register @ref READ and @ref WRITE steps, get the register.
 * @return	Register.
 */

/**
 * @fn Queue *Step::getQueue(void) const;
 * Only for queue entry @ref USE or @ref RELEASE step, get the queue.
 * @return	Queue.
 */

} } // otawa::hard

ENUM_BEGIN(otawa::hard::Stage::type_t)
VALUE(otawa::hard::Stage::FETCH),
VALUE(otawa::hard::Stage::LAZY),
VALUE(otawa::hard::Stage::EXEC),
VALUE(otawa::hard::Stage::COMMIT),
VALUE(otawa::hard::Stage::DECOMP)
ENUM_END

SERIALIZE(otawa::hard::PipelineUnit);
SERIALIZE(otawa::hard::FunctionalUnit);
SERIALIZE(otawa::hard::Dispatch);
SERIALIZE(otawa::hard::Stage);
SERIALIZE(otawa::hard::Queue);
SERIALIZE(otawa::hard::Processor);

namespace elm { namespace serial2 {

void __unserialize(Unserializer& s, otawa::hard::Dispatch::type_t& v) {

	// List  of identifiers
	static elm::value_t values[] = {
			VALUE(otawa::Inst::IS_COND),
			VALUE(otawa::Inst::IS_CONTROL),
			VALUE(otawa::Inst::IS_CALL),
			VALUE(otawa::Inst::IS_RETURN),
			VALUE(otawa::Inst::IS_MEM),
			VALUE(otawa::Inst::IS_LOAD),
			VALUE(otawa::Inst::IS_STORE),
			VALUE(otawa::Inst::IS_INT),
			VALUE(otawa::Inst::IS_FLOAT),
			VALUE(otawa::Inst::IS_ALU),
			VALUE(otawa::Inst::IS_MUL),
			VALUE(otawa::Inst::IS_DIV),
			VALUE(otawa::Inst::IS_SHIFT),
			VALUE(otawa::Inst::IS_TRAP),
			VALUE(otawa::Inst::IS_INTERN),
			value("", 0)
	};

	// Build the type
	v = 0;
	String text;
	__unserialize(s, text);
	while(text) {

		// Get the component
		int pos = text.indexOf('|');
		String item;
		if(pos < 0) {
			item = text;
			text = "";
		}
		else {
			item = text.substring(0, pos);
			text = text.substring(pos + 1);
		}

		// Find the constant
		bool done = false;
		for(int i = 0; values[i].name(); i++) {
			CString cst = values[i].name();
			if(item == cst ||
					(cst.endsWith(item) && cst[cst.length() - item.length() - 1] == ':')) {
				done = true;
				v |= values[i].value();
				break;
			}
		}
		if(!done) {
			t::uint32 m;
			try {
				item >> m;
				v |= m;
			}
			catch(io::IOException& e) {
				throw io::IOException(_ << "unknown symbol \"" << item << "\".");
			}
		}
	}
}

void __serialize(Serializer& s, otawa::hard::Dispatch::type_t v) {
	ASSERT(0);
}

} } // elm::serial2
