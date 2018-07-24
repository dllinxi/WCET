/*
 * $Id$
 * Copyright (c) 2004, Alfheim Corporation.
 *
 * io.h -- formatted IO classes interfaces.
 */
#ifndef ELM_IO_IO_H
#define ELM_IO_IO_H

#include <elm/sys/SystemIO.h>
#include <elm/io/IOException.h>
#include <elm/io/Output.h>
#include <elm/io/Input.h>

namespace elm {

// Standard environment
extern io::Output cout;
extern io::Output cerr;
extern io::Input cin;

} // elm

#endif	// ELM_IO_IO_H
