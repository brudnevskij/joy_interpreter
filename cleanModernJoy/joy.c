#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// expressionType represents alias, for types of multilayer List
enum expressionType{stringpool_member, list, number, open_parenthesis, closed_parenthesis};

// stringPoolType represents alias, for string pool members
enum stringPoolType{sp_string, sp_function, sp_subexpression};

// signType represents +, - in Number struct
enum signType{plus, minus};

// string data type
// len field keeps number of chars + \0
struct string{
	char* value;
	int length;
};
typedef struct string String ;

// number data type
// sign field keeps - or + depends on a number state
// len field keeps number of chars + \0
struct number{
	char* value;
	char sign; // 0: +; 1:-;
	int length;
};
typedef struct number Number;

// homogenous list
struct m_list{
    int type;
    void* value;
    struct m_list* link;
};
typedef struct m_list List;

struct pool_member{
	int type;
	struct string* name;
	void* value;
	struct pool_member* link;
};
typedef struct pool_member Pool_member;

//
//<----------------------------Global interpreter variables---------------------------->
//
// stack of interpreter
List* stack;
// expression for execution
List* expression;
// String pool of interpreter, keeps strings, interpreter functions and user defined functions
Pool_member* string_pool = NULL;
// stack_depth, max depth level of sub-lists in a sourcecode
int stack_depth = 0;
// flag for std output
char trace = 0;
// counter of free() calls
int freecounter = 0;
// counter of malloc() calls
int malloccounter = 0;

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

// Straight link for true and false values
Pool_member* true_link;
Pool_member* false_link;

//
//<----------------------------String operations---------------------------->
//

// Returns len of char*
int len(const char* s)
{
    int l = 0;
    while(s[l] != '\0')++l;
    return l+1;

}

// copies char*, mainly used to set some functions with char* literals
char* str_copy(const char* s, int len)
{
    char* result = (char*) malloc(sizeof(char) * len);
    int i = 0;
    while (i < len)
    {
        result[i] = s[i];
        ++i;
    }
     return result;
}

int8_t str_equal(const char* s1, const char* s2)
{
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0')
    {
        if(s1[i] != s2[i])return 0;
        ++i;
    }
    return 1;
}

// move_str_left moves char* left on 1 byte, mainly
// used on tokenization step to work with negative numbers
char* move_str_left(char* s, int len)
{
    int i = 1;
    while (i < len )
    {
        s[i - 1] = s[i];
        ++i;
    }
    s = realloc(s, sizeof(char) * (len - 1));
    return s;
}
// compare_str compares 2 equal len char* values
// n1 > n2, returns n1
// n2 > n1, returns n2
// n1 == n2, returns NULL
char* compare_str(char* n1, char* n2, int len)
{
    int i = 0;
    while (i < len - 1)
    {
        if(n1[i] > n2[i]) return n1;
        else if(n2[i] > n1[i]) return n2;
        ++i;
    }
    return NULL;
}

// add_str add two absolute char* numbers
// n1_len & n2_len representing char*'s len + \0
char* add_str(char* n1,int n1_len, char* n2, int n2_len)
{
    char* result;
    int result_len;
    // maximum symbols of addition is the biggest length+1
    if(n1_len > n2_len)
    {
        result_len = n1_len + 1;
    }
    else
    {
        result_len = n2_len + 1;
    }
    result = (char*) malloc(sizeof(char) * result_len);
    result[result_len-1] = '\0';
    // flag to check for future allocation
    // if no carry on highest positions
    result[0] = '0';

    // n1_len-2 because, n1_ptr[n1_len-1] == '\0'
    int n1_ptr = n1_len - 2;
    int n2_ptr = n2_len - 2;
    int result_ptr = result_len -2;
    int8_t carry = 0;
    char sum = 0;
    while( n1_ptr >= 0 || n2_ptr >= 0){
        sum = 0;
        if(n1_ptr >= 0)sum += n1[n1_ptr--] - 48;
        if(n2_ptr >= 0)sum += n2[n2_ptr--] - 48;
        if(carry)
        {
            ++sum;
            carry = 0;
        }
        if(sum >9)
        {
            carry = 1;
            sum -= 10;
        }
        result[result_ptr--] = sum + 48;
    }

    // check if highest order sum had carry
    if(carry)
    {
        result[0] = result[0] + 1;
    }
    while(result[0] == '0' && result_len > 2){
        result = move_str_left(result, result_len);
        --result_len;
    }
    return result;
}

