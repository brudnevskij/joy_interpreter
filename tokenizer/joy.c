#include <stdio.h>
#include <stdlib.h>
#include <math.h>



struct list{
char* word;
struct list* link;
};

struct ulist{
    char type;
    union value{
    char* str;
    struct ulist* link;
    } value;
    struct ulist* link;
};

int freecounter = 0;
int malloccounter = 0;

struct list* swaplist(struct list* list);

void freelist(struct list*);
void freelistnwords(struct list*);
char* substract(char*, char*);
int equal(char*, char*);
char* moveleft(char*);

void myfree(void* a, int linen, const char* caller){
    printf("free've been called from line:%d, caller name :%s\n", linen, caller);
    free(a);
    freecounter++;
}

void* mymalloc(int n, int linen, const char* caller){
    printf("malloc've been called from line:%d, caller name :%s\n", linen, caller);
    malloccounter++;
    return malloc(n);
}

// #define free(n) myfree(n, __LINE__, __FUNCTION__)
// #define malloc(n) mymalloc(n, __LINE__, __FUNCTION__)

int stringlen(char s[]){
    int r = 0;
    while(s[r]!='\0'){
	r++;
}
    return r;
}

char * strcopy(char s[]){
    int len =stringlen(s);
    int i = 0;
    char * ns = (char*) malloc(len*sizeof(char)+1);
    while(s[i]!='\0'){
    ns[i] = s[i];
    i++;
}
    ns[len] = '\0';
    return ns;
}


char* add2(char s1[], char s2[]){
    int str1len = stringlen(s1);
    int str2len = stringlen(s2);
    int reallen;
    int end1 =str1len-1;
    int end2 =str2len-1;
    int end3;
    char* r;
    int sum = 0;
    int flag = 0;

    if(str1len > str2len){
	r = (char*)malloc(str1len*sizeof(char)+2);
	reallen = str1len+2;
	r[reallen-1] = '\0';
	end3 = reallen-2;
	
    }else{
	r = (char*)malloc(str2len*sizeof(char)+2);
	reallen= str2len +2;
	r[reallen-1] = '\0';
	end3 = reallen-2;
    }
	for(int i = reallen-2; i >= 0; i--){
	    if(end1 >= 0)sum+= s1[end1]-48;
	    if(end2 >= 0)sum+= s2[end2]-48;
	    if(flag > 0)sum++;
	    r[i] =(char)sum%10+48;
	    if(sum/10 > 0){
		flag = 1;
	    }else{
		flag = 0;
	    }
	    end1--;
	    end2--;
	    sum = 0;
	}
    if(r[0]<49||r[0]>57){
	for(int i = 0; i< reallen-1; i++){
	    r[i]= r[i+1];
	}
	r = realloc(r, reallen-1);
    }
    free(s1);
    free(s2);
    return r;
}

char* multonechar(char* s, char c, int lens){
    char* ns;
    int cval = c-48;
    ns = (char*)malloc(lens*sizeof(char)+2);
    ns[lens+1] = '\0';
    int carry= 0;
    int prod = 0;
    int end = lens -1;
    for(int i = 0; i < lens+1; i++){
	if(end>=0)prod = cval*(s[end]-48);
	prod+=carry;
	ns[lens-i]= (prod%10)+48;
	carry = prod/10;
	prod=0;
	end--;
    }

    if(ns[0]<49||ns[0]>57){
	for(int j = 0; j<lens+1; j++){
	    ns[j]= ns[j+1];
	}
	ns = realloc(ns, lens);
    }

    return ns;
}


char* multonten(char* s, int n){
    char* ns;
    int len = stringlen(s);
    ns = (char*)malloc(len*sizeof(char)+n+1);
    ns[len+n]='\0';
    for(int i =0; i< len+n; i++){
	if(i<len)ns[i]=s[i];
	if(i>=len)ns[i]='0';
    }
    free(s);
    return ns;
}

char* mult(char s1[], char s2[]){
    char* ns =strcopy("0");
    int lens1 = stringlen(s1);
    int lens2 = stringlen(s2);
    for(int i = 0; i< lens1; i++){
	ns = add2(multonten(multonechar(s2, s1[lens1-1-i], lens2) , i),ns);
    }
    free(s1);
    free(s2);
    while(ns[0] =='0'&&stringlen(ns)>1){
	ns = moveleft(ns);
    }
    return ns;
}


