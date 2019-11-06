/*
    sort(condensed);
    condensed = uniq_c(condensed);
    sort_nr(condensed);
	uniq(condensed);
*/

static int max(int x, int y){
	if(x>y)
		return x;
	return y;
}

static int strcmplite(char s1[], char s2[]){
	while(*s1 && (*s1 == *s2)){
		s1++;
		s2++;
	}
	if(*s1=='\0')
		return 1;
	else
		return 0;
}

static int strcmpminusend(char s1[], char s2[]){
	while(*s1 && (*s1 == *s2)){
		s1++;
		s2++;
	}
	if((*s1=='\0')||(*s1=='\n'))
		if((*s2=='\0')||(*s2=='\n'))
			return 1;
	return 0;
}

static void cleanupend(int moveto, char file[]){
	if(moveto>0){
		if(file[moveto-1]=='\n'){
			file[moveto-1]='\0';
			return;
		}
		else if(file[moveto-1]=='\0'){
			return;
		}
		else{
			file[moveto]='\0';
			return
		}
	}
	else
		file[0]='\0';
}

static void cut_f (int startwfield, int endwfield, char file[]){
	int i=0,moveto=0,field=1,startofline=1;
	while(1){
		if(file[i]=='\t')
			field++;
		else if(file[i]=='\n'){
			field=1;
			startofline=1;
			if(moveto>0){
				if(file[moveto-1]!='\n'){
					file[moveto]='\n';
					moveto++;
				}
			}
			i++;
			continue;
		}
		else if(file[i]=='\0')
			break;

		if(field>=startwfield)
			if((endwfield==0)||(field<=endwfield)){
				if(startofline==0){
					file[moveto]=file[i];
					moveto++;
				}
				else
					startofline=0;//skip extra tab at beginning
			}
		i++;
	}
	cleanupend(moveto,file);
}

static void grep (char lookfor[],char file[]){
	int i=0,start=0,same=0,moveto=0;
	while(1){
		if(file[i]=='\n')
			start=i+1;
		else if(file[i]=='\0')
			break;
		same=strcmplite(lookfor,file+i);
		while(same){
			file[moveto]=file[start];
			moveto++;
			if(file[start]=='\n'){
				i=start-1;
				break;
			}
			else if(file[start]=='\0'){
				i=start-1;
				break;
			}
			start++;
		}
		i++;
	}
	cleanupend(moveto,file);
}

static void grep_v (char lookfor[],char file[]){
	int i=0,start=0,same=0,moveto=0;
	while(1){
		if((file[i]=='\n')||(file[i]=='\0')){
			while(1-same){
				file[moveto]=file[start];
				moveto++;
				if((file[start]=='\n')||(file[start]=='\0')){
					i=start;
					break;
				}
				start++;
			}
			start=i+1;
			same=0;
		}
		if(file[i]=='\0')
			break;
		same=max(same,strcmplite(lookfor,file+i));
		i++;
	}
	cleanupend(moveto,file);
}

static void sort (char file[]){

}

static void sort_nr (char file[]){

}

static void uniq (char file[]){
	int i=0,start=0,same=0,moveto=0, prev=0;
	while(1){
		if(same==0){
			prev = moveto;
			while(1){
				file[moveto]=file[start];
				moveto++;
				if((file[start]=='\n')||(file[start]=='\0')){
					i=start;
					break;
				}
				start++;
			}
		}
		if(file[start]=='\0')
			break;
		else if(file[start]=='\n')
			start++;
		same=strcmpminusend(file+prev,file+start);
	}
	cleanupend(moveto,file);
}

static char* uniq_c (char file[]){

}