// subtract_str add two absolute char* numbers
// n1 - minuend
// n2 - subtrahend
// result = n1 - n2
// minuend must always be longer than subtrahend, otherwise returns NULL
// if operands have same len, minuend must be bigger in value than subtrahend
// n1_len & n2_len representing char*'s len + \0
char* subtract_str(char* n1, int n1_len, char* n2, int n2_len)
{
    if(n2_len > n1_len) return NULL;

    int result_len = n1_len + 1;
    char* result = (char*) malloc(sizeof(char) * result_len);
    result[result_len-1] = '\0';
    result[0] = '0';

    // -2 because n1[n1_len-1] = \0
    int n1_ptr = n1_len - 2;
    int n2_ptr = n2_len - 2;
    int result_ptr = result_len - 2;
    int diff = 0;
    int carry = 0;
    while(result_ptr >= 0)
    {
        diff = 0;
        if(n1_ptr >= 0) diff += n1[n1_ptr--] - 48;
        if(n2_ptr >= 0) diff -= n2[n2_ptr--] - 48;
        diff += carry;
        if (diff < 0)
        {
            diff += 10;
            carry = -1;
        }
        else carry = 0;

        result[result_ptr] = diff + 48;
        --result_ptr;
    }
    while(result[0] == '0' && result_len > 2){
        result = move_str_left(result, result_len);
        --result_len;
    }

    return result;
}

char* multiply_str_by_char(char* n, int len, char m, int ten){
    int result_len = len + 1 + ten;
    char* result = (char*)malloc(sizeof(char));
    result[result_len - 1] = '\0';
    result[0] = '0';

    int n_ptr = len - 2;
    int result_ptr = result_len - 2;
    int carry = 0;
    int product = 1;
    while(ten)
    {
        result[result_ptr--] = '0';
        --ten;
    }
    while(result_ptr >= 0)
    {
        if(n_ptr >= 0)product *= n[n_ptr--] - 48;
        else product *= 0;

        product *= m - 48;
        product += carry;
        if(product > 10)
        {
            carry = product / 10;
            product %= 10;
        }
        else
        {
            carry = 0;
        }
        result[result_ptr--] = product + 48;
        product = 1;
    }
    if (carry){
        result[0] +=1;
    }
    while(result[0] == '0' && result_len > 2){
        result = move_str_left(result, result_len);
        --result_len;
    }
    return  result;
}

// string_from_str allocates memory for new string pool member
// len should be provided with consideration of \0
String* string_from_str(char* s, int len)
{
    String* result = (String*) malloc(sizeof(String));
    result->value = s;
    result->length = len;
    return result;
}

// strings_equal compares 2 String* by bytes
int8_t strings_equal(String* s1, String* s2)
{
    int i = 0;
    while(s1->length > i && s2->length > i)
    {
        if(s1->value[i] != s2->value[i])return 0;
        ++i;
    }
    return 1;
}

void print_string(String* s){
    printf("s:%s",s->value);
}

void print_stringnl(String* s){
    printf("s:%s\n",s->value);
}

void free_string(String* s){
    free(s->value);
    free(s);
}

//
//<----------------------------Number operations---------------------------->
//

uint8_t is_number(char* s)
{
    int i = 0;
    if(s[i] == '-' && s[i+1] == '\0')return 0;
    if((s[i] <48 || s[i] > 57) && s[i] != '-')return 0;
    ++i;

    while (s[i] != '\0'){
        if(s[i] < 48 || s[i] > 57)return 0;
        ++i;
    }
    return 1;
}

Number* number_from_str(char* n, int len, char sign)
{
    Number* result = (Number*) malloc(sizeof(Number));
    result->value = n;
    result->length = len;
    result->sign = sign;
    return result;
}

