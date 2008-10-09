#include <stdio.h>

int main(){
	char valaname[1024];
	char lowercname[4096];
	int i, j;
	fgets(valaname, 1000, stdin);
	for(i=0, j=0; valaname[i]!='\n' && valaname[i]!=0; i++) {
		if(valaname[i] == '.') {
			lowercname[j++] = '_'; 
			continue;
		}
		if(isupper(valaname[i])) {
			if(i >0 
			&& valaname[i-1] != '.' 
			&& valaname[i-1] != '_' 
			&& !isupper(valaname[i-1])) {
				lowercname[j++] = '_';
			} 
		}
		lowercname[j++] = tolower(valaname[i]);
	}
	lowercname[j] = 0;
	fputs(lowercname, stdout);
	return 0;
}
