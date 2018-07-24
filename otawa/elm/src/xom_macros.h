#ifndef ELM_XOM_MACROS_H
#define ELM_XOM_MACROS_H

#include <elm/xom/Node.h>
#include <libxml/tree.h>

#define DOC(p)	((xmlDocPtr)(p))
#define NODE(p)	((xmlNodePtr)(p))

namespace elm { namespace xom {

/**
 * Get the XOM object linked with this parser representation node.
 * @param xml_node	Parser node.
 * @return			Matching XOM node.
 */
inline Node *Node::get(void *xml_node) {
	if(NODE(xml_node)->_private)
		return (Node *)(NODE(xml_node)->_private);
	else
		return make(xml_node);
}

} } // elm::xom

#endif // ELM_XOM_MACROS_H