// number_from_string converts String* to number
// if String->value has negative sign
Number* number_from_string(String* s)
{
    Number* result = (Number*) malloc(sizeof(Number));
    if(s->value[0] == '-')
    {
        result->value = move_str_left(s->value, s->length);
        result->sign = minus;
        result->length = s->length - 1;
    } else{
        result->value = s->value;
        result->sign = plus;
        result->length = s->length;
    }
    return result;
}

Number* add_numbers(Number* n1, Number* n2)
{
    Number* result = (Number*)malloc(sizeof(Number));
    // if signs are identical we perform addition of absolute values
    // and assigning number's sign
    if (n1->sign == n2->sign)
    {
        result->sign = n1->sign;
        result->value = add_str(n1->value, n1->length, n2->value, n2->length);
                result->length = len(result->value);
    }
    else
    {
        // In this case, sign of the biggest number
        // will define sign of the result
        if(n1->length > n2->length)
        {
            result->value = subtract_str(n1->value, n1->length, n2->value, n2->length);
            result->sign = n1->sign;
        }
        else if(n2->length > n1->length)
        {
            result->value = subtract_str(n2->value, n2->length, n1->value, n1->length);
            result->sign = n2->sign;
        }
        else
        {
            // When length are equal, we have to find actual highest number
            char* biggest_value = compare_str(n1->value, n2->value, n1->length);
            // Comparing addresses
            if (biggest_value == n1->value)
            {
                result->value = subtract_str(n1->value, n1->length, n2->value, n2->length);
                result->sign = n1->sign;
            }
            else if(biggest_value == n2->value)
            {
                result->value = subtract_str(n2->value, n2->length, n1->value, n1->length);
                result->sign = n2->sign;
            }
            else
            {
                // values are equal => sum will always be 0
                result->value =str_copy("0",2);
                result->sign = plus;
            }
        }
    }
    result->length = len(result->value);
    // Edge case
    if (result->length == 2 && result->value[0] == '0') result->sign = plus;
    return result;
}

// TODO: consider optimisation
char* multiply_str(char* n1, int n1_len, char* n2, int n2_len){
    char* result = str_copy("0", 2);
    char* product_buffer;
    char* sum_buffer;
    for(int i = 0; i < n1_len-1; i++){
        product_buffer = multiply_str_by_char(n2, n2_len, n1[n1_len-2-i], i);
        sum_buffer = result;
        result = add_str(product_buffer, len(product_buffer), sum_buffer, len(sum_buffer));
        free(product_buffer);
        free(sum_buffer);
    }

    int result_len = len(result);
    while(result[0] == '0' && result_len > 2){
        char* free_buffer = result;
        result = move_str_left(result, result_len);
        --result_len;
    }
    return result;
}

// TODO: consider optimisation
Number* multiply_numbers(Number* n1, Number* n2)
{
    Number* result = (Number*) malloc(sizeof(Number));
    if(n1->sign == n2->sign){
        result->sign = plus;
    }
    else
    {
        result->sign = minus;
    }

    result->value = multiply_str(n1->value, n1->length, n2->value, n2->length);
    result->length = len(result->value);
    return result;
}

// if n1 > n2, returns n1
// if n1 < n2, returns n2
// if n1 == n2, returns NULL
Number* compare_numbers(Number* n1, Number* n2)
{
    if(n1->sign != n2->sign)
    {
        if(n1->sign == plus)return n1;
        return n2;
    }
    else
    {
        if(n1->sign == plus)
        {
            if(n1->length > n2->length)return n1;
            else if(n2->length > n1->length) return n2;

            char* val_cmp = compare_str(n1->value, n2->value, n1->length);
            if(val_cmp == n1->value) return n1;
            if(val_cmp == n2->value)return n2;
            //n1 == n2
            return NULL;
        }
        else
        {
            if(n1->length > n2->length)return n2;
            else if(n2->length > n1->length) return n1;

            char* val_cmp = compare_str(n1->value, n2->value, n1->length);
            if(val_cmp == n1->value) return n2;
            if(val_cmp == n2->value)return n1;
            //n1 == n2
            return NULL;
        }
    }
}

