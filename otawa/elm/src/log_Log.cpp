/*
 *	elm::log::Debug class implementation
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
#include <elm/log/Log.h>

/** @defgroup log Logging and pretty-printing
 *  Module for parameTrized pretty-printing logs on the console, with colors, automatically prefixed by source path and such.
 */

namespace elm { namespace color {
/**
 * @class Color
 * @ingroup log
 * @brief Define ANSI colors (including foreground, background, bold and underline) for global use in std C/elm strings/outstreams, using implicit type conversions.
 */
}}

namespace elm { namespace log {
/**
 * @class Debug
 * @ingroup log
 * @brief Provide necessary static methods to configure debugging for the use of the ELM_DBG* macros.
 */


/**
 * @ingroup log
 * @var int Debug::_flags;
 * @brief Flags to set for the debugging macros.
 * * DEBUG: Enable debugging\n
 * * SOURCE_INFO: Display the source path (with line numbers)\n
 * * NUMBERING: Number the log lines\n
 * * COLOR/COLORS: Use colors
 */
int Debug::_flags = DEBUG | COLOR | SOURCE_INFO | NUMBERING; // default to enabled debugging with colors, prefixed by source path and log line numbers

/**
 * @ingroup log
 * @var int Debug::_verbose_level;
 * @brief Verbose bit mask.
 * ELM_DBGV will only be enabled if the provided parameTer level & verbose_level is non-null  
 */
int Debug::_verbose_level = 0xffff; // default to activate everything 

/**
 * @ingroup log
 * @var int Debug::_srcpath_length;
 * @brief Numeric value that defines the constant length of the source path in the prefix.
 * @warning: must be > 3
 */
int Debug::_srcpath_length = 20; // default to 20 characters

/**
 * @ingroup log
 * @var int Debug::_function_name_length;
 * @brief Numeric value that defines the constant length of the function name in the prefix.
 * @warning: must be > 3
 */
int Debug::_function_name_length = 15; // default to 15 characters

/**
 * @ingroup log
 * @var elm::color::Color _prefix_color;
 * @brief Color used for printing the prefix message.
 * The default value is elm::color::Yel
 */
elm::color::Color Debug::_prefix_color = elm::color::Yel;

/**
 * @fn static bool Debug::getDebugFlag();
 * @brief Check if debugging messages are enabled
 * @return True if the flag is enabled
 */
/**
 * @ingroup log
 * @fn static void Debug::setDebugFlag(bool set);
 * @brief Enable/Disable debugging messages
 * @param Boolean to set the flag to
 */
/**
 * @fn static bool Debug::getSourceInfoFlag();
 * @brief Check if printing source info in the prefix of logs is enabled
 * @return True if the flag is enabled
 */
/**
 * @ingroup log
 * @fn static void Debug::setSourceInfoFlag(bool set);
 * @brief Enable/Disable printing source info in the prefix of logs
 * @param Boolean to set the flag to
 */
/**
 * @fn static bool Debug::getNumberingFlag();
 * @brief Check if the numbering of the logs is enabled
 * @return True if the flag is enabled
 */
/**
 * @ingroup log
 * @fn static void Debug::setNumberingFlag(bool set);
 * @brief Enable/Disable numbering of the logs
 * @param Boolean to set the flag to
 */
/**
 * @fn static bool Debug::getColorFlag();
 * @brief Check if colors are enabled for the logs
 * @return True if the flag is enabled
 */
/**
 * @ingroup log
 * @fn static void Debug::setColorFlag(bool set);
 * @brief Enable/Disable the use of colors in logs
 * @param Boolean to set the flag to
 */
/**
 * @fn static int Debug::getVerboseLevel();
 * @brief Retrieve the current verbose level 
 * @return The current verbose level
 */
/**
 * @ingroup log
 * @fn static void Debug::setVerboseLevel(int verbose_level);
 * @brief Set the verbose level to use
 * @param The new verbose level to be set
 */
/**
 * @fn static int Debug::getSourcePathLength();
 * @brief Retrieve the current length set for the source path in the prefix of logs
 * @return The current source path length in the prefix
 */
/**
 * @ingroup log
 * @fn static void Debug::setSourcePathLength(int srcpath_length);
 * @brief Set a new source path length to use in the prefix of logs
 * @param The new source path length to use for the prefix
 */
/**
 * @fn static color::Color Debug::getPrefixColor();
 * @brief Get the current color of the prefix of logs
 * @return The current color of the prefix of logs
 */
/**
 * @ingroup log
 * @fn static void Debug::setPrefixColor(color::Color prefix_color)
 * @brief Set a new  color for the prefix of logs
 * @param The new color to use for the prefix of logs
 */

/**
 * @ingroup log
 * @def ELM_DBG(str);
 * @param Str the string to print
 * @brief Format and output the string str on the log, depending on the global parameTers set in flags and srcpath_length.\n
 * If ELM_NO_DBG is not defined, the alias DBG is available.
 */

/**
 * @ingroup log
 * @def ELM_DBGLN(str);
 * @param Str the string to print
 * @brief The same as ELM_DBG, except it adds a new line character at the end.\n
 * If ELM_NO_DBG is not defined, the alias DBG is available.
 */

/**
 * @ingroup log
 * @def ELM_DBGV(level, str);
 * @param Str the string to print
 * @param Level bitmask to select the levels of verbose that should enable this debug. If level=0, it will never be printed.
 * @brief Will only output the string str if (level & verbose_level) is non-null, that is, if one of the bits in the bit mask level is set to 1 in the global variable verbose_level.
 */

/**
 * @ingroup log
 * @brief Display prefix of log line, which may include the source path and line number of file, the line number of the log, both or neither
 * @return The string to display
 */
elm::String Debug::debugPrefixWrapped(const char* file, int line)
{
	if(getSourceInfoFlag() || getNumberingFlag())
	{
		elm::String rtn = _ << _prefix_color << "["; // opening bracket and setting up color if we are in color mode
		if(getSourceInfoFlag()) // path of the source that called DBG
		{
			rtn = _ << file << ":" << line;
			if(rtn.length() > getSourcePathLength())
			{ 	// Source path too long, cut it: "longpath/src/main.cpp" becomes [...ath/src/main.cpp])
				rtn = _ << _prefix_color << "[" << "..." << rtn.substring(rtn.length() + 3 - getSourcePathLength());
			}
			else
			{	// Source path too short, align it with whitespaces: "src/main.cpp" becomes [       src/main.cpp]
				elm::String whitespaces;
				for(unsigned int i = 0, len = rtn.length(); i < getSourcePathLength() - len; i++)
					whitespaces = whitespaces.concat(elm::CString(" "));
				rtn = _ << _prefix_color << "[" << whitespaces << rtn;
			}
		}
		if(getNumberingFlag()) // output line numbers
		{
			static int line_nb = 0;
			if(getSourceInfoFlag()) // if we printed the source info before
				rtn = _ << rtn << "|"; // add a | separator
			rtn = _ << rtn << io::align(io::RIGHT, io::width(6, ++line_nb)); // this auto-adds the necessary whitespaces to have a 6-characters number. 45 becomes "    45" and so on
		}
		return elm::String(_ << rtn << "]" << color::RCol()); // closing bracket of the prefix, and resetting color if need be
	}
	else // no prefix
		return "";
}

elm::String Debug::debugPrefix(const char* file, int line, const char* functionName) {
	if(getSourceInfoFlag() || getNumberingFlag())
	{
		elm::String rtn = _ << debugPrefixWrapped(file, line);
		if(elm::String(functionName).length() == 0)
			rtn = _ << rtn << " ";
		else
			rtn = _ << rtn << debugPrefixFunction(functionName);

		return elm::String(rtn);
	}
	else
		return "";
}

elm::String Debug::debugPrefixFunction(const char* functionName)
{
	if(getSourceInfoFlag())
	{
		elm::String rtn = _ << _prefix_color << "["; // opening bracket and setting up color if we are in color mode
		if(getSourceInfoFlag()) // path of the source that called DBG
		{
			rtn = _ << functionName;
			if(rtn.length() > getFunctionNameLength())
			{ 	// Source path too long, cut it: "longpath/src/main.cpp" becomes [...ath/src/main.cpp])
				rtn = _ << _prefix_color << "[" << "..." << rtn.substring(rtn.length() + 3 - getFunctionNameLength());
			}
			else
			{	// Source path too short, align it with whitespaces: "src/main.cpp" becomes [       src/main.cpp]
				elm::String whitespaces;
				for(unsigned int i = 0, len = rtn.length(); i < getFunctionNameLength() - len; i++)
					whitespaces = whitespaces.concat(elm::CString(" "));
				rtn = _ << _prefix_color << "[" << whitespaces << rtn;
			}
		}
		return elm::String(_ << rtn << "] " << color::RCol()); // closing bracket of the prefix, and resetting color if need be
	}
	else // no prefix
		return " ";
}

color::Color Debug::getPrefixColor()
	{ return _prefix_color; }

void Debug::setPrefixColor(const elm::color::Color& prefix_color)
	{ _prefix_color = prefix_color; }

} // log
} // elm
