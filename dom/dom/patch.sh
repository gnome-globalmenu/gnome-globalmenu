#! /bin/bash


function patch {
	edit=$1;
	shift;
	for i in $*; do
		echo -n patching $i with $edit ...
		if ! sed -e "$edit" $i > $i.new; then
			echo failed.
		else
			echo OK.
		fi
		mv $i.new $i; 
	done;
}

patch '/g_type_create_instance/d' \
	dom-comment.c \
	dom-text.c \
	dom-document.c \
	dom-attr.c \
	dom-element.c \
	dom-documentfragment.c \
	dom-extended.c

patch '/glong\ ref_count/d' dom-node.h

patch '/return dom_node_insertBefore (self, newChild, NULL, &inner_error);/c 	return\ dom_node_insertBefore\ (self,\ newChild,\ NULL,\ error);' dom-node.c

patch '/attr = ((DOMNode\*) (((DOMNode\*) (gee_iterator_get (attr_it)))));/c attr=dom_node_ref(gee_iterator_get(attr_it));' dom-visitor.c