Number* copy_number(Number* n)
{
    Number* result = (Number*) malloc(sizeof(Number));
    result->sign = n->sign;
    result->value = str_copy(n->value, n->length);
    result->length = n->length;
    return result;
}

void print_number(Number* n){
    if(n->sign == minus)printf("n:-%s",n->value);
    else if(n->sign == plus)printf("n:%s",n->value);
}

void print_numbernl(Number* n){
    if(n->sign == minus)printf("n:-%s\n",n->value);
    else if(n->sign == plus)printf("n:%s\n",n->value);
}

void free_number(Number* n)
{
    free(n->value);
    free(n);
}

//
//<----------------------------List operations---------------------------->
//

List* add_to_list(void* value, int type, List* link)
{
    List* result = (List*) malloc(sizeof(List));
    result->value = value;
    result->type = type;
    result->link = link;
    return result;
}

List* reverse_list(List * l)
{
    List* buffer = NULL;
    List* result = NULL;
    while(l != NULL)
    {
        buffer = l->link;
        l->link = result;
        result = l;
        l = buffer;
    }
    return result;
}

List* copy_list(List* l)
{
    List* result = NULL;
    while(l != NULL && l->type != closed_parenthesis)
    {
        if(l->type == number)
        {
            result = add_to_list(copy_number(l->value), l->type, result);
        }
        else if(l->type == stringpool_member)
        {
            result = add_to_list(l->value, l->type, result);
        }
        else if(l->type == list)
        {
            result = add_to_list(copy_list(l->value), l->type, result);
        }
        l = l->link;
    }
    return reverse_list(result);
}

List* concat_list(List* head, List* tail)
{
    List* result = head;

    // Seeking end
    while(head->link != NULL)
    {
        // Eliminating closed parentheses
        if(head->link->type == closed_parenthesis)
        {
            free(head->link);
            head->link = NULL;
            break;
        }
        head = head->link;
    }

    head->link = tail;
    return result;
}

void free_list(List* l){
    List* buffer;
    while(l != NULL)
    {
        buffer = l->link;
        switch (l->type) {
            case list:
                free_list(l->value);
                free(l);
                break;

            case number:
                free_number(l->value);
                free(l);
                break;
            case open_parenthesis:// in case of parentheses, strings freed in the end
                free(l);
                break;
            case closed_parenthesis:
                free(l);
                break;
            case stringpool_member: // in case of SP_MEMBER strings are freed in the end
                free(l);
                break;
        }
        l = buffer;
    }
}

void print_list(List* l)
{
    while(l != NULL)
    {
        switch (l->type) {
            case stringpool_member:
                print_string(
                        ((Pool_member*)l->value)->name
                        );
                printf(" ");
                break;
            case number:
                print_number((Number*)l->value);
                printf(" ");
                break;
            case list:
                printf("(");
                print_list((List*)l->value);
                printf(")");
                break;
            default:
                break;
        }
        l = l->link;
    }
}


void print_list_debug(List* l)
{
    int i = 1;
    while(l != NULL)
    {
        switch (l->type) {
            case stringpool_member:
                printf("member %d\n", i);
                printf("type sp_member\n");
                print_stringnl(
                        ((Pool_member*)l->value)->name
                        );
                break;
            case number:
                printf("member %d\n", i);
                printf("type number\n", i);
                print_numbernl((Number*)l->value);
                break;
            case list:
                printf("member %d\n", i);
                printf("type list\n", i);
                printf("(\n");
                print_list_debug((List*)l->value);
                printf(")\n");
                break;
            default:
                break;
        }
        ++i;
        l = l->link;
    }
}

//
//<----------------------------StringPool operations---------------------------->
//

Pool_member* search_stringpool(String* target)
{
	Pool_member* buffer = string_pool;
	while(buffer!=NULL){
		if(strings_equal(target, buffer->name)) return buffer;
		buffer= buffer->link;	
	}
	return NULL;
}

