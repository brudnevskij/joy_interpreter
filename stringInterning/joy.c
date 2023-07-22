#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

enum stringPoolType{str, cFunc,func};

struct list{
char* word;
struct list* link;
};

struct ulist{
    int type;// 0 - word, 1 - ulist, 2 - StringPoolStruct
    void* value;
    struct ulist* link;
};

struct poolMember{
	int type;
	char* name;
	void* value;
	struct poolMember* link;
};

struct list* swapList(struct list* list);
void freeList(struct list*);
void freelistnwords(struct list*);
char* subtract(char*, char*);
int equal(char*, char*);
char* moveLeft(char*);
struct poolMember* searchStringPool(char*);
struct poolMember* addToStringPool(char*, void*, int);
void printUlistStringPool(struct ulist*);

//global interpreter variabless
struct ulist* stack ;
struct ulist* expression;
struct poolMember* stringPool;
char trace = 0;
int freecounter = 0;
int malloccounter = 0;


//
// <----------------------------Common/System functions---------------------------->
//

void myfree(void* a, int linen, const char* caller){
    // printf("free've been called from line:%d, caller name :%s\n", linen, caller);
    free(a);
    freecounter++;
}

void* mymalloc(int n, int linen, const char* caller){
    // printf("malloc've been called from line:%d, caller name :%s\n", linen, caller);
    malloccounter++;
    return malloc(n);
}

#define free(n) myfree(n, __LINE__, __FUNCTION__)
#define malloc(n) mymalloc(n, __LINE__, __FUNCTION__)

int stringLen(char s[]){
    int r = 0;
    while(s[r]!='\0'){
	r++;
}
    return r;
}

char * strCopy(char s[]){
    int len =stringLen(s);
    int i = 0;
    char * ns = (char*) malloc(len*sizeof(char)+1);
    while(s[i]!='\0'){
    ns[i] = s[i];
    i++;
}
    ns[len] = '\0';
    return ns;
}

int isNumber(char* s){
	int i = 0;
	while(s[i] != '\0'){
		if(s[i] <48 || s[i] > 57)return 0;
		++i;
	}
	return 1;
}

//
//<----------------------------String arithmetics---------------------------->
//




char* addStrings(char s1[], char s2[]){
    int str1len = stringLen(s1);
    int str2len = stringLen(s2);
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
    return r;
}

//multiplies string by one digit-char
char* multByChar(char* s, char c, int lens){
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
		char* tmp = (char*)malloc(sizeof(char)*lens+1);
		char* tmp1 = ns;
	for(int j = 0; j<lens+1; j++){
	    tmp[j]= ns[j+1];
	}
	ns = tmp;
	free(tmp1);
    }

    return ns;
}


char* multByTen(char* s, int n){
    char* ns;
    int len = stringLen(s);
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
    char* ns =strCopy("0");
	char* tmp1;
	char* tmp2;
    int lens1 = stringLen(s1);
    int lens2 = stringLen(s2);
    for(int i = 0; i< lens1; i++){
		tmp1 = multByTen(multByChar(s2, s1[lens1-1-i], lens2) , i);
		tmp2 = ns;
		ns = addStrings(tmp1, tmp2);
		free(tmp1);
		free(tmp2);
    }
    while(ns[0] =='0'&&stringLen(ns)>1){
	ns = moveLeft(ns);
    }
    return ns;
}


//compares two strings returns biggest value
char* compare(char* s1, char* s2){
    int len1 = stringLen(s1);
    int len2 = stringLen(s2);
    if(len1 > len2)return s1;
    if(len2 > len1)return s2;
    for(int i = 0; i<len1; i++){
	if(s1[i]>s2[i])return s1;
	if(s2[i]>s1[i])return s2;
    }
    return NULL;
}

