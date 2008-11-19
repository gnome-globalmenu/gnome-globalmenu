/* testhashmap.vala
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

const string CODE_NOT_REACHABLE = "*code should not be reached*";

void test_hashmap_get () {
	var hashmap = new HashMap<string,string> (str_hash, str_equal, str_equal);
	
	// Check get from empty map
	assert (hashmap.get ("foo") == null);
	
	// Check get from map with one items
	hashmap.set ("key", "value");
	assert (hashmap.get ("key") == "value");
	
	// Check get from non-existing key
	assert (hashmap.get ("foo") == null);
	
	// Check get from map with multiple items
	hashmap.set ("key2", "value2");
	hashmap.set ("key3", "value3");
	assert (hashmap.get ("key") == "value");
	assert (hashmap.get ("key2") == "value2");
	assert (hashmap.get ("key3") == "value3");
	
}

void test_hashmap_set () {
	var hashmap = new HashMap<string,string> (str_hash, str_equal, str_equal);
	
	// check map is empty
	assert (hashmap.size == 0);
	
	// check set an item to map
	hashmap.set ("abc", "one");
	assert (hashmap.contains ("abc"));
	assert (hashmap.get ("abc") == "one");
	assert (hashmap.size == 1);
	
	// check set an item to map with same value
	hashmap.set ("def", "one");
	assert (hashmap.contains ("def"));
	assert (hashmap.get ("abc") == "one");
	assert (hashmap.get ("def") == "one");
	assert (hashmap.size == 2);
	
	// check set with same key
	hashmap.set ("def", "two");
	assert (hashmap.contains ("def"));
	assert (hashmap.get ("abc") == "one");
	assert (hashmap.get ("def") == "two");
	assert (hashmap.size == 2);
}

void test_hashmap_remove () {
	var hashmap = new HashMap<string,string> (str_hash, str_equal, str_equal);
	
	// check removing when map is empty
	hashmap.remove ("foo");
	assert (hashmap.size == 0);
	
	// add items
	hashmap.set ("aaa", "111");
	hashmap.set ("bbb", "222");
	hashmap.set ("ccc", "333");
	hashmap.set ("ddd", "444");
	assert (hashmap.size == 4);
	
	// check remove on first place
	hashmap.remove ("aaa");
	assert (hashmap.size == 3);
	
	// check remove in between 
	hashmap.remove ("ccc");
	assert (hashmap.size == 2);
	
	// check remove in last place
	hashmap.remove ("ddd");
	assert (hashmap.size == 1);
	
	// check remove invalid key
	hashmap.remove ("bar");
	
	// check remove last in map
	hashmap.remove ("bbb");
	assert (hashmap.size == 0);
}

void test_hashmap_contains () {
	var hashmap = new HashMap<string,string> (str_hash, str_equal, str_equal);
	
	// Check on empty map
	assert (!hashmap.contains ("111"));
	
	// Check items
	hashmap.set ("10", "111");
	assert (hashmap.contains ("10"));
	assert (!hashmap.contains ("20"));
	assert (!hashmap.contains ("30"));
	
	assert (hashmap.get ("10") == "111");
	
	hashmap.set ("20", "222");
	assert (hashmap.contains ("10"));
	assert (hashmap.contains ("20"));
	assert (!hashmap.contains ("30"));
	
	assert (hashmap.get ("10") == "111");
	assert (hashmap.get ("20") == "222");
	
	hashmap.set ("30", "333");
	assert (hashmap.contains ("10"));
	assert (hashmap.contains ("20"));
	assert (hashmap.contains ("30"));
	
	assert (hashmap.get ("10") == "111");
	assert (hashmap.get ("20") == "222");
	assert (hashmap.get ("30") == "333");
	
	// Clear and recheck
	hashmap.clear ();
	assert (!hashmap.contains ("10"));
	assert (!hashmap.contains ("20"));
	assert (!hashmap.contains ("30"));
	
	var hashmapOfInt = new HashMap<int,int> ();
	
	// Check items
	hashmapOfInt.set (10, 111);
	assert (hashmapOfInt.contains (10));
	assert (!hashmapOfInt.contains (20));
	assert (!hashmapOfInt.contains (30));
	
	assert (hashmapOfInt.get (10) == 111);
	
	hashmapOfInt.set (20, 222);
	assert (hashmapOfInt.contains (10));
	assert (hashmapOfInt.contains (20));
	assert (!hashmapOfInt.contains (30));
	
	assert (hashmapOfInt.get (10) == 111);
	assert (hashmapOfInt.get (20) == 222);
	
	hashmapOfInt.set (30, 333);
	assert (hashmapOfInt.contains (10));
	assert (hashmapOfInt.contains (20));
	assert (hashmapOfInt.contains (30));
	
	assert (hashmapOfInt.get (10) == 111);
	assert (hashmapOfInt.get (20) == 222);
	assert (hashmapOfInt.get (30) == 333);
}

void test_hashmap_size () {
	var hashmap = new HashMap<string,string> (str_hash, str_equal, str_equal);
	
	// Check empty map
	assert (hashmap.size == 0);
	
	// Check when one item
	hashmap.set ("1", "1");
	assert (hashmap.size == 1);
	
	// Check when more items
	hashmap.set ("2", "2");
	assert (hashmap.size == 2);
	
	// Check when items cleared
	hashmap.clear ();
	assert (hashmap.size == 0);
}

void test_hashmap_get_keys () {
	var hashmap = new HashMap<string,string> (str_hash, str_equal, str_equal);
	
	// Check keys on empty map
	var keySet = hashmap.get_keys ();
	assert (keySet.size == 0);
	
	// Check keys on map with one item
	hashmap.set ("aaa", "111");
	assert (keySet.size == 1);
	assert (keySet.contains ("aaa"));
	keySet = hashmap.get_keys ();
	assert (keySet.size == 1);
	assert (keySet.contains ("aaa"));
	
	// Check modify key set directly
	if (Test.trap_fork (0, TestTrapFlags.SILENCE_STDOUT | TestTrapFlags.SILENCE_STDERR)) {
		keySet.add ("ccc");
		return;
	}
	Test.trap_assert_failed ();
	Test.trap_assert_stderr (CODE_NOT_REACHABLE);
	
	// Check keys on map with multiple items
	hashmap.set ("bbb", "222");
	assert (keySet.size == 2);
	assert (keySet.contains ("aaa"));
	assert (keySet.contains ("bbb"));
	keySet = hashmap.get_keys ();
	assert (keySet.size == 2);
	assert (keySet.contains ("aaa"));
	assert (keySet.contains ("bbb"));
	
	// Check keys on map clear
	hashmap.clear ();
	assert (keySet.size == 0);
	keySet = hashmap.get_keys ();
	assert (keySet.size == 0);
	
}

void test_hashmap_get_values () {
	var hashmap = new HashMap<string,string> (str_hash, str_equal, str_equal);
	
	// Check keys on empty map
	var valueCollection = hashmap.get_values ();
	assert (valueCollection.size == 0);
	
	// Check keys on map with one item
	hashmap.set ("aaa", "111");
	assert (valueCollection.size == 1);
	assert (valueCollection.contains ("111"));
	valueCollection = hashmap.get_values ();
	assert (valueCollection.size == 1);
	assert (valueCollection.contains ("111"));
	
	// Check modify key set directly
	if (Test.trap_fork (0, TestTrapFlags.SILENCE_STDOUT | TestTrapFlags.SILENCE_STDERR)) {
		valueCollection.add ("ccc");
		return;
	}
	Test.trap_assert_failed ();
	Test.trap_assert_stderr (CODE_NOT_REACHABLE);
	
	// Check keys on map with multiple items
	hashmap.set ("bbb", "222");
	assert (valueCollection.size == 2);
	assert (valueCollection.contains ("111"));
	assert (valueCollection.contains ("222"));
	valueCollection = hashmap.get_values ();
	assert (valueCollection.size == 2);
	assert (valueCollection.contains ("111"));
	assert (valueCollection.contains ("222"));
	
	// Check keys on map clear
	hashmap.clear ();
	assert (valueCollection.size == 0);
	valueCollection = hashmap.get_values ();
	assert (valueCollection.size == 0);

}

void test_hashmap_clear () {
	var hashmap = new HashMap<string,string> (str_hash, str_equal, str_equal);
	assert (hashmap.size == 0);
	
	// Check clear on empty map
	hashmap.clear ();
	assert (hashmap.size == 0);
	
	// Check clear one item
	hashmap.set ("1", "1");
	assert (hashmap.size == 1);
	hashmap.clear ();
	assert (hashmap.size == 0);
	
	// Check clear multiple items
	hashmap.set ("1", "1");
	hashmap.set ("2", "2");
	hashmap.set ("3", "3");
	assert (hashmap.size == 3);
	hashmap.clear ();
	assert (hashmap.size == 0);
}

void main (string[] args) {
	Test.init (ref args);

	Test.add_func ("/HashMap/Map/get", test_hashmap_get);
	Test.add_func ("/HashMap/Map/set", test_hashmap_set);
	Test.add_func ("/HashMap/Map/remove", test_hashmap_remove);
	Test.add_func ("/HashMap/Map/contains", test_hashmap_contains);
	Test.add_func ("/HashMap/Map/size", test_hashmap_size);
	Test.add_func ("/HashMap/Map/get_keys", test_hashmap_get_keys);
	Test.add_func ("/HashMap/Map/get_values", test_hashmap_get_values);
	Test.add_func ("/HashMap/Map/clear", test_hashmap_clear);

	Test.run ();
}