// struct list* addlist(struct list* start, char* value){
//     struct list* nm;
//     struct list* end;
//     while(start!=NULL){
// 	if(start->link==NULL)end = start;
// 	start = start->link;
//     }
//     nm = (struct list*) malloc(sizeof(struct list));
//     nm->word = value;
//     nm->link = NULL;
//     end->link = nm;
//     return start;
// }

struct list* addlist2(struct list* start, char* value){
    struct list* newmember;
    newmember = (struct list*)malloc(sizeof(struct list));
    newmember->word = value;
    newmember->link = start;
    return newmember;
}

void printlist(struct list* list){
    struct list* temp = list;
    while(list!=NULL){
	printf("%s\n", list->word);
	list = list->link;
    }
    freelistnwords(temp);
}

char* copytilspace(char* s, int start){
    int counter = 0;
    int i = start;
    while((s[i] != ' ')&&(s[i] != '\0') ){
	counter++;
	i++;
    }
    char* ns;
    ns = (char*)malloc(counter*sizeof(char)+1);
    ns[counter]='\0';
    i = start;
    for(int j = 0; j < counter; j++){
	ns[j] = s[start+j];
    }
    return ns;
}

struct list* split(char* s){
    int i =-1;
    int start = 0;
    struct list* list = NULL;
    struct list* temp;
    do{
	i++;
	if(s[i]==' '|| s[i] == '\0'){
	    list = addlist2(list, copytilspace(s, start));
	    start = i+1;
	}
	
	}while(s[i] != '\0');
	temp = swaplist(list);
	freelist(list);
    return temp;
}

struct list* swaplist(struct list* list){
    struct list* nl =NULL;
    while(list!=NULL){
	nl = addlist2(nl, list->word);
	list = list->link;
    }
    return nl;
}

void freelist(struct list* list){
    if(list == NULL)return;
    struct list* temp;
    do{
	temp = list;
	list = list->link;
	free(temp);
    }while(list!=NULL);
}

void freelistnwords(struct list* list){
    if(list == NULL)return;
    struct list* temp;
    do{
	temp = list;
	list = list->link;
	free(temp->word);
	free(temp);
    }while(list!=NULL);
}

char* compare(char* s1, char* s2){
    int len1 = stringlen(s1);
    int len2 = stringlen(s2);
    if(len1> len2)return s1;
    if(len2> len1)return s2;
    for(int i = 0; i<len1; i++){
	if(s1[i]>s2[i])return s1;
	if(s2[i]>s1[i])return s2;
    }
    return NULL;
}

char* comparesm(char* s1, char* s2){
    int len1 = stringlen(s1);
    int len2 = stringlen(s2);
    if(len1< len2)return s1;
    if(len2< len1)return s2;
    for(int i = 0; i<len1; i++){
	if(s1[i]>s2[i])return s2;
	if(s2[i]>s1[i])return s1;
    }
    return NULL;
}

char* moveleft(char* s){
    int len = stringlen(s);
    char* ns = (char*) malloc(len*sizeof(char));
    ns[len-1]='\0';
    for(int i = 0; i<len-1; i++){
	ns[i]= s[i+1];
    }
    free(s);
    return ns;
}

char* moveleftnf(char* s){
    int len = stringlen(s);
    char* ns = (char*) malloc(len*sizeof(char));
    ns[len-1]='\0';
    for(int i = 0; i<len-1; i++){
	ns[i]= s[i+1];
    }
    return ns;
}

char* addminus(char* s){
    int len = stringlen(s);
    char* ns = (char*)malloc(sizeof(char)*len+2);
    ns[0] = '-';
    ns[len+1]='\0';
    int i = 1;
    while(ns[i]!='\0'){
	ns[i]=s[i-1];
	i++;
    }
    free(s);
    return ns;
}

int equal(char* s1, char* s2){
    int i = 0;
    int len1 = stringlen(s1);
    int len2 = stringlen(s2);
    if(len1 != len2)return 0;
    while(s1[i] !='\0'&& s2[i]!='\0'){
	if(s1[i]!= s2[i])return 0;
	i++;
    }
    return 1;
}