//compares two strings returns smallest value
char* comparesm(char* s1, char* s2){
    int len1 = stringLen(s1);
    int len2 = stringLen(s2);
    if(len1 < len2)return s1;
    if(len2 < len1)return s2;
    for(int i = 0; i<len1; i++){
	if(s1[i]>s2[i])return s2;
	if(s2[i]>s1[i])return s1;
    }
    return NULL;
}

 int bigger(char* s1, char* s2){
 	int len1 = stringLen(s1);
 	int len2 = stringLen(s2);
 	if(len1 > len2)return 1;
    if(len1 < len2)return 0;
    for(int i = 0; i < len1;i++){
    	if(s1[i] > s2[i])return 1;
    	if(s1[i] < s2[i])return 0;
    }

    return 0;
 }

 int smaller(char* s1, char* s2){
 	int len1 = stringLen(s1);
 	int len2 = stringLen(s2);
 	if(len1 < len2)return 1;
    if(len1 > len2)return 0;
    for(int i = 0; i < len1; i++){
    	if(s1[i] < s2[i])return 1;
    	if(s1[i] > s2[i])return 0;
    }
    return 0;
 }

int eq(char* s1, char* s2){
 	int len1 = stringLen(s1);
 	int len2 = stringLen(s2);
 	if(len1 < len2)return 0;
    if(len1 > len2)return 0;
    for(int i = 0; i < len1; i++){
    	if(s1[i] < s2[i])return 0;
    	if(s1[i] > s2[i])return 0;
    }
    return 1;
}

//moves string on a 1 char left
char* moveLeft(char* s){
    int len = stringLen(s);
    char* ns = (char*) malloc(len*sizeof(char));
    ns[len-1]='\0';
    for(int i = 0; i<len-1; i++){
	ns[i]= s[i+1];
    }
    free(s);
    return ns;
}

char* moveLeftNoFree(char* s){
    int len = stringLen(s);
    char* ns = (char*) malloc(len*sizeof(char));
    ns[len-1]='\0';
    for(int i = 0; i<len-1; i++){
	ns[i]= s[i+1];
    }
    return ns;
}

//adds minus to beginning of str
char* addMinus(char* s){
    int len = stringLen(s);
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
    int len1 = stringLen(s1);
    int len2 = stringLen(s2);
    if(len1 != len2)return 0;
    while(s1[i] !='\0'&& s2[i]!='\0'){
	if(s1[i]!= s2[i])return 0;
	i++;
    }
    return 1;
}

//dispatch functions allow arithmetics of interpreter to work with negative numbers

char* dispatchAdd(char* s1, char* s2){
    char* ns;
    int sign1 = 0;
    int sign2 = 0;
    char* comparer;
    if(s1[0]=='-')sign1 =1;
    if(s2[0] == '-')sign2 = 1;
    if(sign1 && sign2){
	ns = addStrings(moveLeft(s1), moveLeft(s2));
	ns = addMinus(ns);
	return ns;
    }
    else if(sign1 && !sign2){
	comparer = moveLeftNoFree(s1);
	if(compare(comparer,s2) == comparer){
	    ns = subtract(moveLeft(s1), s2);
	    ns = addMinus(ns);
	    free(comparer);
	    return ns;
	}
	free(comparer);
	ns = subtract(moveLeft(s1), s2);
	return ns;
    }
    else if(!sign1 && sign2){
	    comparer = moveLeftNoFree(s2);
	if(compare(s1,comparer) == comparer){
	    ns = subtract(moveLeft(s2), s1);
	    ns = addMinus(ns);
	    free(comparer);
	    return ns;
	}
	ns = subtract(moveLeft(s2), s1);
	free(comparer);
	return ns;
    }
    else{
	ns = addStrings(s1,s2);
	return ns;
    }
}

char* dispatchMult(char* s1, char* s2){
    char* ns;
    int sign1 = 0;
    int sign2 = 0;
    if(s1[0]=='-')sign1 =1;
    if(s2[0] == '-')sign2 = 1;
    if(sign1 && sign2){
	ns =mult(moveLeft(s1), moveLeft(s2));
	return ns;
    }else if(sign1 && !sign2){
	ns = mult(moveLeft(s1), s2);
	if(ns[0]=='0')return ns;
	return addMinus(ns);
    }else if(!sign1 && sign2){
	ns = mult(s1, moveLeft(s2));
	if(ns[0]=='0')return ns;
	return addMinus(ns);
    }else{
	ns = mult(s1,s2);
	return ns;
    }
}