Pool_member* add_to_stringpool(String* name, void* value, int type)
{
	Pool_member* newMember = malloc(sizeof(Pool_member));
	newMember->type = type;
	newMember->name = name;
	newMember->value = value;
	newMember->link = string_pool;
	string_pool = newMember;
	return newMember;
}

void free_string_pool(){
    Pool_member* buffer;

	while(string_pool != NULL)
	{
        buffer = string_pool->link;
        switch (string_pool->type) {
        case sp_string:
            free_string(string_pool->name);
			free(string_pool);
            break;
        case sp_function:
            free_string(string_pool->name);
            free(string_pool);
            break;

        case sp_subexpression:
        	free_string(string_pool->name);
			free_list(string_pool->value);
			free(string_pool);
            break;
        }
    string_pool = buffer;
	}
}

int is_separator(char c)
{
    // all system char and parentheses are separators
    if(c <= ' ' || c == '(' || c == ')') return 1;
    return 0;
}

String* read_token( const char* buffer, int start)
{
    int end = start;
    String * result;
    // Seeking end of token
    while(!is_separator(buffer[++end]));

    int len = end - start + 1;
    int i = 0;
    char* value = (char*) malloc(sizeof(char) * len);
    while(i < len - 1)
    {
        value[i] = buffer[start];
        ++start;
        ++i;
    }
    value[i] = '\0';

    result = string_from_str(value, len);
    return result;
}

// Debugging functions
void print_token(String* token){
    printf("\n");
    printf("\"%s\", len:%d\n", token->value, token->length);
    int i = 0;
    while (token->value[i] != '\0')
    {
        printf(" %d ", token->value[i++]);
    }
    printf(" %d ", token->value[i++]);
    printf("\n");

}

//
//<----------------------------Interpreter functions---------------------------->
//

void add_f()
{
    // Reusing operation's List* struct as result's place-holder
    List* buffer = expression;
    expression = expression->link;
    buffer->link = stack;
    buffer->type = number;
    buffer->value = add_numbers(stack->value, stack->link->value);
    stack = buffer;
}

void subtract_f()
{
    // Reusing operation's List* struct as result's place-holder
    List* buffer = expression;
    expression = expression->link;
    buffer->link = stack;
    buffer->type = number;

    Number * n1 = stack->link->value;
    Number * n2 = stack->value;
    // n1 - n2, minus operator just reverses sign of subtrahend
    char sign = n2->sign;
    if(sign == plus)n2->sign = minus;
    else n2->sign = plus;

    buffer->value = add_numbers(n1, n2);

    // returning sign to previous state
    sign = n2->sign;
    if(sign == plus)n2->sign = minus;
    else n2->sign = plus;
    stack = buffer;
}

void mult_f()
{
    // Reusing operation's List* struct as result's place-holder
    List* buffer = expression;
    expression = expression->link;
    buffer->link = stack;
    buffer->type = number;
    buffer->value = multiply_numbers(stack->value, stack->link->value);
    stack = buffer;
}

void less_f()
{
    // Reusing operation's List* struct as result's place-holder
    List* buffer = expression;
    expression = expression->link;
    buffer->link = stack;
    buffer->type = stringpool_member;

    Number* n = compare_numbers(stack->value, stack->link->value);
    if(n == stack->value)buffer->value = true_link;
    else buffer->value = false_link;

    stack = buffer;
}

void more_f()
{
    // Reusing operation's List* struct as result's place-holder
    List* buffer = expression;
    expression = expression->link;
    buffer->link = stack;
    buffer->type = stringpool_member;

    Number* n = compare_numbers(stack->value, stack->link->value);
    if(n == stack->link->value)buffer->value = true_link;
    else buffer->value = false_link;

    stack = buffer;
}

void eq_f()
{
    // Reusing operation's List* struct as result's place-holder
    List* buffer = expression;
    expression = expression->link;
    buffer->link = stack;
    buffer->type = stringpool_member;

    Number* n = compare_numbers(stack->value, stack->link->value);
    if(n == NULL)buffer->value = true_link;
    else buffer->value = false_link;

    stack = buffer;
}