char* dispatchadd(char* s1, char* s2){
    char* ns;
    int sign1 = 0;
    int sign2 = 0;
    char* comparer;
    if(s1[0]=='-')sign1 =1;
    if(s2[0] == '-')sign2 = 1;
    if(sign1 && sign2){
	ns = add2(moveleft(s1), moveleft(s2));
	ns = addminus(ns);
	return ns;
    }
    else if(sign1 && !sign2){
	comparer = moveleftnf(s1);
	if(compare(comparer,s2) == comparer){
	    ns = substract(moveleft(s1), s2);
	    ns = addminus(ns);
	    free(comparer);
	    return ns;
	}
	free(comparer);
	ns = substract(moveleft(s1), s2);
	return ns;
    }
    else if(!sign1 && sign2){
	    comparer = moveleftnf(s2);
	if(compare(s1,comparer) == comparer){
	    ns = substract(moveleft(s2), s1);
	    ns = addminus(ns);
	    free(comparer);
	    return ns;
	}
	ns = substract(moveleft(s2), s1);
	free(comparer);
	return ns;
    }
    else{
	ns = add2(s1,s2);
	return ns;
    }
}

char* dispatchmult(char* s1, char* s2){
    char* ns;
    int sign1 = 0;
    int sign2 = 0;
    if(s1[0]=='-')sign1 =1;
    if(s2[0] == '-')sign2 = 1;
    if(sign1 && sign2){
	ns =mult(moveleft(s1), moveleft(s2));
	return ns;
    }else if(sign1 && !sign2){
	ns = mult(moveleft(s1), s2);
	if(ns[0]=='0')return ns;
	return addminus(ns);
    }else if(!sign1 && sign2){
	ns = mult(s1, moveleft(s2));
	if(ns[0]=='0')return ns;
	return addminus(ns);
    }else{
	ns = mult(s1,s2);
	return ns;
    }
}

char* dispatchsub(char* s1, char* s2){
    char* ns;
    int sign1 = 0;
    int sign2 = 0;
    char* comparer1 = moveleftnf(s1);
    char* comparer2 = moveleftnf(s2);
    if(s1[0]=='-')sign1 =1;
    if(s2[0] == '-')sign2 = 1;
    if(sign1 && sign2){
	ns =substract(moveleft(s1), moveleft(s2));
	if(compare(comparer1, comparer2) == comparer2){
	    free(comparer1);
	    free(comparer2);
	    return ns;
	}
	if(compare(comparer1, comparer2) == comparer1){
	    free(comparer1);
	    free(comparer2);
	    return addminus(ns);
	}
	free(comparer1);
	free(comparer2);
	return  ns;
    }else if(sign1 && !sign2){
	ns = add2(moveleft(s1), s2);
	free(comparer1);
	free(comparer2);
	return addminus(ns);
    }else if(!sign1 && sign2){
	ns = add2(s1, moveleft(s2));
	free(comparer1);
	free(comparer2);
	return ns;
    }else{
	ns =substract(s1, s2);
	if(compare(comparer1, comparer2) == comparer2){
	    free(comparer1);
	    free(comparer2);
	    return addminus(ns);
	}
	if(compare(comparer1, comparer2) == comparer1){
	    free(comparer1);
	    free(comparer2);
	    return ns;
	}
	free(comparer1);
	free(comparer2);
	return  ns;
    }

}

char* substract(char* s1, char* s2){
    char* bigger = compare(s1,s2);
    char* smaller = comparesm(s1,s2);
    int len1 = stringlen(s1);
    int len2 = stringlen(s2);
    int sent1;
    int sent2;
    int sub1;
    int sub2;
    int carry = 0;
    int i;
    char* result;
    if(bigger == NULL){
	result =(char*)malloc(len1+1);
	result[len1]='\0';
	i = len1-1;
	sent1 = len1-1;
	bigger= s1;
	smaller = s2;
	sent2 = len2-1;
    }else{
	result= (char*)malloc(stringlen(bigger)+1);
	i = stringlen(bigger)-1;
	result[i+1]='\0';
	sent1= stringlen(bigger)-1;
	sent2 = stringlen(smaller)-1;
    }
    while( i >=0){
	if(sent1>=0)sub1 = bigger[sent1]-48;
	if(sent2>=0)sub2= smaller[sent2]-48;
	result[i] = sub1 - sub2 +carry+48;
	sub1 = 0;
	sub2 = 0;
	if(result[i]<48){
	     result[i] = result[i]+10;
	    carry =-1;
	}else{
	    carry =0;
	}
	sent1--;
	sent2--;
	i--;
    }
    while(result[0] =='0'&&stringlen(result)>1){
	result = moveleft(result);
    }
	free(s1);
	free(s2);
    return result;
}

