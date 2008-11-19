using GLib;
using Gee;

namespace DOM {
	/***************
	 *
	 * Differ from DOM Spec:
	 *
	 * length and offset are measured in characters rather than 16bit words.
	 * and they are long instead of being ulong.
	 * */
	public class CharacterData : Node {
		private CharacterData(){/*Never create any instances*/ }
		/* CharacterData Interface */
		public string data { get {return nodeValue;} construct set {nodeValue = value;}}
		public ulong length {get {return data.length;}}

		public string substringData(long offset, long count) throws Exception {
			check_offset(offset, true);
			check_count(offset, count, false);

			return data.substring(offset, count);
		}
		public void appendData(string arg) {
			data += arg;
		}
		public void insertData(long offset, string arg) throws Exception {
			check_offset(offset, false);
			if(offset == 0) {
				data = arg + data;
				return;
			}
			if(offset == data.length) {
				data = data + arg;	
				return ;
			}
			string part1 = data.substring(0, offset - 1);
			string part2 = data.substring(offset, data.length - offset);
			data = part1 + arg + part2;
		}
		public void deleteData(long offset, long count) throws Exception {
			check_offset(offset, true);
			check_count(offset, count, false);

			if(offset == 0 && offset + count == data.length) {
				data = "";
				return;
			}
			if(offset == 0) {
				data = data.substring(offset + count, data.length - offset - count);
				return;
			}
			if(offset + count == data.length) {
				data = data.substring(0, offset);
				return;
			}
			string part1 = data.substring(0, offset);
			string part2 = data.substring(offset + count, data.length - offset - count);
			data = part1 + part2;
		}
		public void replaceData(long offset, long count, string arg) throws Exception {
			check_offset(offset, true);
			check_count(offset, count, false);
			if(offset == 0 && offset + count == data.length) {
				data = arg;
				return;
			}
			if(offset == 0) {
				data = arg + data.substring(offset + count, data.length - offset - count);
				return;
			}
			if(offset + count == data.length) {
				data = data.substring(0, offset) + arg;
				return;
			}
			string part1 = data.substring(0, offset);
			string part2 = data.substring(offset + count, data.length - offset - count);
			data = part1 + arg + part2;
		}
		private void check_offset(long offset, bool strict) throws Exception {
			if(offset < 0) throw new Exception.INDEX_SIZE_ERR("offset less than zero");
		   	if(strict && offset >= data.length) throw new Exception.INDEX_SIZE_ERR("offset not strictly inside data length");
		   	if(!strict && offset > data.length) throw new Exception.INDEX_SIZE_ERR("offset not loosely inside data length");
		}
		private void check_count(long offset, long count, bool strict) throws Exception {
			if(count < 0) throw new Exception.INDEX_SIZE_ERR("count less than zero");
		   	if(strict && offset + count >= data.length) throw new Exception.INDEX_SIZE_ERR("count not strictly inside data length");
		   	if(!strict && offset + count > data.length) throw new Exception.INDEX_SIZE_ERR("count not loosely inside data length");
		}
	}

}