void dup_f()
{
    // Reusing operation's List* struct as result's place-holder
    List* buffer = expression;
    expression = expression->link;
    buffer->link = stack;
    buffer->type = stack->type;
    switch (buffer->type) {
        case number:
            buffer->value = copy_number(stack->value);
            break;
        case list:
            buffer->value = copy_list(stack->value);
            break;
        case stringpool_member:
            buffer->value = stack->value;
            break;
    }
    stack = buffer;
}

void drop_f()
{
    // freeing operator
    List* buffer = expression;
    expression = expression->link;
    free(buffer);

    buffer = stack;
    stack = stack->link;
    switch (buffer->type) {
        case number:
            free_number(buffer->value);
            free(buffer);
            break;
        case list:
            free_list(buffer->value);
            free(buffer);
            break;
        case stringpool_member:
            free(buffer);
            break;
    }
}

void swap_f()
{
    // freeing operator
    List* buffer = expression;
    expression = expression->link;
    free(buffer);

    buffer = stack;
    stack = stack->link;
    buffer->link = stack->link;
    stack->link = buffer;
}

void null_f()
{
    List* buffer = expression;
    expression = expression->link;
    buffer->link = stack;
    buffer->type = stringpool_member;
    if(stack->value == NULL || ((List*)stack->value)->type == closed_parenthesis) buffer->value = true_link;
    else buffer->value = false_link;
    stack=buffer;
}

void first_f()
{
    // freeing operator
    List* buffer = expression;
    expression = expression->link;
    free(buffer);

    buffer = stack->value;
    stack->value = buffer->link;
    buffer->link = stack->link;

    free_list(stack->value);
    free(stack);
    stack = buffer;
}

void rest_f()
{
    // freeing operator
    List* buffer = expression;
    expression = expression->link;
    free(buffer);

    buffer = stack->value;
    stack->value = buffer->link;

    // freeing buffer
    switch (buffer->type) {
        case number:
            free_number(buffer->value);
            free(buffer);
            break;
        case list:
            free_list(buffer->value);
            free(buffer);
            break;
        case stringpool_member:
            free(buffer);
            break;
    }
}

void cons_f()
{
    // freeing operator
    List* buffer = expression;
    expression = expression->link;
    free(buffer);

    buffer = stack->link;
    stack->link = buffer->link;

    buffer->link = stack->value;
    stack->value = buffer;
}

void uncons_f()
{
    // freeing operator
    List* buffer = expression;
    expression = expression->link;
    free(buffer);

    buffer = stack->value;
    stack->value = buffer->link;

    buffer->link = stack->link;
    stack->link = buffer;
}

void i_f()
{
    // freeing operator
    List* buffer = expression;
    expression = expression->link;
    free(buffer);

    buffer = stack;
    stack = stack->link;

    expression = concat_list(buffer->value, expression);
    free(buffer);
}

void if_f()
{
    // freeing operator
    List* buffer = expression;
    expression = expression->link;
    free(buffer);

    List* true_condition = stack->link;
    List*  false_condition = stack;
    List* bool = stack->link->link;
    if(bool->value == true_link)
    {
        expression = concat_list(true_condition->value, expression);
        free_list(false_condition->value);
    }
    else if(bool->value == false_link)
    {
        expression = concat_list(false_condition->value, expression);
        free_list(true_condition->value);
    }

    stack = stack->link->link->link;
    free(bool);
    free(true_condition);
    free(false_condition);
}

void dip_f()
{
    // freeing operator
    List* buffer = expression;
    expression = expression->link;
    free(buffer);

    List* first_stack_buffer = stack;
    List* second_stack_buffer = stack->link;
    stack = stack->link->link;

    second_stack_buffer->link = NULL;
    buffer = concat_list(first_stack_buffer->value, second_stack_buffer);
    expression = concat_list(buffer, expression);
    free(first_stack_buffer);
}

void def_f()
{
    // freeing operator
    List* buffer = expression;
    expression = expression->link;
    free(buffer);

    List* function_name = stack->link;
    List* function_body = stack;

    Pool_member* pool_member_function = search_stringpool(((Pool_member*)function_name->value)->name);
    pool_member_function->type = sp_subexpression;
    pool_member_function->value = function_body->value;

    stack = stack->link->link;

    // freeing function's placeholders
    free(function_name);
    free(function_body);
}

