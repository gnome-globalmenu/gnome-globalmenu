[Compact]
[CCode (cname = "GStringChunk", cprefix = "g_string_chunk_", free_function = "g_string_chunk_free")]
public class StringChunk {
	public StringChunk (ulong size);
	public weak string insert(string str);
	public weak string insert_const(string str);
	public weak string insert_len(void * buffer, ulong len);
	public void clear();
}
