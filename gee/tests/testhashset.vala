/* testhashset.vala
 *
 * Copyright (C) 2008  Jürg Billeter
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 * Author:
 * 	Jürg Billeter <j@bitron.ch>
 */

using GLib;
using Gee;

void test_hashset_add () {
	// Check adding of strings
	var hashset = new HashSet<string> (str_hash, str_equal);

	hashset.add ("42");
	assert (hashset.contains ("42"));
	assert (hashset.size == 1);
	
	hashset.add ("43");
	assert (hashset.contains ("42"));
	assert (hashset.contains ("43"));
	assert (hashset.size == 2);
	
	// Check add same element
	assert (hashset.size == 2);
	hashset.add ("43");
	assert (hashset.contains ("42"));
	assert (hashset.contains ("43"));
	assert (hashset.size == 2);
	
	// Check adding of ints
	var hashsetInt = new HashSet<int> ();

	hashsetInt.add (42);
	assert (hashsetInt.contains (42));
	assert (hashsetInt.size == 1);
	
	hashsetInt.add (43);
	assert (hashsetInt.contains (42));
	assert (hashsetInt.contains (43));
	assert (hashsetInt.size == 2);
	
	// Check add same element
	assert (hashsetInt.size == 2);
	hashsetInt.add (43);
	assert (hashsetInt.contains (42));
	assert (hashsetInt.contains (43));
	assert (hashsetInt.size == 2);
	
	// Check adding of objects
	var hashsetObject = new HashSet<Object> ();
	
	var fooObject = new Object();
	hashsetObject.add (fooObject);
	assert (hashsetObject.contains (fooObject));
	assert (hashsetObject.size == 1);
	
	var fooObject2 = new Object();
	hashsetObject.add (fooObject2);
	assert (hashsetObject.contains (fooObject));
	assert (hashsetObject.contains (fooObject2));
	assert (hashsetObject.size == 2);
	
	// Check add same element
	assert (hashsetObject.size == 2);
	hashsetObject.add (fooObject2);
	assert (hashsetObject.contains (fooObject));
	assert (hashsetObject.contains (fooObject2));
	assert (hashsetObject.size == 2);
}

void test_hashset_clear () {
	var hashset = new HashSet<string> (str_hash, str_equal);
	assert (hashset.size == 0);
	
	// Check clear on empty set
	hashset.clear ();
	assert (hashset.size == 0);
	
	// Check clear one item
	hashset.add ("1");
	assert (hashset.size == 1);
	hashset.clear ();
	assert (hashset.size == 0);
	
	// Check clear multiple items
	hashset.add ("1");
	hashset.add ("2");
	hashset.add ("3");
	assert (hashset.size == 3);
	hashset.clear ();
	assert (hashset.size == 0);
}

void test_hashset_contains () {
	var hashsetString = new HashSet<string> (str_hash, str_equal);
	
	// Check on empty set
	assert (!hashsetString.contains ("1"));
	
	// Check items
	hashsetString.add ("10");
	assert (hashsetString.contains ("10"));
	assert (!hashsetString.contains ("20"));
	assert (!hashsetString.contains ("30"));
	
	hashsetString.add ("20");
	assert (hashsetString.contains ("10"));
	assert (hashsetString.contains ("20"));
	assert (!hashsetString.contains ("30"));
	
	hashsetString.add ("30");
	assert (hashsetString.contains ("10"));
	assert (hashsetString.contains ("20"));
	assert (hashsetString.contains ("30"));
	
	// Clear and recheck
	hashsetString.clear ();
	assert (!hashsetString.contains ("10"));
	assert (!hashsetString.contains ("20"));
	assert (!hashsetString.contains ("30"));
	
	var hashsetInt = new HashSet<int> ();
	
	// Check items
	hashsetInt.add (10);
	assert (hashsetInt.contains (10));
	assert (!hashsetInt.contains (20));
	assert (!hashsetInt.contains (30));
	
	hashsetInt.add (20);
	assert (hashsetInt.contains (10));
	assert (hashsetInt.contains (20));
	assert (!hashsetInt.contains (30));
	
	hashsetInt.add (30);
	assert (hashsetInt.contains (10));
	assert (hashsetInt.contains (20));
	assert (hashsetInt.contains (30));
	
	// Clear and recheck
	hashsetInt.clear ();
	assert (!hashsetInt.contains (10));
	assert (!hashsetInt.contains (20));
	assert (!hashsetInt.contains (30));
}