// Tokenizes source based on ), (, \s symbols
List* tokenize( char* source)
{
    List * result = NULL;
    int i = 0;

    while(source[i] != '\0')
    {
        if(source[i] == '(')
        {
            result = add_to_list(NULL, open_parenthesis, result);
            ++stack_depth;
        }
        else if (source[i] == ')')
        {
            result = add_to_list(NULL, closed_parenthesis, result);
        }
        else if (source[i] <= ' ')
        {
        
        }
        else if(source[i] == '/' && source[i+1] == '/')
        {
            while (source[i] != '\n' && source[i] != '\0')++i;
        }
        else
        {
            String* token = read_token(source, i);
            // Checking type of token
            void* typed_token;
            int type;
            if(is_number(token->value))
            {
                typed_token = number_from_string(token);
                type = number;

                // freeing only cover of token because value might be reused
                free(token);
            }
            else
            {
                type = stringpool_member;
                Pool_member* member =  search_stringpool(token);
                if(member == NULL)
                {
                    typed_token = add_to_stringpool(token, NULL, sp_string);
                }else{
                    typed_token = member;
                    // freeing whole token, because member is used
                    free_string(token);
                }

            }
            result = add_to_list(typed_token, type, result);

            // setting i to the end position of token
            while (!is_separator(source[++i]));
            --i;
        }

        ++i;
    }
    return reverse_list(result);
}

// parses token_list to AST
// leaves type closed_parenthesis as sublist terminator
List* parse(List* token_list)
{
    List* result = token_list;
    List* buffer = NULL;
    List** sub_list_stack = NULL;
    int stack_ptr = 0;

    if (stack_depth)sub_list_stack = (List**)malloc(sizeof(List) * stack_depth);

    while (token_list != NULL)
    {

        if (token_list->type == open_parenthesis)
        {
            // check point
            sub_list_stack[stack_ptr++] = token_list;
            // reassigning parenthesis struct to be a list struct now
            token_list->type = list;
            // concatenating token_list to a list value
            token_list->value = token_list->link;
            token_list = token_list->value;
        }
        else if(token_list->type == closed_parenthesis)
        {
            sub_list_stack[--stack_ptr]->link = token_list->link;
            token_list->link = NULL;
            token_list = sub_list_stack[stack_ptr];
        }
        else{
            token_list = token_list->link;
        }
    }

    if(stack_depth)free(sub_list_stack);

    return result;
}

List* calculate_with_trace(List* ast)
{
    List* buffer = NULL;
    expression = ast;
    stack = NULL;
    int operation = 0;

    printf("\nExpression to calculate:\n");
    print_list(expression);
    printf("\n");

    while (expression != NULL && expression->type != closed_parenthesis)
    {
        ++operation;
        if(expression->type == stringpool_member)
        {
            Pool_member* member = expression->value;
            if (member->type == sp_function)
            {
                void (*fptr)() = member->value;
                (*fptr)();
            }
            else if(member->type == sp_string)
            {
                // Putting List member on stack
                buffer = expression;
                expression = expression->link;
                buffer->link = stack;
                stack = buffer;
            }
            else if (member->type == sp_subexpression)
            {
                buffer = expression;
                expression = expression->link;
                expression = concat_list(copy_list(member->value), expression);
                free(buffer);
            }
        }
        else
        {
            // Putting List member on stack
            buffer = expression;
            expression = expression->link;
            buffer->link = stack;
            stack = buffer;
        }
        printf("\nOperation %d", operation);
        printf("\nExp:");
        print_list(expression);
        printf("\nStack:");
        print_list(stack);
        printf("\n");
    }
    return stack;
}

