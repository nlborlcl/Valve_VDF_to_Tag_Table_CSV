#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char * readintobuffer( char* file){
	FILE *fp = fopen(file, "r");
	if (fp == NULL){
		if(file=="sharedconfig.vdf")
			printf("Please find the sharedconfig.vdf file in /home/LINUXUSERID/.local/share/Steam/userdata/{STEAMUSERID}/7/remote/ and place it into this folder.\n");
		else
			printf("Couldn't open file %s.\n",file);
		return NULL;
	}
	fseek(fp, 0, SEEK_END);
	long fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);  /* same as rewind(fp); */
	char *string = malloc(fsize + 1);
	if (string == NULL){
		printf("Couldn't allocate memory.\n");
		return 0;
	}
	fread(string, 1, fsize, fp);
	fclose(fp);
	return string;
}

int tagorgamef (char string[]){
	if(string[0]=='\0')
		return -1;

	char strgame[] = {'\n','\t','\t','\t','\t','\t',34,'\0'};
	char strtag[] = {'\n','\t','\t','\t','\t','\t','\t','\t',34,'\0'};
	int x;
	for (x=0; x<9; x++){
		if(string[x] == strtag[x]){
			continue;
		}
		else if (x == 6){
			if (string[x] == strgame[x])
				return 1;
		}
		else{
			return 0;
		}
	}
	return 2;
}

int findgame(char string[]){
	int x,game=0;
	for (x=0;x<7;x++){
	//	printf("\n****%d****\n",game);
	//	printf("%d (%c)\n",x,string[x]);
		if(string[x]==34)//"
			return game;
		if(string[x]>=48)//0
			if(string[x]<=57)//9
				game=game*10+(string[x]-48);
	}
}

void printtag(char string[], FILE *exportfp){
	int y, phase = 0;
	for (y=0;y<100;y++){
		if(string[y]==9)//tab
			continue;
		if(string[y]==34){//"
			if(phase==0){
				phase++;
				continue;
			}
			break;
		}
	//	printf("%c",string[y]);
		fprintf(exportfp,"%c",string[y]);
	}
}

int condensevdf(char originalvdf[]){
	FILE *exportfp = fopen("condensed.txt", "w");
	int x = 0, game = 0, tagorgame, tagnumber = 0, uniquegamenumber=0;
	while(1){
		tagorgame = tagorgamef( &originalvdf[x]);
		if(tagorgame == 2){ //tag
			x+=(9+3);//for the tag prefix+for the tag number, assuming ##.
			fprintf(exportfp,"%d\t",game);
			printtag( &originalvdf[x], exportfp);
			fprintf(exportfp,"\n");
			tagnumber++;
		}
		if(tagorgame == 1){ //game
			x+=7;
			game = findgame( &originalvdf[x]);
			x+=3;//??
			uniquegamenumber++;
		}
		x++;
		if(tagorgame == -1)
			break;
	}
	printf("Tags exported to intermediate file: %d\n",tagnumber);
	fprintf(exportfp,'\0');
	fclose(exportfp);
	return uniquegamenumber;
}

void genuniquesortedtaglist(){
	system("cat condensed.txt | grep '(Genre)' | cut  -f 2- | sort -n | uniq -c | sort -nr | awk '{print $2,$3,$4,$5,$6,$7,$8}' | sed 's/^[[:space:]]*//;s/[[:space:]]*$//' > exporttags.txt");
	system("cat condensed.txt | grep '(Platform)' | cut  -f 2- | sort -n | uniq -c | sort -nr | awk '{print $2,$3,$4,$5,$6,$7,$8}' | sed 's/^[[:space:]]*//;s/[[:space:]]*$//' >> exporttags.txt");
	system("cat condensed.txt | grep '(Flags)' | cut  -f 2- | sort -n | uniq -c | sort -nr | awk '{print $2,$3,$4,$5,$6,$7,$8}' | sed 's/^[[:space:]]*//;s/[[:space:]]*$//' >> exporttags.txt");
	system("cat condensed.txt | grep '(Score)' | cut  -f 2- | sort -n | uniq -c | sort -nr | awk '{print $2,$3,$4,$5,$6,$7,$8}' | sed 's/^[[:space:]]*//;s/[[:space:]]*$//' >> exporttags.txt");
	system("cat condensed.txt | grep '(Tags)' | cut  -f 2- | sort -n | uniq -c | sort -nr | awk '{print $2,$3,$4,$5,$6,$7,$8}' | sed 's/^[[:space:]]*//;s/[[:space:]]*$//' >> exporttags.txt");
	system("cat condensed.txt | grep '(Year)' | cut  -f 2- | sort -nr | uniq >> exporttags.txt");
	system("cat condensed.txt | grep -v -e '(Genre)' -e '(Platform)' -e '(Flags)' -e '(Score)' -e '(Tags)' -e '(Year)' | cut  -f 2- | sort -nr | uniq >> exporttags.txt");
}

