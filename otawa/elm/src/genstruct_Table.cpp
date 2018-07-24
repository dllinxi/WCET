/*
 *	$Id$
 *	Table class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2004-07, IRIT UPS.
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

#include <elm/genstruct/Table.h>

namespace elm { namespace genstruct {

/**
 * @class Table Table.h elm/genstruct/Table.h
 * This class allows handling simple table stored as C tables with an item
 * count.
 * @param T Type of the items in the table.
 * @ingroup gen_datastruct
 */


/**
 * @fn Table::Table(T *table, int count);
 * Build a new table.
 * @param table		C table containings items.
 * @param count		Item count.
 */


/**
 * @fn int Table::count(void) const;
 * Get the items count in the table.
 * @return Items count.
 */


/**
 * @fn const T *Table::table(void) const;
 * Get the C table as writable.
 * @return Writeable C table.
 */


/**
 * @fn T *Table::table(void);
 * Get the C table as read-only.
 * @return	Read-only C table.
 */


/**
 * @fn T Table::get(int index) const;
 * Get a table item.
 * @param index		Index of the item to get.
 * @return			Value of the item.
 */


/**
 * @fn T& Table::get(int index);
 * Get a reference on a table item.
 * @param index		Index of the item to get.
 * @return			Reference of the item.
 */


/**
 * @fn void Table::set(int index, const T& value);
 * Set the value of an item.
 * @param index	Item index.
 * @param value	Item value.
 */


/**
 * @fn T Table::operator[](int index) const;
 * Same as Table::get() const.
 */


/**
 * @fn T& Table::operator[](int index);
 * Same as Table:get().
 */

/**
 * @fn Table<T>& Table::operator=(const Table& table);
 * Implements assignment for tables.
 * @param table	Table to assign.
 * @return		Current table.
 */

} } // elm::genstruct