char* dispatchSub(char* s1, char* s2){
	char* absoluteValue1;
	char* absoluteValue2;
	char* result;
	int sign1 =0;
	int sign2 =0;
	if(s1[0]=='-'){
		sign1 =1;
		absoluteValue1 = moveLeftNoFree(s1);
	}else absoluteValue1 = strCopy(s1);

	if(s2[0]=='-'){
		sign2 = 1;
		absoluteValue2 = moveLeftNoFree(s2);
	}else absoluteValue2 = strCopy(s2);
	
	if(sign1&&!sign2)
	{
		return addMinus(addStrings(absoluteValue1, absoluteValue2));
	}
	else if(!sign1&&sign2)
	{
		return addStrings(absoluteValue1, absoluteValue2);
	}
	else if(sign1&&sign2)
	{
		if(compare(absoluteValue1, absoluteValue2) == absoluteValue1){
			return addMinus(subtract(absoluteValue1, absoluteValue2));
		}else if(compare(absoluteValue1, absoluteValue2) == absoluteValue2){
			return subtract(absoluteValue1, absoluteValue2);
		}else{
			return subtract(absoluteValue1, absoluteValue2);
		}
	}
	else
	{
		if(compare(absoluteValue1, absoluteValue2) == absoluteValue1){
			return subtract(absoluteValue1, absoluteValue2);
		}else if(compare(absoluteValue1, absoluteValue2) == absoluteValue2){
			return addMinus(subtract(absoluteValue1, absoluteValue2));
		}else{
			return subtract(absoluteValue1, absoluteValue2);
		}
	}
}

int dispatchBigger(char* s1, char* s2){
	int sign1 = 0;
	int sign2 = 0;
	char* absoluteValue1;
	char* absoluteValue2;

	if(s1[0] == '-') sign1 = 1;
	if(s2[0] == '-') sign2 = 1;
	if(sign1&&sign2){
		absoluteValue1 = moveLeftNoFree(s1);
		absoluteValue2 = moveLeftNoFree(s2);
		int r = bigger(absoluteValue1, absoluteValue2);
		free(absoluteValue1);
		free(absoluteValue2);
		return r;
	}else if(sign1&&!sign2){
		return 0;
	}else if(!sign1&&sign2){
		return 1;
	}else{
		return bigger(s1, s2);
	}
}

int dispatchSmaller(char* s1, char* s2){
	int sign1 = 0;
	int sign2 = 0;
	char* absoluteValue1;
	char* absoluteValue2;

	if(s1[0] == '-') sign1 = 1;
	if(s2[0] == '-') sign2 = 1;
	if(sign1&&sign2){
		absoluteValue1 = moveLeftNoFree(s1);
		absoluteValue2 = moveLeftNoFree(s2);
		int r = smaller(absoluteValue1, absoluteValue2);
		free(absoluteValue1);
		free(absoluteValue2);
		return r;
	}else if(sign1&&!sign2){
		return 1;
	}else if(!sign1&&sign2){
		return 0;
	}else{
		return smaller(s1, s2);
	}
}

int dispatchEq(char* s1, char* s2){
	int sign1 = 0;
	int sign2 = 0;
	char* absoluteValue1;
	char* absoluteValue2;

	if(s1[0] == '-') sign1 = 1;
	if(s2[0] == '-') sign2 = 1;
	if(sign1&&sign2){
		absoluteValue1 = moveLeftNoFree(s1);
		absoluteValue2 = moveLeftNoFree(s2);
		int r = eq(absoluteValue1, absoluteValue2);
		free(absoluteValue1);
		free(absoluteValue2);
		return r;
	}else if(sign1&&!sign2){
		return 0;
	}else if(!sign1&&sign2){
		return 0;
	}else{
		return eq(s1, s2);
	}
}