struct list* calculate(struct list* list){
    struct list* stack = NULL;
    struct list* temp1;
    struct list* temp2;
    char* tempch;
    while(list!=NULL){
	if(equal(list->word, "+")){
	    temp1 = stack;
	    temp2 = stack->link;
	    stack = addlist2(stack->link->link ,dispatchadd(stack->word, stack->link->word));
	    free(temp1);
	    free(temp2);
	    free(list->word);
	}
	else if(equal(list->word, "-")){
	    temp1 = stack;
	    temp2 = stack->link;
	    stack = addlist2(stack->link->link ,dispatchsub(stack->word, stack->link->word));
	    free(temp1);
	    free(temp2);
	    free(list->word);
	}
	else if(equal(list->word, "*")){
	    temp1 = stack;
	    temp2 = stack->link;
	    stack = addlist2(stack->link->link ,dispatchmult(stack->word, stack->link->word));
	    free(temp1);
	    free(temp2);
	    free(list->word);
	}else if(equal(list->word, "dup")){
	    stack = addlist2(stack,strcopy(stack->word));
	    free(list->word);
	}else if(equal(list->word, "swap")){
	    tempch = stack->word;
	    stack->word = stack->link->word;
	    stack->link->word = tempch;
	    free(list->word);
	}else if(equal(list->word, "drop")){
	    temp1 = stack;
	    stack = stack->link;
	    free(temp1->word);
	    free(temp1);
	    free(list->word);
	}
	else{
	    stack = addlist2(stack, list->word);
	}
	temp1 = list;
	list = list->link;
	free(temp1);
	
    }

    return stack;
}

/*
	LEGACY CODE

struct list* newsplit(char* s){
    int i = 0;
    int start = 0;
    struct list* nl = NULL;
    struct list* temp;
    while(s[i] != '\0'){
	    if(s[i]!=' '){
		nl = addlist2(nl, copytilspace(s,i));
	    while(s[i]!= ' ' && s[i]!= '\0')i++;
	    }
	i++;
    }
	free(s);
	temp = swaplist(nl);
	freelist(nl);
    return temp;
}


char* copytillend(char* s, int start, int end){
    char* ns;

    int counter = end-start;
    int i = start;
    
    ns = (char*)malloc(counter*sizeof(char)+1);
    ns[counter]='\0';
    for(int j = 0; j < counter; j++){
	ns[j] = s[start+j];
    }
    return ns;
}




struct list* token(char* s){
    struct list* nl = NULL;
    int i = -1;
    int start = 0;
    do{
	i++;
	if(s[i] =='('){
	    
	    if(start!= i){
		nl = addlist2(nl, copytillend(s, start, i));
	    }
	    nl = addlist2(nl, strcopy("("));
	    start=i+1;
        }
	else if(s[i]==')'){
	    
	    if(start!= i){
		nl = addlist2(nl, copytillend(s, start, i));
	    }
	    nl = addlist2(nl, strcopy(")"));
	    start=i+1;
	}else if(s[i]==' '||s[i]=='\0'){
	    
	    if(start!=i){
		nl = addlist2(nl, copytillend(s, start, i));
	    }
	    while(s[i]==' '){
	    start = i+1;
	    i++;
	    }
	    if(s[i]!='\0')i--;
	}
    }while(s[i]!='\0');
    struct list* temp = swaplist(nl);
    freelist(nl);
    return temp;
}


void printulist(struct ulist* ulist, int i){
    int j = 0;
    while(ulist!= NULL){
	if(ulist->type){
	    printulist(ulist->value.link, i+1);
	}else{
	    while(j != i){
		printf(" ");
		    j++;
		}
	    printf("%s\n",ulist->value.str);
	    j=0;
	}
	ulist=ulist->link;
    }
}

struct ulist* addtoulist(char type, char* s, struct ulist* start, struct ulist* link){
    struct ulist* nulist = malloc(sizeof(struct ulist));
    nulist->type=type;
    if(!type){
	nulist->value.str =s;
    }else{
	nulist->value.link =link; 
    }
    nulist->link=start;
    return nulist;
}

struct ulist* swapulist(struct ulist* oldulist){
    struct ulist* nulist = NULL;
    while(oldulist!= NULL){
	nulist = addtoulist(oldulist->type, oldulist->value.str, nulist, oldulist->value.link);
	oldulist = oldulist->link;
    }
    return nulist;
}

struct ulist* genulist(struct list* list){
    struct ulist* nulist = NULL;
    while(list!=NULL){
	if(equal(list->word,"(")){
	    nulist = addtoulist(1, NULL, nulist, genulist(list->link));
	    while(!equal(list->word, ")") ){
		if(equal(list->word, "(")){
		    while(!equal(list->word, ")")){
			list=list->link;
		    }
		}
		list=list->link;
	    }
	}else if(equal(list->word, ")")){
	    return swapulist(nulist);
	}else{
	    nulist = addtoulist(0, list->word, nulist, NULL);
	}
	
	list = list->link;
    }
    nulist = swapulist(nulist);
    return nulist;
}


struct ulist{
    char type;
    union value{
    char* str;
    struct ulist* link;
    } value;
    struct ulist* link;
};



struct ulist* calculateu(struct ulist* list){
    struct ulist* stack = NULL;
	while(list != NULL){
	    if(list->type){
		stack = addtoulist(1,NULL, stack, list->value.link);
	    }
	    else if(!list->type){
		if(equal(list->value.str, "first")){
		    if(stack->type){
			stack->value.link->link = NULL;
			stack = stack->value.link;

		    }
		}else{
		stack = addtoulist(0,list->value.str, stack, NULL);
		}
	    }
	    list = list->link;
	}
    return stack;
}

*/

