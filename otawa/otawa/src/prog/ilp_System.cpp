/*
 *	$Id$
 *	Copyright (c) 2005, IRIT UPS.
 *
 *	src/ilp_System.h -- documentation of the system interface.
 */

#include <otawa/otawa.h>
#include <otawa/ilp/System.h>
#include <math.h>

namespace otawa { namespace ilp {

/**
 * Assistant to perform ILP system dump.
 */
class Dumper {
public:
	Dumper(void): vcnt(0) { }

	string name(ilp::Var *var) {
		string r = var->name();
		if(!r) {
			r = map.get(var, "");
			if(!r) {
				r = _ << "x" << vcnt;
				map.put(var, r);
				vcnt++;
			}
		}
		return r;
	}

private:
	int vcnt;
	HashTable<Var *, string> map;
};


/**
 * @class System
 * An ILP system is a colletion of ILP constraint that may maximize or minimize
 * some object function.
 * @ingroup ilp
 */


/**
 * @fn Constraint *System::newConstraint(Constraint::comparator_t comp, double constant = 0);
 * Build a new constraint that may be initialized by the user. As the ILP system
 * manage the memory of the constraint, the constraint must never be deleted
 * by the user.
 * @param comp		Comparator used (one of Constraint::LT, LE, EQ, GT, GE).
 * @param constant	Constant value.
 * @return			Built constraint.
 */


/**
 * @fn boolean System::solve(WorkSpace *ws);
 * Solve the ILP system.
 * @param ws	If passed (default to null), the given workspace is used to test
 * 				if the computation is cancelled in order to stop the solver.
 * @return		True if the resolution is succesful or false else (mainly due
 * 				to lack of constraint).
 */


/**
 * @fn void System::addObjectFunction(double coef, Var *var = 0);
 * Add a factor to the object function.
 * @param coef	Coefficient of the factor.
 * @param var	Variable of the factor.
 */


/**
 * @fn double System::valueOf(Var *var);
 * This method can only be called after the resolution of the system and returns
 * the value of a variable. It is an error to pass a variable not involved in
 * the system.
 * @param var	Variable whose value is looked for.
 * @return		Value of the variable.
 */


/**
 * @fn double System::value(void);
 * Return the value of the optimized object function.
 * @return	Object function optimum.
 */


/**
 * Dump the system to the given output. The dumping format depends upon the
 * actual used ILP engine. Usually, it is compatible with other tools of
 * the ILP engine.
 * @param out	Used output.
 * @deprecated Use dumpSystem() and dumpSolution() instead.
 */
void System::dump(elm::io::OutStream& out) {
	io::Output output(out);
	dumpSystem(output);
	dumpSolution(output);
}


/**
 * Solve the ILP system in the given workspace
 * using the given monitor to output logs. The workspace may be used
 * to support cancellation or progression in user interface.
 * @param ws	Current workspace.
 * @param mon	Monitor to use.
 */
bool System::solve(WorkSpace *ws, otawa::Monitor& mon) {
	return solve(ws);
}


/**
 * Get the message of the last error.
 * @since 1.2.0 ILP interface.
 * @return	Last error message.
 */
string System::lastErrorMessage(void) {
	return "error";
}


/**
 * @fn Var *System::newVar(Var::type_t type, const string& name);
 * Build a new variable with the given type.
 * @param type	Type of the variable.
 * @param name	Variable name.
 * @return		Built variable.
 * @since		ILP 1.2.0
 */


/**
 * Return the owner plugin. As a default, return null.
 * @return	Owner plugin.
 */
ILPPlugin *System::plugin(void) {
	return 0;
}


/**
 * Dump the system to the given output and format.
 * @param out	Used output.
 * @param fmt 	Used format
 */
void System::dump(format_t fmt, elm::io::OutStream& out) {
	switch(fmt) {
	case DEFAULT:	{ io::Output output(out); dumpSystem(output); } break;
	case LP_SOLVE:	dumpLPSolve(out); break;
	case CPLEX:		dumpCPlex(out); break;
	case MOSEK:		dumpMOSEK(out); break;
	default:		ASSERTP(false, "Unsopported ILP system format."); break;
	}
}


/**
 * Output an identifier as a C identifier.
 * Non-C symbols are escaped to _xx where xx is the hexadecimal ASCII code.
 * @param out	Output to output to.
 * @param in	Identifier to output.
 */
class CID: public string {
public:
	inline CID(string s): string(s) { }
};
io::Output& operator<<(io::Output& out, const CID& id) {
	io::IntFormat fmt = io::IntFormat().hex().width(2).pad(0);
	if(!id)
		return out;
	int i = 0;
	if(id[0] >= '0' && id[0] <= '9') {
		i = 1;
		out << '_' << fmt(id[0]);
	}
	for(; i < id.length(); i++) {
		if((id[i] >= '0' && id[i] <= '9')
		|| (id[i] >= 'a' && id[i] <= 'z')
		|| (id[i] >= 'A' && id[i] <= 'Z')
		|| id[i] == '_')
			out << id[i];
		else
			out << '_' << fmt(id[i]);
	}
	return out;
}


/**
 * Print a term.
 * @param out		Output to use.
 * @param term		Term to display.
 * @param dumper	Dumper assistant.
 * @param fst		True if the term is the first of the expression.
 */
static void printTerm(io::Output& out, Term term, Dumper& dumper, bool fst) {
	double val = term.snd;
	if(!val)
		return;
	if(val < 0) {
		out << "- ";
		if(val != -1)
			out << -val << ' ';
	}
	else {
		if(!fst)
			out << "+ ";
		if(val != 1)
			out << val << ' ';
	}
	out << CID(dumper.name(term.fst));
}


/**
 * Dump in LPSolve format.
 * @param _out	Output stream.
 */
void System::dumpLPSolve(io::OutStream& _out) {
	Dumper dumper;
	io::Output out(_out);
	out << "/* IPET system */\n";
	avl::Set<Var *> vars;

	// Output the objective function
	out << "max:";
	bool fst = true;
	for (ObjTermIterator term(this); term; term++) {
		cout << ' ';
		printTerm(out, *term, dumper, fst);
		vars.add((*term).fst);
		fst = false;
	}
	out << ";\n";

	// Output the constraints
	for(datastruct::Iterator<Constraint *> cons(constraints()); cons; cons++) {

		// print positives
		bool pos = false;
		fst = true;
		for(datastruct::Iterator<Term> term(cons->terms()); term; term++) {
			if((*term).snd > 0) {
				if(!fst)
					cout << ' ';
				printTerm(out, *term, dumper, fst);
				vars.add((*term).fst);
				pos = true;
				fst = false;
			}
		}

		// print negative constant
		if(cons->constant() < 0) {
			if(pos)
				out << " + ";
			out << -cons->constant();
		}

		// print comparator
		out << ' ' << cons->comparator();

		// print negatives
		bool neg = false;
		fst = true;
		for(datastruct::Iterator<Term> term(cons->terms()); term; term++)
			if((*term).snd < 0) {
				cout << ' ';
				printTerm(out, Term((*term).fst, -(*term).snd), dumper, fst);
				vars.add((*term).fst);
				neg = true;
				fst = false;
			}

		// print positive constant
		if(!neg && cons->constant() <= 0)
			out << " 0";
		else if(cons->constant() > 0 || (cons->constant() == 0 && !neg)) {
			if(neg)
				out << " +";
			out << ' ' << cons->constant();
		}

		// end of constraint
		out << ";";
		const string& label = cons->label();
		if(label)
			out << "\t/* " << label << "*/";
		out << io::endl;
	}

	// Output int constraints
	for(avl::Set<Var *>::Iterator var(vars); var; var++)
		switch(var->type()) {
		case Var::INT:		out << "int " << CID(dumper.name(var)) << ";\n"; break;
		case Var::FLOAT:	break;
		case Var::BIN:		out << "bin " << CID(dumper.name(var)) << ";\n"; break;
		default:			break;
		}
}


/**
 * Dump in MOSEK format.
 * @param _out	Output stream.
 */
void System::dumpMOSEK(OutStream& _out) {
	io::Output out(_out);
	HashTable<Var*, String*> rename;
	int idx = 0;

	// dump the objective function
	out << "[objective maximize 'obj']\n";
	for (ObjTermIterator term(this); term; term++) {
		t::int32 val = lrint((*term).snd);
		if (!rename.hasKey((*term).fst)) {
			rename.put((*term).fst, new String((_ << "x" << idx)));
			idx++;
		}
		if (val == 0)
			continue;

		out << " ";
		if (val == 1) {
			out << "+ " << *rename.get((*term).fst, 0);
		} else if (val == -1) {
			out << "- " << *rename.get((*term).fst, 0);
		} else {
			out << io::sign(val) << " " << *rename.get((*term).fst, 0);
		}
	}
	out << "\n";
	out << "[/objective]\n";

	// dump the constraints
	out << "[constraints]\n";
	for (ConstIterator cons2(this); cons2; cons2++) {
		out << "[con]";
		//bool bound = true;
		int numvar = 0;
		for (Constraint::TermIterator term(cons2); term; term++) {
			//if ((*term).snd != 1)
			//	bound = false;
			numvar++;
		}
		//if (numvar != 1)
		//	bound = false;

		for (Constraint::TermIterator term(cons2); term; term++) {
			t::int32 val = lrint((*term).snd);
			if (!rename.hasKey((*term).fst)) {
				rename.put((*term).fst, new String((_ << "x" << idx)));
				idx++;
			}
			if (val == 0)
				continue;

			out << " ";
			if (val == 1) {
				out << "+" << *rename.get((*term).fst, 0);
			} else if (val == -1) {
				out << "-" << *rename.get((*term).fst, 0);
			} else {
				out << io::sign(val) << " " << *rename.get((*term).fst, 0);
			}

		}
		switch(cons2->comparator()) {
			case Constraint::LT:
				out << " < ";
				break;
			case Constraint::LE:
				out << " <= ";
				break;
			case Constraint::EQ:
				out << " = ";
				break;
			case Constraint::GE:
				out << " >= ";
				break;
			case Constraint::GT:
				out << " > ";
				break;
			default:
				out << " ?? ";;
				break;
		}

		out << cons2->constant();
		out << " [/con] \n";
	}
	out << "\n";
	out << "[/constraints]\n";

	// dump bounds
	out << "[bounds]\n";
	out << "[b] 0 <= * [/b]\n";
	out << "[/bounds]\n";

	// dump the integer variable definition
	out << "[variables]\n";
	for (HashTable<Var*, String*>::Iterator item(rename); item; item++) {
		String *str = *item;
		out << " ";
		out << *str;
		out << "\n";

	}
	out << "\n";
	out << "[/variables]\n";

	// dump the integer variable definition
	out << "[integer]\n";
	for (HashTable<Var*, String*>::Iterator item(rename); item; item++) {
		String *str = *item;
		out << " ";
		out << *str;
		delete str;
		out << "\n";

	}
	out << "\n";
	out << "[/integer]\n";
}


/**
 * Dump system in CPlex format.
 * @param _out	Output stream to use.
 */
void System::dumpCPlex(OutStream& _out) {
	io::Output out(_out);
	HashTable<Var*, String*> rename;
	int idx = 0;
	/* Rename the variables for cplex */


	out << "\\* Objective function *\\\n";
	out << "Maximize\n";
	/* dump the objective function */

	for (ObjTermIterator term(this); term; term++) {
		t::int32 val = lrint((*term).snd);
		if (!rename.hasKey((*term).fst)) {
			rename.put((*term).fst, new String((_ << "x" << idx)));
			idx++;
		}
		if (val == 0)
			continue;

		out << " ";
		if (val == 1) {
			out << "+" << *rename.get((*term).fst, 0);
		} else if (val == -1) {
			out << "-" << *rename.get((*term).fst, 0);
		} else {
			out << io::sign(val) << " " << *rename.get((*term).fst, 0);
		}
		out << "\n";
	}
	out << "\n\n";

	// dump the constraints
	out << "\\* Constraints *\\\n";
	out << "Subject To\n";
	for (ConstIterator cons2(this); cons2; cons2++) {
		bool bound = true;
		int numvar = 0;
		for (Constraint::TermIterator term(cons2); term; term++) {
			if ((*term).snd != 1)
				bound = false;
			numvar++;
		}
		if (numvar != 1)
			bound = false;

		if (bound)
			continue;

		for (Constraint::TermIterator term(cons2); term; term++) {
			t::int32 val = lrint((*term).snd);
			if (!rename.hasKey((*term).fst)) {
				rename.put((*term).fst, new String((_ << "x" << idx)));
				idx++;
			}
			if (val == 0)
				continue;

			out << " ";
			if (val == 1) {
				out << "+" << *rename.get((*term).fst, 0);
			} else if (val == -1) {
				out << "-" << *rename.get((*term).fst, 0);
			} else {
				out << io::sign(val) << " " << *rename.get((*term).fst, 0);
			}

		}
		switch(cons2->comparator()) {
			case Constraint::LT:
				out << " < ";
				break;
			case Constraint::LE:
				out << " <= ";
				break;
			case Constraint::EQ:
				out << " = ";
				break;
			case Constraint::GE:
				out << " >= ";
				break;
			case Constraint::GT:
				out << " > ";
				break;
			default:
				out << " ?? ";;
				break;
		}

		out << cons2->constant() << "\n";
	}
	out << "\n\n";

	// dump the bounds
	out << "\\* Variable bounds *\\\n";
	out << "Bounds\n";
	for (ConstIterator cons2(this); cons2; cons2++) {
		bool bound = true;
		int numvar = 0;
		Var *var;
		for (Constraint::TermIterator term(cons2); term; term++) {
			if ((*term).snd != 1)
				bound = false;
			numvar++;
			var = (*term).fst;
		}
		if (numvar != 1)
			bound = false;

		if (!bound)
			continue;

		out << " " << *rename.get(var, 0);
		switch(cons2->comparator()) {
			case Constraint::LT:
				out << " < ";
				break;
			case Constraint::LE:
				out << " <= ";
				break;
			case Constraint::EQ:
				out << " = ";
				break;
			case Constraint::GE:
				out << " >= ";
				break;
			case Constraint::GT:
				out << " > ";
				break;
			default:
				out << " ?? ";;
				break;
		}

		out << cons2->constant() << "\n";
	}
	out << "\n\n";

	// dump the integer variable definition
	out << "\\* Integer definitions *\\\n";
	out << "Integer\n";
	for (HashTable<Var*, String*>::Iterator item(rename); item; item++) {
		String *str = *item;
		out << " ";
		out << *str;
		delete str;
		out << "\n";
	}
	out << "\n\n";

	// dump end
	out << "End\n";
}


/**
 * Tests if it is possible to dump in the given format.
 * @param fmt 	Used format
 * @return	Boolean
 */
bool System::hasDump(format_t fmt) {
	switch(fmt) {
		case DEFAULT:
		case LP_SOLVE:
		case CPLEX:
		case MOSEK:
			return true;
			break;
		default:
			return false;
			break;
	}
}

/**
 * @fn int System::countVars(void);
 * Count the number of variables in the system.
 * @return	Variable count.
 */


/**
 * @fn int System::countConstraints(void);
 * Count the number of constraints in the system.
 * @return	Constraint count.
 */


/**
 * @fn void System::exportLP(io::Output& out);
 * Export the system to the given output using the LP format (lp_solve).
 * @param out	Stream to export to (default to cout).
 */


/**
 * Dump the system in text format (as a default, call exportLP).
 * @param out	Stream to dump to (default to cout).
 */
void System::dumpSystem(io::Output& out) {
	exportLP(out);
}


/**
 * @fn void System::dumpSolution(io::Output& out);
 * Dump the solution textually to the given output.
 * @param out	Stream to output to (default to cout).
 */


/**
 * @fn Var *System::newVar(cstring name = "");
 * Build a new variable.
 * @param name	Name of the variable (may be an empty string for an anonymous
 *				variable).
 * @return		Built variable.
 */


/**
 * Var *System::newVar(const string& name);
 * Build a new variable.
 * @param name	Name of the variable (may be an empty string for an anonymous
 *				variable).
 * @return		Built variable.
 */


/**
 * Build a named constraint.
 * @param label		Label of the constraint.
 * @param comp		Used comparator (one of Constraint::LT, LE, EQ, GT, GE).
 * @param constant	Used constant.
 * @return			Built constraint.
 */
/*Constraint *System::newConstraint(const string& label, Constraint::comparator_t comp, double constant) {
	return newConstraint(comp, constant);
}*/

} // ilp

} // otawa
