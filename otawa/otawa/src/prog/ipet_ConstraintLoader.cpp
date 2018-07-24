/*
 *	$Id$
 *	Copyright (c) 2005, IRIT UPS.
 *
 *	prog/ipet_ConstraintLoader.cpp -- ConstraintLoader class implementation.
 */

#include <stdio.h>
#include <elm/string/StringBuffer.h>
#include <otawa/cfg/CFGCollector.h>
#include <otawa/ipet/IPET.h>
#include <otawa/ipet/ConstraintLoader.h>
#include <otawa/cfg.h>
#include <otawa/ipet/VarAssignment.h>
#include "ExpNode.h"

// Externals
extern FILE *ipet_in;
extern int ipet_line;

namespace otawa { namespace ipet {

/**
 * @defgroup extcons ExtCons : External Constraint Format
 *
 * This kinb of file contains constraints that may be added to an IPET
 * computation performed in OTAWA (using the @ref otawa::ipet::ConstraintLoader
 * processor). This features may be used to insert hand-written constraints
 * or constraints generated by external tool.
 *
 * The format of this file is simple text possibly containing comments
 * starting from '#' symbol to the end of the file.
 *
 * The constraints are written as usual inequalities with comparators : '<',
 * '>', '=', '<=', or '>='. The expressions may contain:
 * @li variable names,
 * @li numeric constants,
 * @li '+' expression : prefixed plus,
 * @li '-' expression : prefixed minus,
 * @li expression '+' expression : addition,
 * @li expression '-' expression : subtraction,
 * @li expression '*' expression : multiplication,
 * @li expression '/' expression : division,
 * @li '(' expression ')' : parentheses,
 *
 * To be consistent with the current ILP constraint system in OTAWA, links must
 * be done with the variables associated with nodes and edges. This is
 * performed by the following commands:
 * @li 'bb' id address -- name id the variable identifying occurrences of BB at given address,
 * @li 'bb' id '$' index -- name id the variable identifying occurrences of BB with given index,
 * @li 'edge' id address address -- name id the variable identifying occurrences edge whose source and target BB are at given address.
 *
 * The identifier id are name of variable that may be then used in the constraint description.
 *
 * Below is displayed a simple example of mutual exclusive execution:
 * @code
 * # variable identification
 * bb	x_1 	0x125C
 * bb 	x_2 	$11
 *
 * # constraints
 * x_1 + x_2 <= 1
 *
 * @endcode
 */



// NormNode class
class NormNode {
	double coef;
	ilp::Var *var;
	NormNode *nxt;
public:
	inline NormNode(double coefficient, ilp::Var *variable = 0, NormNode *next = 0)
		: coef(coefficient), var(variable), nxt(next) {
		};
	inline double coefficient(void) const
		{ return coef; };
	inline ilp::Var *variable(void) const
		{ return var; };
	inline NormNode *next(void) const
		{ return nxt; };
	inline NormNode *append(NormNode *to_append) {
		NormNode *cur;
		for(cur = this; cur->nxt; cur = cur->nxt) ;
		cur->nxt = to_append;
		return this;
	};
	inline bool isConstant(void) const
		{ return var == 0 && !nxt; };
	inline bool isSimple(void) const
		{ return !nxt; };
	inline NormNode *multiply(double cst) {
		for(NormNode *cur = this; cur; cur = cur->nxt)
			cur->coef *= cst;
		return this;
	};
};


// NormalizationException class
class NormalizationException {
};


/**
 * This property identifier is used for passing specific file to load to
 * the @ref ConstraintLoader (argument of type @ref elm::String).
 * @ingroup extcons
 */
Identifier<string> ConstraintLoader::PATH("otawa::ipet::ConstraintLoader::PATH", "");


/**
 * @class ConstraintLoader
 * This code processor allows loading IPET constraint from an external file.
 * The external file must follows the format @ref ExtCons.
 *
 * @par Configuration Properties
 * @li @ref ConstraintLoader::PATH - path to the file containing the constraints.
 *
 * @par Required Feature
 * @li @ref CFG_INFO_FEATURE
 *
 * @ingroup extcons
 */


Registration<ConstraintLoader> ConstraintLoader::reg(
	"otawa::ipet::ConstraintLoader", Version(1, 0, 0),
	p::base,	&CFGProcessor::reg,
	p::require,	&otawa::ipet::ILP_SYSTEM_FEATURE,
	p::require,	&otawa::ipet::ASSIGNED_VARS_FEATURE,
	p::end
);

/**
 * Constructor.
 */
ConstraintLoader::ConstraintLoader(AbstractRegistration& r)
:	CFGProcessor(r),
 	fw(0),
 	system(0) {
}


/**
 * Find the BB matching the given address.
 * @param addr	Address of BB to find.
 * @return		Found BB or null.
 */
BasicBlock *ConstraintLoader::getBB(address_t addr) {
	BasicBlock *bb = bbs.get(addr, 0);
	if(!bb) {
		for (CFGCollection::Iterator icfg(INVOLVED_CFGS(fw)); icfg; icfg++) {
			for (CFG::BBIterator ibb(icfg); ibb; ibb++)
				if(!ibb->isEnd()) {
					if(ibb->address() <= addr && addr < ibb->topAddress())
						bb = ibb;
				}
		}
		if(!bb) {
			log << "ERROR: cannot find basic block at " << addr << ".\n";
			return 0;
		}
		bbs.put(addr, bb);
	}
	return bb;
}


/**
 * find BB matching the given index from the main CFG.
 * @param 	index	BB index.
 * @return	Found BB or null.
 */
BasicBlock *ConstraintLoader::getBB(int index) {
	const CFGCollection& coll = **INVOLVED_CFGS(fw);
	for(CFG::BBIterator bb(coll[0]); bb; bb++)
		if(bb->number() == index)
			return bb;
	return 0;
}


/**
 * For internal use only.
 */
void ConstraintLoader::newBBVar(cstring name, address_t addr) {
	BasicBlock *bb = getBB(addr);
	if(!bb)
		error("cannot find BB at given address");
	else {
		ilp::Var *var = ipet::VAR(bb);
		if(!var)
			error(_ << "variable " << name << " of basic block at " << addr << " is not defined.");
		else
			vars.put(name, var);
	}
}


/**
 * For internal use only.
 */
void ConstraintLoader::newBBVar(cstring name, int index) {
	BasicBlock *bb = getBB(index);
	if(!bb)
		error("cannot find the BB!");
	else {
		ilp::Var *var = ipet::VAR(bb);
		if(!var)
			error(_ << "variable " << name << " of basic block " << index << " is not defined.");
		else
			vars.put(name, var);
	}
}


/**
 * For internal use only.
 */
bool ConstraintLoader::newEdgeVar(CString name, address_t src, address_t dst) {

	// Find basic blocks
	BasicBlock *src_bb = getBB(src);
	if(!src_bb)
		return false;
	BasicBlock *dst_bb = getBB(dst);
	if(!dst_bb)
		return false;

	// Find edge
	for(BasicBlock::OutIterator edge(src_bb); edge; edge++) {
		if(edge->target() == dst_bb) {
			ilp::Var *var = ipet::VAR(edge);
			if(var) {
				vars.put(name, var);
				return var;
			}
			else
				log << "ERROR: variable " << name
					<< " of edge from basic block " << src
					<< " to basic block " << dst
					<< " is not defined.\n";
		}
	}

	// Not found error
	log << "ERROR: no edge from basic block " << src
		<< " to basic block " << dst << ".\n";
	return false;
}


/**
 * For internal use only.
 */
ilp::Var *ConstraintLoader::getVar(CString name) {
	ilp::Var *var = vars.get(name, 0);
	if(!var)
		log << "ERROR: variable \"" << name << "\" is not defined.\n";
	return var;
}


/**
 * Add a constraint to the current ILP system.
 * @param left		Left side of the equation.
 * @param t			Comparator of the equation.
 * @param right		Right side of the equation.
 * @return
 */
bool ConstraintLoader::addConstraint(ExpNode *left, ilp::Constraint::comparator_t t,
ExpNode *right) {
	try {
		NormNode *norm = normalize(left, 1);
		norm->append(normalize(right, -1));
		ilp::Constraint *cons = system->newConstraint(t);
		if(logFor(LOG_BB))
			log << '\t';
		for(; norm; norm = norm->next()) {
			cons->add(norm->coefficient(), norm->variable());
			if(logFor(LOG_BB)) {
				log << norm->coefficient() << ' ';
				if(norm->variable()) {
					if(norm->variable()->name() == "")
						log << "X" << (void *)norm->variable() << ' ';
					else
						log << norm->variable()->name() << ' ';
				}
			}
		}
		if(logFor(LOG_BB)) {
			switch(t) {
			case ilp::Constraint::EQ: log << "="; break;
			case ilp::Constraint::GE: log << ">="; break;
			case ilp::Constraint::GT: log << ">"; break;
			case ilp::Constraint::LT: log << "<"; break;
			case ilp::Constraint::LE: log << "<="; break;
			default:				  ASSERT(false); break;
			}
			log << " 0\n";
		}
		return true;
	}
	catch(NormalizationException e) {
		log << "ERROR: expression cannot be reduced !\n";
		return false;
	}
}


/**
 * Normalize the given expression.
 * @param node	Expression to normalize.
 * @param mult	Multiplier to apply.
 * @return		Normalized expression.
 */
NormNode *ConstraintLoader::normalize(ExpNode *node, double mult) {
	ASSERT(node);
	NormNode *arg1, *arg2;

	switch(node->kind()) {

	// Constant
	case ExpNode::CST:
		return new NormNode(mult * node->cst());

	// Variable
	case ExpNode::VAR:
		return new NormNode(mult, node->var());

	// Positive
	case ExpNode::POS:
		return normalize(node->arg(), mult);

	// Negative
	case ExpNode::NEG:
		return normalize(node->arg(), -mult);

	// Subtraction
	case ExpNode::SUB:
		arg1 = normalize(node->arg1(), mult);
		arg2 = normalize(node->arg2(), -mult);
		goto process_sum;

	// Addition
	case ExpNode::ADD:
		arg1 = normalize(node->arg1(), mult);
		arg2 = normalize(node->arg2(), mult);
	process_sum:
		if(arg1->isConstant() && arg2->isConstant())
			return new NormNode(arg1->coefficient() + arg2->coefficient());
		else
			return arg1->append(arg2);

	// Multiplication
	case ExpNode::MUL:
		arg1 = normalize(node->arg1(), mult);
		arg2 = normalize(node->arg2(), mult);
		if(arg1->isConstant())
			return arg2->multiply(arg1->coefficient());
		else if(arg2->isConstant())
			return arg1->multiply(arg2->coefficient());
		else
			throw new NormalizationException();

	// Division
	case ExpNode::DIV:
		/* TODO if(arg2->isConstant())
			return arg1->multiply(1 / arg2->coefficient());
		else*/
			throw NormalizationException();

	// Too bad !
	default:
		ASSERT(0);
		return 0;
	}
}


/**
 */
void ConstraintLoader::configure(const PropList& props) {
	CFGProcessor::configure(props);
	path = PATH(props);
}


/**
 * <p>Read the constraint file and add it to the current ILP system.</p>
 * <p>The read file path is taken from configuration if available, or built
 * from the binary file path with ".ipet" appended.</p>
 */
void ConstraintLoader::processCFG(WorkSpace *_fw, CFG *cfg) {

	// Initialization
	fw = _fw;
	system = ipet::SYSTEM(_fw);
	ASSERT(system);

	// Select the file
	if(!path) {
		elm::StringBuffer buffer;
		buffer <<_fw->process()->program()->name() << ".ipet";
		path = buffer.toString();
		if(logFor(LOG_DEPS))
			log << "\tno file provided: trying with " << path << io::endl;
	}

	// Open the file
	if(logFor(LOG_DEPS))
		log << "\tloading constraints from " << path << io::endl;
	ipet_in = fopen(&path.toCString(), "r");
	if(!ipet_in)
		throw ProcessorException(*this, _ << "cannot open the constraint file \"" << &path << "\".");

	// Perform the parsing
	ipet_parse(this);

	// Close all
	fclose(ipet_in);
}


/**
 * Display an error with line number.
 * @param message	Message to display.
 */
void ConstraintLoader::error(string message) {
	log << "ERROR: " << ipet_line << ": " << message << io::endl;
}


/**
 * Display an error message and stop the execution (raising an exception).
 * @param message	Message to display.
 */
void ConstraintLoader::fatal(string message) {
	error(message);
	throw otawa::Exception("fatal error: stopping.");
}

} } // otawa::ipet