char* copytillend(char* s, int start, int end){
    char* ns;

    int counter = end-start;
    int i = start;
    
    ns = (char*)malloc(counter*sizeof(char)+1);
    ns[counter]='\0';
    for(int j = 0; j < counter; j++){
	ns[j] = s[start+j];
    }
    return ns;
}




struct list* token(char* s){
    struct list* nl = NULL;
    int i = -1;
    int start = 0;
    do{
	i++;
	if(s[i] =='('){
	    
	    if(start!= i){
		nl = addlist2(nl, copytillend(s, start, i));
	    }
	    nl = addlist2(nl, strcopy("("));
	    start=i+1;
        }
	else if(s[i]==')'){
	    
	    if(start!= i){
		nl = addlist2(nl, copytillend(s, start, i));
	    }
	    nl = addlist2(nl, strcopy(")"));
	    start=i+1;
	}else if(s[i]==' '||s[i]=='\0'){
	    
	    if(start!=i){
		nl = addlist2(nl, copytillend(s, start, i));
	    }
	    while(s[i]==' '){
	    start = i+1;
	    i++;
	    }
	    if(s[i]!='\0')i--;
	}
    }while(s[i]!='\0');
    struct list* temp = swaplist(nl);
    freelist(nl);
    return temp;
}

void printulist(struct ulist* ulist, int i){
    int j = 0;
    while(ulist!= NULL){
	if(ulist->type){
	    printulist(ulist->value.link, i+1);
	    // free(ulist->value.link);
	}else{
	    while(j != i){
		printf(" ");
		    j++;
		}
	    printf("%s\n",ulist->value.str);
	    j=0;
	    free(ulist->value.str);
	}

	struct ulist* tmp = ulist;
	ulist=ulist->link;
	free(tmp);
    }
}
void freeUlist(struct ulist* ulist){
	while(ulist != NULL){
		if(ulist->type) {freeUlist(ulist->value.link);}
		else {free(ulist->value.str);}

        struct ulist* tmp = ulist;
		ulist = ulist->link;
		free(tmp);
	}
}

// void foo(struct ulist* l){
// 	while(l != NULL){
// 		printf("yes\n");
// 		l = l->link;
// 	}
// }
struct list* copyInside(struct list* list){
	int n = 1;
	struct list* newList = NULL;
	while(list != NULL && n != 0){
		if(equal("(", list->word)){
		struct list* tmp;
		n += 1;
		tmp = (struct list*) malloc(sizeof(list));
			tmp->word = strcopy(list->word);
			tmp->link = newList;
			newList = tmp;
		}
		else if(equal(")", list->word)){
		n -= 1;
		if(n != 0){
			struct list* tmp;
			tmp = (struct list*) malloc(sizeof(list));
			tmp->word = strcopy(list->word);
			tmp->link = newList;
			newList = tmp;
		}
		}
		else{
			struct list* tmp;
			tmp = (struct list*) malloc(sizeof(list));
			tmp->word = strcopy(list->word);
			tmp->link = newList;
			newList = tmp;
		}
		list = list->link;

	}
	struct list* tmp = swaplist(newList);
	freelist(newList);
	return tmp;
}

