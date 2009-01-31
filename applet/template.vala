namespace Template {
	const int NORMAL = 0;
	const int KEYWORD = 1;
	public string replace_simple(string template, string keyword, string value) {
		string[] subs = { keyword, value };
		return replace(template, subs);
	}
	public string replace (string template, string[] subs) {
		weak string p = template;
		unichar c;
		int state = NORMAL;
		weak string keyword = null;
		unichar keyword_open = 0;
		StringBuilder sb = new StringBuilder("");
		while(0 != (c = p.get_char())) {
			switch(state) {
				case NORMAL:
					switch(c) {
						case '@':
						case '%':
							keyword_open = c;
							keyword = p;
							state = KEYWORD;
						break;
						default:
							sb.append_unichar(c);
						break;
					}
				break;
				case KEYWORD:
					switch(c) {
						default:
							if(c == keyword_open) {
								bool found = false;
								for(int i = 0; i< subs.length; i+=2) {
									weak string st_keyword = subs[i];
									weak string st_value = subs[i+1];
									weak string pp = keyword;
									weak string qq = st_keyword;
									unichar cc = 0;
									unichar dd = 0;
									/* strcmp w.r.t st_keyword*/
									while(0 != (cc = pp.get_char())
									&&    0 != (dd = qq.get_char())) {
										if(cc != dd) {
											break;
										}
										qq = qq.next_char();
										pp = pp.next_char();
									}
									if(dd == 0) {
										sb.append(st_value);
										found = true;
										break;
									}
								}
								if(!found) {
									sb.append_unichar(c);
									sb.append(keyword);
									sb.append_unichar(c);
								}
								state = NORMAL;
								keyword = null;
							}
						break;
					}
				break;
			}
			p = p.next_char();
		}
		return sb.str;
	}
}