void test_hashset_remove () {
	var hashsetString = new HashSet<string> (str_hash, str_equal);
	
	// Check remove if list is empty
	hashsetString.remove ("42");
	
	// Add 5 different elements
	hashsetString.add ("42");
	hashsetString.add ("43");
	hashsetString.add ("44");
	hashsetString.add ("45");
	hashsetString.add ("46");
	assert (hashsetString.size == 5);
	
	// Check remove first
	hashsetString.remove ("42");
	assert (hashsetString.size == 4);
	assert (hashsetString.contains ("43"));
	assert (hashsetString.contains ("44"));
	assert (hashsetString.contains ("45"));
	assert (hashsetString.contains ("46"));
	
	// Check remove last
	hashsetString.remove ("46");
	assert (hashsetString.size == 3);
	assert (hashsetString.contains ("43"));
	assert (hashsetString.contains ("44"));
	assert (hashsetString.contains ("45"));
	
	// Check remove in between
	hashsetString.remove ("44");
	assert (hashsetString.size == 2);
	assert (hashsetString.contains ("43"));
	assert (hashsetString.contains ("45"));

	// Check removing of int element
	var hashsetInt = new HashSet<int> ();
	
	// Add 5 different elements
	hashsetInt.add (42);
	hashsetInt.add (43);
	hashsetInt.add (44);
	hashsetInt.add (45);
	hashsetInt.add (46);
	assert (hashsetInt.size == 5);
	
	// Remove first
	hashsetInt.remove (42);
	assert (hashsetInt.size == 4);
	assert (hashsetInt.contains (43));
	assert (hashsetInt.contains (44));
	assert (hashsetInt.contains (45));
	assert (hashsetInt.contains (46));
	
	// Remove last
	hashsetInt.remove (46);
	assert (hashsetInt.size == 3);
	assert (hashsetInt.contains (43));
	assert (hashsetInt.contains (44));
	assert (hashsetInt.contains (45));
	
	// Remove in between
	hashsetInt.remove (44);
	assert (hashsetInt.size == 2);
	assert (hashsetInt.contains (43));
	assert (hashsetInt.contains (45));
}

void test_hashset_size () {
	var hashset = new HashSet<string> (str_hash, str_equal);
	
	// Check empty list
	assert (hashset.size == 0);
	
	// Check when one item
	hashset.add ("1");
	assert (hashset.size == 1);
	
	// Check when more items
	hashset.add ("2");
	assert (hashset.size == 2);
	
	// Check when items cleared
	hashset.clear ();
	assert (hashset.size == 0);
}

void test_hashset_iterator () {
	var hashset = new HashSet<string> (str_hash, str_equal);
	
	// Check iterate empty list
	var iterator = hashset.iterator ();
	assert (!iterator.next());
	
	// Check iterate list
	hashset.add ("42");
	hashset.add ("43");
	
	iterator = hashset.iterator ();
	
	// A set is usually not ordered, so this is not a requirement 
	assert (iterator.next());
	string firstString = iterator.get();
	assert (hashset.contains (firstString)); 
	
	assert (iterator.next());
	string secondString = iterator.get();
	assert (hashset.contains (secondString));
	assert (!str_equal (firstString, secondString)); // they can not be identical neither equal
	
	assert (!iterator.next());
}
void main (string[] args) {
	Test.init (ref args);

	Test.add_func ("/HashSet/Collection/add", test_hashset_add);
	Test.add_func ("/HashSet/Collection/clear", test_hashset_clear);
	Test.add_func ("/HashSet/Collection/contains", test_hashset_contains);
	Test.add_func ("/HashSet/Collection/remove", test_hashset_remove);
	Test.add_func ("/HashSet/Collection/size", test_hashset_size);
	
	Test.add_func ("/HashSet/Iterable/iterator", test_hashset_iterator);

	Test.run ();
}