struct list* skipInside(struct list* list){
	int n = 1;
	struct list* tmp;
	do{
		tmp = list;
		free(tmp->word);
		list = list->link;
		if(equal("(", list->word)) n += 1;
		else if(equal(")", list->word)) n -= 1;
	}while(list != NULL && n != 0);
	return list;
}

struct ulist* swapUpperUlist(struct ulist* ulist){
	struct ulist* newUList = NULL;
	struct ulist* tmp;			

	while(ulist != NULL){
		if(ulist->type){
			tmp = (struct ulist*)malloc(sizeof(ulist));
			tmp->link = newUList;
			tmp->value.link = ulist->value.link;
			tmp->type = 1;
			newUList = tmp;
		}else{
			tmp = (struct ulist*)malloc(sizeof(ulist));
			tmp->link = newUList;
			tmp->value.str = ulist->value.str;
			tmp->type = 0;
			newUList = tmp;
		}
		tmp = ulist;
		ulist = ulist->link;
		free(tmp);
	}
	return newUList;
}

struct ulist* generateUlist(struct list* list){
	struct ulist* newList = NULL;
	struct list* start = list;
	while(list != NULL){
		if(equal("(", list->word)){
			struct ulist* sublist;
			sublist = (struct ulist*)malloc(sizeof(struct ulist*));
			sublist->link = newList;
			sublist->value.link = generateUlist(copyInside(list->link));
			list = skipInside(list);
			free(list->word);
			sublist->type = 1;
			newList = sublist;

		}else if(equal(")", list->word)){

		}else{
			struct ulist* tmp;
			tmp = (struct ulist*)malloc(sizeof(struct ulist*));
			tmp->link = newList;
			tmp->value.str = list->word;
			tmp->type = 0;
			newList = tmp;
		}

		list = list->link;
	}

	freelist(start);
	return swapUpperUlist(newList);
}




int main(){
//    printf("%s", copytillend("(a b)", 0, 1 ));
//    printlist(calculate(newsplit(strcopy("22 23 drop"))));
//    printlist(newsplit(strcopy("1    1    +   44  *   -88   +")));
//    printf("\n free: %d , malloc: %d\n", freecounter, malloccounter);
//    printlist(token("3 1 2 3 4 5"));
//    printulist(calculateu(genulist(token("12 1 ( (69) 2 3)"))));
//    printulist(genulist(token("12 11 (10 (100 200)300)")), 0);
//    printlist(token("12 1 ( (69) 2 3)"));
//    printlist(newsplit());

    // struct ulist* nulist0;
    // struct ulist* nulist1;
    // struct ulist* nulist2;
    // struct ulist* nulist3;
    // struct ulist* nulist4;
    // nulist0 = malloc(sizeof(struct ulist));
    // nulist1 = malloc(sizeof(struct ulist));
    // nulist2 = malloc(sizeof(struct ulist));
    // nulist3 = malloc(sizeof(struct ulist));
    // nulist4 = malloc(sizeof(struct ulist));
    // nulist0->type=0;
    // nulist0->value.str = strcopy("nul0");
    // nulist0->link = nulist1;
    // nulist1->type=1;
    // nulist1->value.link=nulist2;
    // nulist1->link = nulist3;
    // nulist2->type = 0;
    // nulist2->value.str=strcopy("nul2");
    // nulist2->link=nulist4;
    // nulist4->type = 0;
    // nulist4->value.str = strcopy("nul4");
    // nulist4->link= NULL;
    // nulist3->type = 0;
    // nulist3->value.str= strcopy("nul3");
    // nulist3->link =NULL;

    printulist(generateUlist(token("1 (2 dup drop swap(12 privet drop)) 3")), 0);
    // generateUlist(token("( 3 )"));

    //freeUlist(nulist0);
//    printulist(swapulist(nulist0));
	// printlist(copyInside(token("12 (13) (14 (13)(13)(13)) ) 25 26 (17)")));
	// printlist(token("12 (13) 14 ) 25 26 (17)"));

    // printulist(generateUlist(token("12 11 10")), 0);
    // foo(generateUlist(token(strcopy("12 11 10"))));
    printf("Malloc calls:%d Free calls:%d\n",malloccounter,freecounter);
    return 0;
}