int finduniquetagnumber(char steamtags[]){
	int x=0,uniquetagnumber=0;
	while(1){
		if(steamtags[x]=='\n')
			uniquetagnumber++;
		else if(steamtags[x]=='\0')
			break;
		x++;
	}
	return uniquetagnumber;
}

char ** importtaglist(int uniquetagnumber, char steamtags[]){
	char **tags = malloc ( sizeof ( char *) * (uniquetagnumber+1));
	if(tags == NULL){
		printf("out of memory 2\n");
		return 0;
	}

	int x=0,y=0,currenttagnumber=1;
	char tagstring[100];
	while(1){
		if(steamtags[x]=='\n'){
			tagstring[y]='\0';
			char *tempstring = malloc(100);
			strcpy (tempstring,tagstring);
			tags[currenttagnumber]=tempstring;
			currenttagnumber++;
			y=0;
		}
		else if(steamtags[x]=='\0'){
			break;
		}
		else{
			tagstring[y]=steamtags[x];
			y++;
		}
		x++;
	}
	return tags;
}

int main (){
	char *originalvdf = readintobuffer("sharedconfig.vdf");
	if (originalvdf==NULL) return 0;
	int uniquegamenumber = condensevdf(originalvdf);
	genuniquesortedtaglist();
	char *steamtags = readintobuffer("exporttags.txt");
	int uniquetagnumber=finduniquetagnumber(steamtags);
	char **tags = importtaglist(uniquetagnumber,steamtags); //List of pointers to all tags

	int x=0,y=0,z,ignorefirst=1;
	FILE *exportfp = fopen("exporttable.csv", "w");
	fprintf(exportfp,"Steam Tags by Game,");
	for (z=1; z<=uniquetagnumber;z++){
		fprintf(exportfp,"%s,",tags[z]);
	}

	char tagstring[100];
	int curgametagstagged[uniquetagnumber+2];

	for (z=0; z<uniquetagnumber+1;z++){
		curgametagstagged[z]=0;
	}

	char *gametagslist = readintobuffer("condensed.txt");
	int readgame=1, currentgame=0, row=0,tagsearch=0;
	while(1){
		if(gametagslist[x]=='\n'){
			tagstring[y]='\0';
			for(tagsearch=1;tagsearch<=uniquetagnumber;tagsearch++){
				if(strcmp(tagstring,tags[tagsearch])==0){
					curgametagstagged[tagsearch]=1;
					break;
				}
			}
			y=0;
			readgame=1;
		}
		else if(gametagslist[x]=='\t'){
			if(curgametagstagged[0]!=currentgame){
				if(ignorefirst==0)
					for (z=0; z<=uniquetagnumber;z++){
						fprintf(exportfp,"%d,",curgametagstagged[z]);
						curgametagstagged[z]=0;
					}
				else
					ignorefirst=0;
				curgametagstagged[0]=currentgame;
				fprintf(exportfp,"\n");
			}
			currentgame=0;
			readgame=0;
		}
		else if(gametagslist[x]=='\0'){
			for (z=0; z<=uniquetagnumber;z++){
				fprintf(exportfp,"%d,",curgametagstagged[z]);
			}
			break;
		}
		else{
			if(readgame==0){//tag
				tagstring[y]=gametagslist[x];
				y++;
			}
			else if(readgame==1){//game
				currentgame=currentgame*10+(gametagslist[x]-48);
			}
		}
		x++;
	}
	fprintf(exportfp,'\0');
	fclose(exportfp);
	return 0;
}