char* subtract(char* s1, char* s2){
    char* bigger = compare(s1,s2);
    char* smaller = comparesm(s1,s2);
    int len1 = stringLen(s1);
    int len2 = stringLen(s2);
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
		result= (char*)malloc(stringLen(bigger)+1);
		i = stringLen(bigger)-1;
		result[i+1]='\0';
		sent1= stringLen(bigger)-1;
		sent2 = stringLen(smaller)-1;
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
    while(result[0] =='0'&&stringLen(result)>1){
	result = moveLeft(result);
    }
	free(s1);
	free(s2);
    return result;
}

//
//<----------------------------List operations---------------------------->
//

char* copyUntilEnd(char* s, int start, int end){
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



struct list* addToList(struct list* start, char* value){
    struct list* newmember;
    newmember = (struct list*)malloc(sizeof(struct list));
    newmember->word = value;
    newmember->link = start;
    return newmember;
}

//inverts list
struct list* swapList(struct list* list){
    struct list* nl = NULL;
	struct list* buffer;
	while(list!=NULL){
		buffer = list->link;
		list->link = nl;
		nl = list;
		list = buffer;
	}
    return nl;
}

//frees list structs, without freeing values
void freeList(struct list* list){
    if(list == NULL)return;
    struct list* temp;
    do{
	temp = list;
	list = list->link;
	free(temp);
    }while(list!=NULL);
}


//splits text by ), (, \n. Trims spaces
struct list* tokenize(char* s){
    struct list* nl = NULL;
    int i = -1;
    int start = 0;
    do{
	i++;
	if(s[i] =='('){
	    
	    if(start!= i){
		nl = addToList(nl, copyUntilEnd(s, start, i));
	    }
	    nl = addToList(nl, strCopy("("));
	    start=i+1;
        }
	else if(s[i]==')'){
	    
	    if(start!= i){
		nl = addToList(nl, copyUntilEnd(s, start, i));
	    }
	    nl = addToList(nl, strCopy(")"));
	    start=i+1;
	}else if(s[i]==' '||s[i]=='\0'){
	    
	    if(start!=i){
		nl = addToList(nl, copyUntilEnd(s, start, i));
	    }
	    while(s[i]==' '){
	    start = i+1;
	    i++;
	    }
	    if(s[i]!='\0')i--;
	}
    }while(s[i]!='\0');
    struct list* temp = swapList(nl);
    // freeList(nl);
    return temp;
}

void printAndFreeUlist(struct ulist* ulist, int i){
    int j = 0;
    while(ulist!= NULL){
	if(ulist->type){
	    printf("(");
	    printAndFreeUlist((struct ulist*)ulist->value, i+1);
	    printf(")");
	}else{
	    printf(" %s ",(char*)ulist->value);
	    j=0;
	    free(ulist->value);
	}

	struct ulist* tmp = ulist;
	ulist=ulist->link;
	free(tmp);
    }
}

void printUlist(struct ulist* ulist){
    while(ulist!= NULL){
	if(ulist->type){
	    printf("(");
	    printUlist((struct ulist*)ulist->value);
	    printf(")");
	}else{
	    printf(" %s ", (char*)ulist->value);
	}

	ulist=ulist->link;
    }
}

void freeUlist(struct ulist* ulist){
	while(ulist != NULL){
		if(ulist->type==1) {freeUlist((struct ulist*)ulist->value);}
		else if(ulist->type==0){free((char *)ulist->value);}

        struct ulist* tmp = ulist;
		ulist = ulist->link;
		free(tmp);
	}
}

//functions skipInside() and copyInside() help to to tokenize raw text
//by navigating over several levels of parantheses
struct list* copyInside(struct list* list){
	int n = 1;
	struct list* newList = NULL;
	while(list != NULL && n != 0){
		if(equal("(", list->word)){
		struct list* tmp;
		n += 1;
		tmp = (struct list*) malloc(sizeof(struct list));
			tmp->word = strCopy(list->word);
			tmp->link = newList;
			newList = tmp;
		}
		else if(equal(")", list->word)){
		n -= 1;
		if(n != 0){
			struct list* tmp;
			tmp = (struct list*) malloc(sizeof(struct list));
			tmp->word = strCopy(list->word);
			tmp->link = newList;
			newList = tmp;
		}
		}
		else{
			struct list* tmp;
			tmp = (struct list*) malloc(sizeof(struct list));
			tmp->word = strCopy(list->word);
			tmp->link = newList;
			newList = tmp;
		}
		list = list->link;

	}
	struct list* tmp = swapList(newList);
	// freeList(newList);
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

struct ulist* swapUpperUlist(struct ulist* list)
{
	struct ulist* newUList = NULL;
	struct ulist* tmp;			
	while(list != NULL){
		tmp = list->link;
		list->link = newUList;
		newUList = list;
		list = tmp;
	}
	return newUList;
}

struct ulist* copyUlist(struct ulist* ulist){
	struct ulist* nUlist = NULL;
	while(ulist != NULL){
		if(ulist->type == 1){
			struct ulist* tmp;
			tmp = (struct ulist*)malloc(sizeof(struct ulist));
			tmp->type = 1;
			tmp->value = copyUlist((struct ulist*)ulist->value);
			tmp->link = nUlist;
			nUlist = tmp;
		}else if(ulist->type == 0){
			struct ulist* tmp;
			tmp = (struct ulist*)malloc(sizeof(struct ulist));
			tmp->type = 0;
			tmp->value = strCopy((char*)ulist->value);
			tmp->link = nUlist;
			nUlist = tmp;
		}else if(ulist->type == 2)
		{
			struct ulist* tmp = (struct ulist*)malloc(sizeof(struct ulist));
			tmp->type = 2;
			tmp->value = ulist->value;
			tmp->link = nUlist;
			nUlist = tmp;
		}
		ulist = ulist->link;
	}
	struct ulist* tmp = swapUpperUlist(nUlist);
	return tmp;
}


struct ulist* generateUlist(struct list* list){
	struct ulist* newList = NULL;
	struct list* start = list;
	while(list != NULL){
		if(equal("(", list->word)){
			struct ulist* sublist;
			sublist = (struct ulist*)malloc(sizeof(struct ulist));
			sublist->link = newList;
			sublist->value = generateUlist(copyInside(list->link));
			list = skipInside(list);
			free(list->word);
			sublist->type = 1;
			newList = sublist;

		}else if(equal(")", list->word)){

		}else{
			struct ulist* tmp;
			tmp = (struct ulist*)malloc(sizeof(struct ulist));
			tmp->link = newList;
			if(isNumber(list->word)){
				tmp->value = list->word;
				tmp->type = 0;
			}else{
				struct poolMember* ref = searchStringPool(list->word);
				tmp->type = 2;
				if(ref == NULL)
				{
					ref = addToStringPool(list->word, NULL, str);
					
				}else{
					free(list->word);
				}
				tmp->value = ref;
			}
			
			
			newList = tmp;
		}

		list = list->link;
	}

	freeList(start);
	return swapUpperUlist(newList);
}

struct ulist* getLastElement(struct ulist* ulist){
	while(ulist->link != NULL){
		ulist = ulist->link;
	}
	return ulist;
}


struct ulist* concatUlist(struct ulist* start, struct ulist* end){
	struct ulist* lastEl = getLastElement(start);
	lastEl->link = end;
	return start;
}

struct ulist* addToUlist(struct ulist* ulist, void* value, int type){
	struct ulist* newMember = (struct ulist*)malloc(sizeof(struct ulist));
	newMember->type = type;
	newMember->value = value;
	newMember->link = ulist;
	return newMember;
}


//
//<----------------------------Interpreter functions---------------------------->
//

//Interpreter initial functions
void addf(){
	struct ulist* tmp1 = expression;
	//sum
	stack = addToUlist(stack, dispatchAdd(stack->link->value, stack->value), 0);
	expression = expression->link;
	free(tmp1);

}


void substractf(){
	struct ulist* tmp1 = stack->link;
	//difference
	stack =addToUlist(stack, dispatchSub(stack->link->value, stack->value), 0);
	tmp1 = expression;
	expression = expression->link;
	free(tmp1);
}

void multiplyf(){
	struct ulist* tmp1 = stack->link;
	//product
	stack = addToUlist(stack, dispatchMult(stack->link->value, stack->value), 0);
	tmp1 = expression;
	expression = expression->link;
	free(tmp1);
}

void moref(){
	struct ulist* tmp1 = stack->link;
	if(dispatchBigger(stack->link->value, stack->value)){
		stack = addToUlist(stack, searchStringPool("true"), 2);
	}else{
		stack = addToUlist(stack, searchStringPool("false"), 2);
	}
	tmp1 = expression;
	expression = expression->link;
	free(tmp1);
}

void lessf(){
	struct ulist* tmp1 = stack->link; 
	if(dispatchSmaller(stack->link->value, stack->value)){
		stack = addToUlist(stack, searchStringPool("true"), 2);
	}else{
		stack = addToUlist(stack, searchStringPool("false"), 2);
	}
	tmp1 = expression;
	expression = expression->link;
	free(tmp1);
}

void equalf(){
	struct ulist* tmp1 = stack->link; // first operand
	if(dispatchEq(stack->link->value, stack->value)){
		stack = addToUlist(stack, searchStringPool("true"), 2);
	}else{
		stack = addToUlist(stack, searchStringPool("false"), 2);
	}
	tmp1 = expression;
	expression = expression->link;
	free(tmp1);

}

void nullf(){
	struct ulist* tmp = stack;

	if(stack->value == NULL)stack = addToUlist(stack, searchStringPool("true"), 2);
	else stack = stack = addToUlist(stack, searchStringPool("false"), 2);

	tmp = expression;
	expression = expression->link;
	free(tmp);
}

void dupf(){
	struct ulist* tmp = stack->link;
	if(stack->type == 1){
		stack = addToUlist(stack, copyUlist((struct ulist*)stack->value), 1);
	}else if(stack->type == 0){
		stack = addToUlist(stack, strCopy(stack->value), 0);
	}else if(stack->type == 2){
		stack = addToUlist(stack, stack->value, 2);
	}

	tmp = expression;
	expression = expression->link;
	free(tmp);
}

void dropf(){
	struct ulist* tmp = stack;
	stack = stack->link;
	if(tmp->type == 1)freeUlist(tmp->value);
	else if(tmp->type == 0)free(tmp->value);
	free(tmp);
	tmp = expression;
	expression = expression->link;
	free(tmp);
}

void swapf(){
	struct ulist* tmp = stack;
	stack = stack->link;
	tmp->link = stack->link;
	stack->link = tmp;

	tmp = expression;
	expression = expression->link;
	free(tmp);
}

void firstf(){
	struct ulist* tmp1 = stack;
	struct ulist* tmp2 = ((struct ulist*)stack->value)->link;
	((struct ulist*)stack->value)->link = stack->link;
	stack = stack->value;

	freeUlist(tmp2);
	free(tmp1);
	tmp1 = expression;
	expression = expression->link;
	free(tmp1);

}

void restf(){
	struct ulist* tmp = stack->value;
	stack->value = ((struct ulist*)stack->value)->link;

	tmp->link = NULL;
	freeUlist(tmp);
	tmp = expression;
	expression = expression->link;
	free(tmp);

}

void consf(){
	struct ulist* tmp = stack->link;
	stack->link = stack->link->link;
	tmp->link = stack->value;
	stack->value = tmp;

	tmp = expression;
	expression = expression->link;
	free(tmp);
}

void unconsf(){
	struct ulist* tmp = ((struct ulist*)stack->value)->link;
	((struct ulist*)stack->value)->link = stack->link;
	stack->link = stack->value;
	stack->value = tmp;

	tmp = expression;
	expression = expression->link;
	free(tmp);
}

void If(){
	struct ulist* tmp1 = stack;
	struct ulist* tmp2 = expression;
	expression = concatUlist((struct ulist*)stack->value, expression->link);
	stack= stack->link;

	free(tmp1);
	free(tmp2);						
}
void dipf(){
	struct ulist* tmp1 = stack->link;
	struct ulist* tmp2 = stack;
	stack = stack->link->link;
	tmp1->link = NULL;
	tmp1 = concatUlist(tmp2->value, tmp1);
	free(tmp2);
	tmp2 = expression;
	expression = concatUlist(tmp1, expression->link);
	free(tmp2);
}
void iff(){
	struct ulist* ifTrue = stack->link;
	struct ulist* ifFalse = stack;
	struct ulist* tmp;

	if(equal("true", ((struct poolMember*)stack->link->link->value)->name)){
		tmp = expression;
		expression = concatUlist(ifTrue->value, expression->link);
		// free(tmp->value);
		free(tmp);// if list cover
		tmp = stack->link->link; // bool value
		stack = stack->link->link->link; // element before bool value
		free(tmp);
		freeUlist(ifFalse->value);
		free(ifFalse);
		free(ifTrue);
	}else if(equal("false", ((struct poolMember*)stack->link->link->value)->name)){
		tmp = expression;
		expression = concatUlist(ifFalse->value, expression->link);
		free(tmp);// "if" list cover
		tmp = stack->link->link; // bool value
		stack = stack->link->link->link; // element before bool value

		free(tmp);
		freeUlist(ifTrue->value);
		free(ifTrue);
		free(ifFalse);
	}
}

void deff(){
	struct ulist* functionName = stack->link;
	struct ulist* functionBody = stack;
	stack = stack->link->link;
	functionName->link = NULL;
	functionBody->link = NULL;
	struct poolMember* ref = (struct poolMember*)functionName->value;

	ref->type = func;
	ref->value = functionBody;
	free(functionName);
	functionName = expression;
	expression = expression->link;
	free(functionName);

}

//
//<----------------------------Calculate functions---------------------------->
//

struct ulist* calculate(char* buffer){
	expression = generateUlist(tokenize(buffer));
	int operation = 0;
	struct ulist* tmp;

	printf("\noperation: %d\n", operation);
	printf("exp: ");
	printUlistStringPool(expression);
	printf("\n");
	printf("stack: ");
	printUlistStringPool(stack);
	printf("\n");
	
	while(expression != NULL){
		
		if(expression->type == 0)
		{
			tmp = expression->link;
			expression->link = stack;
			stack = expression;
			expression = tmp;
		}
		else if(expression->type == 1)
		{
			tmp = expression->link;
			expression->link = stack;
			stack = expression;
			expression = tmp;
		}
		else if (expression->type == 2)
		{
			tmp = expression->link;
			struct poolMember* ref = (struct poolMember*) expression->value;
			if(ref->type == cFunc){
				void (*ptr)() = ref->value;
				(*ptr)();
			}else if(ref->type == str){
				expression->link = stack;
				stack = expression;
				expression = tmp;
			}else if (ref->type == func)
			{
				tmp = expression;
				expression = concatUlist(copyUlist(((struct ulist*)ref->value)->value), expression->link);
				free(tmp);
			}
			
		}
		operation++;
		printf("\noperation: %d\n", operation);
		printf("exp: ");
		printUlistStringPool(expression);
		printf("\n");
		printf("stack: ");
		printUlistStringPool(stack);
		printf("\n");
	}
	return stack;
}

struct ulist* calculateNoTrace(char* buffer){
	expression = generateUlist(tokenize(buffer));
	struct ulist* tmp;

	
	while(expression != NULL){
		
		if(expression->type == 0)
		{
			tmp = expression->link;
			expression->link = stack;
			stack = expression;
			expression = tmp;
		}
		else if(expression->type == 1)
		{
			tmp = expression->link;
			expression->link = stack;
			stack = expression;
			expression = tmp;
		}
		else if (expression->type == 2)
		{
			tmp = expression->link;
			struct poolMember* ref = (struct poolMember*) expression->value;
			if(ref->type == cFunc){
				void (*ptr)() = ref->value;
				(*ptr)();
			}else if(ref->type == str){
				expression->link = stack;
				stack = expression;
				expression = tmp;
			}else if (ref->type == func)
			{
				tmp = expression;
				expression = concatUlist(copyUlist(((struct ulist*)ref->value)->value), expression->link);
				free(tmp);
			}
			
		}
		
	}
	return stack;
}


//
//<----------------------------StringPool operations---------------------------->
//

struct poolMember* searchStringPool(char* target){
	struct poolMember* buffer = stringPool;
	while(buffer!=NULL){
		if(equal(target, buffer->name)) return buffer;
		buffer= buffer->link;	
	}
	return NULL;
}

struct poolMember* addToStringPool(char* name, void* value, int type){
	struct poolMember* newMember = malloc(sizeof(struct poolMember));
	newMember->type = type;
	newMember->name = name;
	newMember->value = value;
	newMember->link = stringPool;
	stringPool = newMember;
	return newMember;
}

void printUlistStringPool(struct ulist* list)
{
	while(list != NULL){
		if(list->type == 0){
			printf("%s ", list->value);
		}else if(list->type == 1){
			printf("(");
			printUlistStringPool(list->value);
			printf(")");
		}else if(list->type == 2){
			printf("%s ", ((struct poolMember*)list->value)->name);
		}
		list = list->link;
	}
}

void freeStringPool(){
	struct poolMember* tmp;
	while(stringPool != NULL){
		if(stringPool->type == str){
			tmp = stringPool->link;
			free(stringPool->name);
			free(stringPool);
			stringPool = tmp;
		}else if(stringPool->type == func){
			tmp = stringPool->link;
			free(stringPool->name);
			freeUlist(stringPool->value);
			free(stringPool);
			stringPool = tmp;
		}else if(stringPool->type == cFunc){
			tmp = stringPool->link;
			free(stringPool->name);
			free(stringPool);
			stringPool = tmp;
		}
	}
}


int main(int argc, char** argv){

// filling string pool with system values

	addToStringPool(strCopy("+"), addf, cFunc);

	addToStringPool(strCopy("-"), substractf, cFunc);

	addToStringPool(strCopy("*"), multiplyf, cFunc);

	addToStringPool(strCopy("<"), lessf, cFunc);

	addToStringPool(strCopy(">"), moref, cFunc);

	addToStringPool(strCopy("=="), equalf, cFunc);

	addToStringPool(strCopy("dup"), dupf, cFunc);

	addToStringPool(strCopy("drop"), dropf, cFunc);
	
	addToStringPool(strCopy("swap"), swapf, cFunc);

	addToStringPool(strCopy("null"), nullf, cFunc);

	addToStringPool(strCopy("first"), firstf, cFunc);

	addToStringPool(strCopy("rest"), restf, cFunc);

	addToStringPool(strCopy("cons"), consf, cFunc);

	addToStringPool(strCopy("uncons"), unconsf, cFunc);

	addToStringPool(strCopy("i"), If, cFunc);

	addToStringPool(strCopy("if"), iff, cFunc);

	addToStringPool(strCopy("dip"), dipf, cFunc);

	addToStringPool(strCopy("def"), deff, cFunc);

	addToStringPool(strCopy("true"), NULL, str);

	addToStringPool(strCopy("false"), NULL, str);





	char * buffer = 0;
	long length;
	FILE * f = fopen (argv[1], "rb");





	if(argc > 2)if(equal(argv[2], "-t")) trace = 1;

	if (f)
	{
  		fseek (f, 0, SEEK_END);
  		length = ftell (f);
  		fseek (f, 0, SEEK_SET);
  		buffer = malloc (length+1);
  	if (buffer){
    	fread (buffer, 1, length, f);
  	}
  	fclose (f);
	}

	if (buffer){
		int i = 0;
		buffer[length] = '\0';
		while(buffer[i]!='\0'){
			if(buffer[i] < 32) buffer[i] = 32;
			if(buffer[i] == '/'&& buffer[i+1] == '/'){
				while(buffer[i+1]!= 10 && buffer[i+1]!= '\0' ){
					buffer[i] = 32;
					i++;
				}
				buffer[i] = 32;
			}			
			i++;
		}
		
		if(trace)printUlistStringPool(calculate(buffer));
		else calculateNoTrace(buffer);
	}
	

	free(buffer);
	freeUlist(stack);
	freeStringPool();
    if(!trace)printf("\nMalloc calls:%d Free calls:%d\n",malloccounter,freecounter);

    return 0;
}
