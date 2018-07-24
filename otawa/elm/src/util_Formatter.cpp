/*
 * $Id$
 * Copyright (c) 2006, IRIT-UPS <casse@irit.fr>
 *
 * elm/util_Formatter.cpp -- Formatter class implementation.
 */

#include <elm/util/Formatter.h>
using namespace elm;
using namespace elm::io;

namespace elm { namespace util {

/**
 * @class Formatter
 * Class for formatting some parts of a stream. The changed parts starts with
 * an escape character ('%' as default). When such a sequenceis found, the
 * method Formatter::process() is called with the following character and may :
 * <ul>
 * <li>reject the character,</li>
 * <li>accept it and perform a specific output,</li>
 * <li>accept the character and answer for the next character (for multi-character
 * escape sequence).</li>
 * </ul>
 * @par
 * Note that two times the escape character is replaced by one occurrence of the
 * escape character.
 */


/**
 * @fn int Formatter::process(io::OutStream& out, char chr);
 * This method must be overloaded for performing special formatting.
 * @param out	Out stream to perform output in.
 * @param chr	Escaped character.
 * @return		One of DONE, CONTINUE, REJECT or a negative error code.
 */


/**
 * Build a new formatter.
 * @param escape	Escape character ('%' as default).
 */
Formatter::Formatter(char escape): esc(escape) {
}


/**
 * Format the given input stream to the given output stream.
 * @param in	Input stream.
 * @param out	Output streal (default to io::stdout).
 * @return		0 for success, <0 for an IO error.
 */
int Formatter::format(io::InStream& in, io::OutStream& out) {
	int chr, res;
	chr = in.read();
	while(chr != InStream::ENDED) {
		if(chr != esc)
			res = out.write(chr);
		else {
			chr = in.read();
			if(chr < 0)
				break;
			if(chr == esc)
				res = out.write(chr);
			else {
				bool work = true;
				while(work) {					
					switch(process(out, chr)) {
					case DONE:
					case REJECT:
						work = false;
						break;
					case CONTINUE:
						chr = in.read();
						if(chr < 0)
							return chr == InStream::FAILED ? -1 : 0;
					default:
						return res;
					}
				}
			}
		}
		if(res < 0)
			return res;
		chr = in.read();
	}
	return chr == InStream::FAILED ? -1 : 0;
}


/**
 * @fn char Formatter::escape(void) const;
 * Get the escape character.
 * @return	Escape character.
 */

} } // elm::util