List* calculate(List* ast)
{
    List* buffer = NULL;
    expression = ast;
    stack = NULL;
    int operation = 0;

    while (expression != NULL && expression->type != closed_parenthesis)
    {
        ++operation;
        if(expression->type == stringpool_member)
        {
            Pool_member* member = expression->value;
            if (member->type == sp_function)
            {
                void (*fptr)() = member->value;
                (*fptr)();
            }
            else if(member->type == sp_string)
            {
                // Putting List member on stack
                buffer = expression;
                expression = expression->link;
                buffer->link = stack;
                stack = buffer;
            }
            else if (member->type == sp_subexpression)
            {
                buffer = expression;
                expression = expression->link;
                expression = concat_list(copy_list(member->value), expression);
                free(buffer);
            }
        }
        else
        {
            // Putting List member on stack
            buffer = expression;
            expression = expression->link;
            buffer->link = stack;
            stack = buffer;
        }
    }
    return stack;
}

int main(int argc, char *argv[]){
    char* source_file_name;
    if (argc < 2)
    {
        printf("Please provide source code\n");
        return 0;
    }
    source_file_name = argv[1];

    // setting flags
    if (argc > 2)
    {
        for(int i = 2; i < argc; ++i)
        {
            if (str_equal(argv[i], "-t"))trace = 1;

        }
    }

    // getting source code from file
    char* source_code;
    long file_length;
    FILE* f = fopen (source_file_name, "rb");
    if(f)
    {
        fseek (f, 0, SEEK_END);
        file_length = ftell (f);
        fseek (f, 0, SEEK_SET);
        source_code = malloc (file_length+1);
    }
    else
    {
        printf("There is no source code file %s in current dir", source_file_name);
    }

    if(source_code)
    {
        fread (source_code, 1, file_length, f);
        fclose(f);
        source_code[file_length] = '\0';
    }
    else
    {
        printf("Failed on allocating memory");
    }

    // Setting string pool with language base function
    add_to_stringpool(string_from_str(str_copy("+", 2), 2), add_f, sp_function);
    add_to_stringpool(string_from_str(str_copy("-", 2), 2), subtract_f, sp_function);
    add_to_stringpool(string_from_str(str_copy("*", 2), 2), mult_f, sp_function);
    add_to_stringpool(string_from_str(str_copy("<", 2), 2), less_f, sp_function);
    add_to_stringpool(string_from_str(str_copy(">", 2), 2), more_f, sp_function);
    add_to_stringpool(string_from_str(str_copy("==", 3), 3), eq_f, sp_function);
    add_to_stringpool(string_from_str(str_copy("dup", 4), 4), dup_f, sp_function);
    add_to_stringpool(string_from_str(str_copy("drop", 5), 5), drop_f, sp_function);
    add_to_stringpool(string_from_str(str_copy("swap", 5), 5), swap_f, sp_function);
    add_to_stringpool(string_from_str(str_copy("null", 5), 5), null_f, sp_function);
    add_to_stringpool(string_from_str(str_copy("first", 6), 6), first_f, sp_function);
    add_to_stringpool(string_from_str(str_copy("rest", 5), 5), rest_f, sp_function);
    add_to_stringpool(string_from_str(str_copy("cons", 5), 5), cons_f, sp_function);
    add_to_stringpool(string_from_str(str_copy("uncons", 7), 7), uncons_f, sp_function);
    add_to_stringpool(string_from_str(str_copy("i", 2), 2), i_f, sp_function);
    add_to_stringpool(string_from_str(str_copy("if", 3), 3), if_f, sp_function);
    add_to_stringpool(string_from_str(str_copy("dip", 4), 4), dip_f, sp_function);
    add_to_stringpool(string_from_str(str_copy("def",4), 4), def_f, sp_function);
    true_link = add_to_stringpool(string_from_str(str_copy("true", 5), 5), NULL, sp_string);
    false_link = add_to_stringpool(string_from_str(str_copy("false", 6), 6), NULL, sp_string);

    List* l = tokenize(source_code);
    List* ast = parse(l);
    free(source_code);

    if (trace)
    {
        calculate_with_trace(ast);
    }
    else
    {
        calculate(ast);
    }
    // freeing elements
    free_list(stack);
    free_string_pool();
    if (trace)
    {
        printf("\nmallocs: %d\n", malloccounter);
        printf("free: %d\n", freecounter);
    }

    return 0;
}