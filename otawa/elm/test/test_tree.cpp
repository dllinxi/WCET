#include <elm/inhstruct/Tree.h>
#include <elm/genstruct/Tree.h>
#include <elm/util/test.h>

using namespace elm;
using namespace elm::inhstruct;

int main(void) {
	
	// simple tree
	CHECK_BEGIN("inhstruct_Tree")
	{
		Tree *t1 = new Tree;
		Tree *t2 = new Tree;
		Tree *t3 = new Tree;
		t1->add(t2);
		t1->add(t3);
		CHECK(!t1->isEmpty());
		Tree::Iterator child(t1);
		CHECK(child == true); 
		CHECK(child == t3);
		child++;
		CHECK(child == true); 
		CHECK(child == t2);
		child++;	
		CHECK(child == false); 
		t1->remove(t3);
		t1->remove(t2);
		CHECK(t1->isEmpty());
	}
	CHECK_END

	// generic tree
	CHECK_BEGIN("genstruct::Tree")
	{
		typedef genstruct::Tree<string> tree_t;
		tree_t *t1 = new tree_t("1");
		tree_t *t2 = new tree_t("2");
		tree_t *t3 = new tree_t("3");
		t1->add(t2);
		t1->add(t3);
		CHECK(!t1->isEmpty());
		tree_t::Iterator child(t1);
		CHECK(child == true); 
		CHECK(child == "3");
		child++;
		CHECK(child == true); 
		CHECK(child == "2");
		child++;	
		CHECK(child == false); 
		t1->remove(t3);
		t1->remove(t2);
		CHECK(t1->isEmpty());
	}
	CHECK_END

}
