#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "template.h"

#define MAXINDEX 1024

// Simplec files will require a main function followed by an integer return type

typedef struct symbol_table
{
	// Each node holds a single character.
	char var_name[MAXINDEX];
	// char *address;
	char address[MAXINDEX];
	// Pointer to next node in linked list.
	int param_len;
	struct symbol_table *next;
} symbol_table;

char *read_prints(FILE *fp, int *counter, char *print_str, symbol_table *table);
char *read_input(FILE *fp, int *counter, symbol_table *table);
symbol_table *create_node(char *var_name, char *address, int len);
symbol_table *destroy_table(symbol_table *table);
void insert_table(char *var, char *address, int signature, symbol_table *table);
char *search_table(FILE *fp, char *var, int *counter, symbol_table *table);
void insert_table_func(char *var_name, int len, symbol_table *table);
char *search_table_func(FILE *fp, char *function, int *counter, symbol_table *table);
void leave(FILE *fp, int *counter, symbol_table *table);

int function(FILE *fp, int *counter, char *print_str, symbol_table *table);
void function_reader(FILE *fp, int *counter, char *print_str, symbol_table *table, symbol_table *local);
void if_stmt(FILE *fp, int *counter, char *print_str, symbol_table *table, symbol_table *main_table);
void while_stmt(FILE *fp, int *counter,char *print_str, symbol_table *table, symbol_table *main_table);
void stmt(FILE *fp, int *counter, char *print_str, symbol_table *table, symbol_table *main_table);
char *function_caller(FILE *fp, int *counter, char *print_str, symbol_table *table, symbol_table *main_table);

char *expr(FILE *fp, int *counter, symbol_table *table, symbol_table *main_table);
char *add_expr(FILE *fp, int *counter, symbol_table *table, symbol_table *main_table);
char *expr_prime(FILE *fp, char *left, int *counter, symbol_table *table, symbol_table *main_table);

char *term(FILE *fp, int *counter, symbol_table *table, symbol_table *main_table);
char *term_prime(FILE *fp, char *left, int *counter, symbol_table *table, symbol_table *main_table);
char *factor(FILE *fp, int *counter, symbol_table *table, symbol_table *main_table);

char *gt_lt_expr(FILE *fp, int *counter, symbol_table *table, symbol_table *main_table);
char *gt_lt_expr_prime(FILE *fp, char *left, int *counter, symbol_table *table, symbol_table *main_table);

char *eq_nq_expr(FILE *fp, int *counter, symbol_table *table, symbol_table *main_table);
char *eq_nq_expr_prime(FILE *fp, char *left, int *counter, symbol_table *table, symbol_table *main_table);

char *and_expr(FILE *fp, int *counter, symbol_table *table, symbol_table *main_table);
char *and_expr_prime(FILE *fp, char *left, int *counter, symbol_table *table, symbol_table *main_table);

char *or_expr_prime(FILE *fp, char *left, int *counter, symbol_table *table, symbol_table *main_table);

char *num_reader(FILE *fp, int *counter, symbol_table *table);
char *var_reader(FILE *fp, int *counter, symbol_table *table, symbol_table *main_table);
char *temp_func(int *counter, symbol_table *table);
char *label_func(int *counter, symbol_table *table);
void read_spaces (FILE *fp);

void main(int argc, char **argv)
{
  FILE *fp;

  fp = fopen(argv[1],"r");
  if (fp == NULL)
  return;
	printf(PROJ4_PROLOGUE);

	symbol_table *table = create_node("invalid", "invalid", 0);
	symbol_table *temp = NULL;
	char single_num[MAXINDEX];
	char *num_temp_ref;
	char var_address[MAXINDEX];
  char c;
  int temp_count;
	int count_equal;
	char *print_str;
  int *counter = calloc(8, sizeof(int));
	// counter[0] = iterates the addresses eg %t1 -> %t2
	// counter[1] = 1 for print output and 0 to not print output
	// counter[2] = 1 for a read and 0 for non
	// counter[3] = 1 for valid print, 0 for not valid
	// counter[4] = iterates the numbers for labels
	// counter[5] = checks for if were expectin an else statement
	// counter[6] = checks if we read a function(1) or regular var(0)
	// counter[7] = counter for how deep the if statements are in

  while (1)
  {
		count_equal = 0;
		counter[1] = 1;
		counter[2] = 0; // checks for read instruction
    counter[3] = 1;

		temp_count = counter[0];
		char *str = read_prints(fp, counter, print_str, table);
		//printf("MAIN====================================str is: ===%s===\n", str);
		if (strcmp(str, "") == 0) // we have read an EOF
			break;
		else if (strcmp(str, ";") == 0)
			continue;
    if (!counter[3])
    {
      counter[3] = 0;
      c = fgetc(fp);
      while(c != ';' && c != EOF)
      {
        c = fgetc(fp);
      }
      // c is at eof or semicolon at this point
      //continue;
      ungetc(c,fp);
    }
		if (strcmp(str, "int") == 0)
		{
			// if the function was successfull we return 1
			if(function(fp, counter, str, table))
				continue;
			// if its not successfull and just a var declarations
			// just go forward regularly
		}
		if (strcmp(str, "if") == 0)
		{
			if_stmt(fp, counter, str, table, NULL);
			// the closing bracket will be noticed inside if
			// statements own stmt function(similar to this while)
			// but without the variable declarations in this main
			// function
			continue;
		}
		else if (strcmp(str, "while") == 0)
		{
			while_stmt(fp, counter, str, table, NULL);
			continue;
		}

    read_spaces(fp);

		if (counter[3] == 0)
    {
      c = fgetc(fp);

      if (c == EOF)
        break;
      else if (c == ';')
        continue;
    }

		c = fgetc(fp);
		if (c == '=')
		{
			count_equal = 1;
			c = fgetc(fp);
		}

		ungetc(c,fp);

		num_temp_ref = expr(fp, counter, table, NULL);

		if (num_temp_ref != NULL)
		{
			strcpy(single_num, num_temp_ref);
		}

		if (counter[1] == 1) // if were printing
		{
			if (temp_count == counter[0])
	    {
	      printf("    %s = add nsw i32 0, %s\n",temp_func(counter, table), single_num);
	    }
	    printf("    call void @print_integer(i32 %%t%d)\n", counter[0]-1);
		}

		if (count_equal || counter[2]) // if were assigning/reading
		{
			for (temp = table; temp != NULL; temp = temp->next)
			{
				// print_str is the var name extracted in the print function
				if (strcmp(temp->var_name, str) == 0)
				{
					// found the var were looking for so were gonna extract its address
					strcpy(var_address, temp->address);
					break;
				}
			}
			if (temp_count == counter[0])
	      printf("    store i32 %s, i32* %s, align 4\n",single_num, var_address);
			else
	    	printf("    store i32 %%t%d, i32* %s, align 4\n", counter[0]-1, var_address);
		}

    c = fgetc(fp);
    if (c == EOF)
      break;
    else if (c == ';')
		{
			c = fgetc(fp);
			if (c == EOF)
			break;
			c = fgetc(fp);
			if (c == EOF)
			break;

			c = ungetc(c,fp);
			c = ungetc(c,fp);
			c = fgetc(fp);
			continue;
		}
  }

	destroy_table(table);
  free(counter);
  fclose(fp);
  return;
}

int function(FILE *fp, int *counter, char *print_str, symbol_table *table)
{
	char c;
  char reader[MAXINDEX];
  int i = 0;
	symbol_table *local_table = create_node("invalid", "invalid", 0);
	symbol_table *temp_A;
	symbol_table *temp_B;
	char *str;
	int int_checker = 1;
	counter[0] = 0;
	read_spaces(fp);

	while (i + 1 < MAXINDEX && ((c = fgetc(fp)) != ' ' && c != EOF && c != ';' && c != '\n'))
  {
		//is it a function?
		if(c == '(')
		{
			int_checker = 0;
			reader[i] = '\0'; // reader temporarily holds a funct name
			str = reader; // current function name
			printf("define i32 @%s(", str);
			function_reader(fp, counter, str, table, local_table);
			break;
		}
    reader[i] = c;
    i++;
  }
	reader[i] = '\0';
	str = reader;
 	if (int_checker)
	{
		// UNGETC BACKWARDS TIME!!!
		// we found an int declarations
		// so go backwards
		int len = strlen(str);
		int last = len;
		int j = 0;
		ungetc(c,fp); // the while from above stops at a semicolon

		for (j = 0; j < len ; j++) //want to fget then keep ungettin
		{
			c = str[last-1]; //we access str and give it to ungetc
			last--;
			ungetc(c,fp); // keep un getting before it access first letter of str
		}

		ungetc(' ',fp);
		c = fgetc(fp); //should be at a space when leaving the function
		return 0;
	}
	return 1;
}

void function_reader(FILE *fp, int *counter, char *print_str, symbol_table *table, symbol_table *local)
{
	symbol_table *temp_A;
	symbol_table *temp_B;
	char c;
	char reader[MAXINDEX];
	char *str;
	int i = 0;
	int j;
	int param_count = 0;
	int var_count = 0;
	read_spaces(fp);
	while (i + 1 < MAXINDEX && ((c = fgetc(fp)) != ')'))
	{
		if (c == ' ') // next should be the integer name
		{
			param_count++;
			read_spaces(fp);
			c = fgetc(fp);
			for (j = 0; c != ',' && c != ')'; j++)
			{
				reader[j] = c;
				c = fgetc(fp);
			}
			reader[j] = '\0'; // var signature string

			str = reader; // modify local list
			insert_table(str, temp_func(counter, table), 0, local);
			if (c == ')')
			{
				printf("i32) \n{\n");
				break;
			}
			else
				printf("i32, ");
		}
		// we should be at a comma soo
		if (c == ',')
		read_spaces(fp);
	}

	//search and insert function
	search_table_func(fp, print_str, counter, table);
	insert_table_func(print_str, param_count, table);
	if (param_count > 0)
	{
		// print statements for the variable parameters
		for (temp_B = local; temp_B != NULL; temp_B = temp_B->next)
		{
			if(strcmp(temp_B->var_name, "invalid") != 0)
			{
				printf("    %s = alloca i32, align 4\n", temp_B->address);
				printf("    store i32 %%%d, i32* %s, align 4\n", var_count, temp_B->address );
				var_count++;
			}
		}
	}
	else
		printf(") \n{\n"); // empty parameter function

	// function reader passes the local as the regular table and table as main table
	stmt(fp, counter, print_str, local, table);
	printf("\n}\n");
	destroy_table(local);
	return;
}
void if_stmt(FILE *fp, int *counter, char *print_str, symbol_table *table, symbol_table *main_table)
{
  char cond[MAXINDEX];
	char ifbody[MAXINDEX];
	char elsebody[MAXINDEX];
	char end[MAXINDEX];
  read_spaces(fp);
  char c = fgetc(fp);

  if (c == '(' )
  {
		counter[5] = 1; //inside if statement
		counter[7]++;
    strcpy(cond, expr(fp, counter, table, main_table));
    read_spaces(fp);
    c = fgetc(fp); //consume rparen
		strcpy(ifbody, label_func(counter, table));
		strcpy(elsebody, label_func(counter, table));
		strcpy(end, label_func(counter, table));

		// the double percent so we print"%" plus the string next to it
		printf("    br i1 %s, label %%%s, label %%%s\n", cond, ifbody, elsebody);
		printf("%s:\n", ifbody); //emit
		stmt(fp, counter, print_str, table, main_table);

		printf("    br label %%%s\n", end); // call
		// consume else
		printf("%s:\n", elsebody); //emit
		counter[5] = 2; // inside else statement

		stmt(fp, counter, print_str, table, main_table);

		printf("    br label %%%s\n", end); // call
		printf("%s:\n", end); //emit
		counter[7]--;
		counter[5] = 0; //leaving the if statement
  }

	ungetc(c,fp);
	return;
}

void while_stmt(FILE *fp, int *counter,char *print_str, symbol_table *table, symbol_table *main_table)
{
	char cond[MAXINDEX];
	char head[MAXINDEX];
	char whilebody[MAXINDEX];
	char end[MAXINDEX];
  read_spaces(fp);
  char c = fgetc(fp);

  if (c == '(' )
  {
		strcpy(head, label_func(counter, table));
		printf("    br label %%%s\n", head); // call
		printf("%s:\n", head); //emit
    strcpy(cond, expr(fp, counter, table, main_table));
    read_spaces(fp);
    c = fgetc(fp); //consume rparen

		strcpy(whilebody, label_func(counter, table));
		strcpy(end, label_func(counter, table));

		// the double percent so we print"%" plus the string next to it
		printf("    br i1 %s, label %%%s, label %%%s\n", cond, whilebody, end);
		printf("%s:\n", whilebody); //emit

		stmt(fp, counter, print_str, table, main_table);
		printf("    br label %%%s\n", head); // call
		printf("%s:\n", end); //emit
		//return;
  }

	ungetc(c,fp);
	return;
}

void stmt(FILE *fp, int *counter, char *print_str, symbol_table *table, symbol_table *main_table)
{

	symbol_table *temp = NULL;
	char *num_temp_ref;
	char single_num[MAXINDEX];
	char var_address[MAXINDEX];
	char c;
	int temp_count;
	int count_equal;
	int brackets_valid = 0; // assume there not valid

	while (1)
	{
		//was counter[5] == 0
		if(counter[7] == 1 && counter[5] == 0)
		{
			return;
		}
		count_equal = 0;
		counter[1] = 1;
		counter[2] = 0; // checks for read instruction
		counter[3] = 1;

		temp_count = counter[0];

		char *str = read_prints(fp, counter, print_str, table);

		//printf("stmt====================================str is: ===%s===\n", str);
		if (strcmp(str, "") == 0) // we have read an EOF
			break;
		else if (strcmp(str, ";") == 0)
		{
			if (brackets_valid == 0 && counter[5] == 1)
			{
				return;
			}
			if(brackets_valid == 0 && counter[5] == 2)
			{
				return;
			}
			continue;
		}


		if (!counter[3])
		{
			counter[3] = 0;
			c = fgetc(fp);
			while(c != ';' && c != EOF)
			{
				c = fgetc(fp);
			}
			// c is at eof or semicolon at this point
			//continue;
			ungetc(c,fp);
		}

		//printf("print reads ===%s===\n", str);
		if (strcmp(str, "else") == 0)
		{
			continue;
		}
		else if (strcmp(str, "else") != 0 && counter[5] == 3)
		{

			int len = strlen(str);
			int last = len;
			int i = 0;
			c = fgetc(fp); // were at a space and want access letter len
			ungetc(c,fp);

			for (i = 0; i < len ; i++) // want to fget then keep ungettin
			{
				c = str[last-1]; // we access str and give it to ungetc
				last--;
				ungetc(c,fp); // keep un getting before it access first letter of str
			}
			// NEXT TIME WE FGETC we should start at the beginning of line
			//checks how fgetc is functioning
			ungetc(' ',fp);
			c = fgetc(fp);

			return;
		}
		else if (strcmp(str, "if") == 0)
		{
			if_stmt(fp, counter, str, table, main_table);
			// the closing bracket will be noticed inside if
			// statements own stmt function(similar to this while)
			// but without the variable declarations in this main
			// function

			if (counter[7] == 0)
			continue; // usually we continue
			else
			return;
		}
		else if (strcmp(str, "while") == 0)
		{
			while_stmt(fp, counter, str, table, main_table);
			continue;
		}
		if (strcmp(str, "}") == 0)
		{
			//if closing bracket then we get out of entire functions
			return;
		}
		if (strcmp(str, "{") == 0)
		{
			if(counter[5] == 1) // if were inside an if statement
				brackets_valid = 1;
			if(counter[5] == 2)
				brackets_valid = 1;
			// if closing bracket then we get out of entire functions
			continue;
		}

		read_spaces(fp);

		if (counter[3] == 0)
		{
			c = fgetc(fp);

			if (c == EOF)
				break;
			else if (c == ';')
				continue;
		}

		c = fgetc(fp);
		if (c == '=')
		{
			count_equal = 1;
			c = fgetc(fp);
		}
		ungetc(c,fp);

		if ( c == '{' && strcmp(str, "else") == 0)
		{
			continue;
		}
		else if(strcmp(str, "else") == 0 && c != '{' )
		{
			// utilize the counter change in the else above
			if (counter[5] == 0) // were expectin an else
			{
				counter[5] = 1;
				continue;
			}
			return;
		}

		num_temp_ref = expr(fp, counter, table, main_table);

		if (num_temp_ref != NULL)
		{
			strcpy(single_num, num_temp_ref);
		}

		if (strcmp(str, "return") == 0)
		{
			if (temp_count == counter[0])
			{
				printf("    %s = add nsw i32 0, %s\n",temp_func(counter, table), single_num);
			}
			printf("    ret i32 %%t%d\n", counter[0]-1);
			c = fgetc(fp); // should read a semicolon
			continue; // meant to read the closing bracket to return
		}

		if (counter[1] == 1) // if were printing
		{
			if (temp_count == counter[0])
			{
				printf("    %s = add nsw i32 0, %s\n",temp_func(counter, table), single_num);
			}
			printf("    call void @print_integer(i32 %%t%d)\n", counter[0]-1);
		}

		if (count_equal || counter[2]) // if were assigning/reading
		{
			for (temp = table; temp != NULL; temp = temp->next)
			{
				// print_str is the var name extracted in the print function
				if (strcmp(temp->var_name, str) == 0)
				{
					// found the var were looking for so were gonna extract its address
					strcpy(var_address, temp->address);
					break;
				}
			}
			if (temp_count == counter[0])
				printf("    store i32 %s, i32* %s, align 4\n", single_num, var_address);
			else
				printf("    store i32 %%t%d, i32* %s, align 4\n", counter[0]-1, var_address);
		}

		c = fgetc(fp);
		if (counter[5] == 1 && brackets_valid == 0)
		{
			// if were inside an if statement with no brackets
			// then it should return after one complete line
			return;
		}

		if (c == EOF)
			break;
		if (c == ';')
		{
			c = fgetc(fp);
			if (c == EOF)
			break;
			c = fgetc(fp);
			if (c == EOF)
			break;

			c = ungetc(c,fp);
			c = ungetc(c,fp);
			c = fgetc(fp);
			continue;
		}
	}
	return;
}

char *function_caller(FILE *fp, int *counter, char *print_str, symbol_table *table, symbol_table *main_table)
{
	char c ;
	char output_address[MAXINDEX];
	char *address;
  int i = 0;
	int param_count = 0;
	int count = 1;
	symbol_table *temp;
	symbol_table *temp_table = create_node("invalid", "invalid", 0);
	c = fgetc(fp); //this should read an LPAREN

	while (c != ')')
	{
		if (c == ',')
		{
			read_spaces(fp);
		}

		address = expr(fp, counter, table, main_table);
		insert_table("parameters", address, 0, temp_table);
		param_count++;
		c = fgetc(fp);
	}
	address = temp_func(counter, table); // the last address for the call
	printf("    %s = call i32 @%s(", address, print_str);
	// search the parameters and emit them accordingly
	for (temp = temp_table; temp != NULL; temp = temp->next)
	{
		if (strcmp(temp->var_name, "parameters") == 0)
		{
			// if were at last parameter than make the final RPAREN
			if (count == param_count)
			{
				printf("i32 %s)\n", temp->address);
				break;
			}
			else
				printf("i32 %s, ", temp->address);
			count++;
		}
	}

	destroy_table(temp_table);
	return address;
}
char *read_input(FILE *fp, int *counter, symbol_table *table)
{
	char c;
	char input[MAXINDEX];
	int i = 0;
	char *str;
	symbol_table *temp;
  c = fgetc(fp);

  while(!isalpha(c) && c != ';' && c != EOF)
	{
		c = fgetc(fp);
	}
	ungetc(c,fp);
	while (i + 1 < MAXINDEX && ((c = fgetc(fp)) != ' ' && c != EOF && c != ';' && c != '\n'))
	{
		input[i] = c;
		i++;
	}
	input[i] = '\0';
	str = input;

	for (temp = table; temp != NULL; temp = temp->next)
	{
		if (strcmp(temp->var_name, input) == 0)
		{
			// variable is declared so we break out
			printf("    %s = call i32 @read_integer()\n", temp_func(counter, table));
			counter[2] = 1;
			break;
		}
		if (temp->next == NULL)
		{
			// if our next is null then our last valid node
			// does not have a the var name were looking for
			// eg z = 5 (but we never declared z)
			fprintf(stderr, "error: use of undeclared variable %s\n", input);
			leave(fp, counter, table);
		}
	}
	// might have to ungetc(c,fp);
	return str;
}

char *read_prints(FILE *fp, int *counter, char *print_str, symbol_table *table)
{
  char c;
  char print[MAXINDEX];
  int i = 0;
	symbol_table *temp;

	c = fgetc(fp);
	while(!isalpha(c) && c != ';' && c != EOF && c != '}' && c != '{')
  {
    c = fgetc(fp);
  }
  ungetc(c,fp);

  while (i + 1 < MAXINDEX && ((c = fgetc(fp)) != ' ' && c != EOF && c != ';' && c != '\n'))
  {
		if(c == '=')
			break;
    print[i] = c;
    i++;
  }
  print[i] = '\0';
	print_str = print;

	// check for DECLARED VARIABLES
	if (strcmp(print, "") == 0)
	{
		if (c == ';')
		{
			print_str = ";";
		}
		else
		{
			print_str = ""; // we will assume its an EOF
		}

		//printf("================================empty string ===%c===\n", c);
		return print_str; // we have read an EOF
	}
	else if (strcmp(print, "}") == 0)
	{
		// ending curly brace
		// just enter and return at end of function
	}
	else if (strcmp(print, "{") == 0)
	{
		// opening curly brace
		// just enter and return at end of function
	}
	else if (strcmp(print, "if") == 0)
	{
		// just enter and return at end of function
	}
	else if (strcmp(print, "while") == 0)
	{
		// just enter and return at end of function
	}
	else if (strcmp(print, "else") == 0)
	{
		// just enter and return at end of function
	}
	else if (strcmp(print, "return") == 0)
	{
		// just enter and return at end of function
	}
	else if (table->next == NULL && strcmp(print, "print") != 0  && strcmp(print, "int") != 0)
	{
		fprintf(stderr, "error: use of undeclared variable %s\n", print);
		leave(fp, counter, table);
	}
	else if (strcmp(print, "print") != 0  && strcmp(print, "int") != 0)
	{
		for (temp = table; temp != NULL; temp = temp->next)
		{
			if (strcmp(temp->var_name, print) == 0)
			{
				// variable is declared so were good

				counter[1] = 2;
				break;
			}
			if(strcmp("read", print) == 0)
			{
				ungetc(c,fp);
				print_str = read_input(fp, counter, table);
				counter[1] = 2;
				break;
			}
			if (temp->next == NULL)
			{
				// if our next is null then our last valid node
				// does not have a the var name were looking for
				// e.g. z = 5 (but we never declared z)
				fprintf(stderr, "error: use of undeclared variable %s\n", print);
				leave(fp, counter, table);
			}
		}
	}

	if (strcmp(print, "int") == 0)
	{
		counter[1] = 0;
	}

  ungetc(c,fp);
	counter[3] = 1;
  return print_str;
}

void read_spaces(FILE *fp)
{
  char c;
  c = fgetc(fp);

  while(c == ' ' || c == '\t')
  {
    c = fgetc(fp);
  }

  ungetc(c,fp);
  return;
}

char *expr(FILE *fp, int *counter, symbol_table *table, symbol_table *main_table)
{
  char *pass_A;
  char *pass_B;
	pass_A = and_expr(fp, counter, table, main_table);
	pass_B = or_expr_prime(fp, pass_A, counter, table, main_table);
  return pass_B;
}

char *expr_prime(FILE *fp, char *left, int *counter, symbol_table *table, symbol_table *main_table)
{
  char right[MAXINDEX];
  char result[MAXINDEX];
  char *pass;
	char left_fixer[MAXINDEX];

  read_spaces(fp);
  char c = fgetc(fp);

  if (c == '+')
  {
    //consume
		strcpy(left_fixer, left);
    strcpy(right, term(fp, counter, table, main_table));
    strcpy(result, temp_func(counter, table));
    printf("    %s = add nsw i32 %s, %s\n", result, left_fixer, right);
    pass = result;

    return expr_prime(fp, pass, counter, table, main_table);
  }
  else if (c == '-')
  {
		strcpy(left_fixer, left);
    strcpy(right, term(fp, counter, table, main_table));
    strcpy(result, temp_func(counter, table));
    printf("    %s = sub nsw i32 %s, %s\n", result, left_fixer, right);
    pass = result;

    return expr_prime(fp, pass, counter, table, main_table);
  }

  ungetc(c,fp);
  return left;
}

char *term(FILE *fp, int *counter, symbol_table *table, symbol_table *main_table)
{
  char *pass_A;
  char *pass_B;
	pass_A = factor(fp, counter, table, main_table);
	pass_B = term_prime(fp, pass_A, counter, table, main_table);
  return pass_B;
}

char *term_prime(FILE *fp, char *left, int *counter, symbol_table *table, symbol_table *main_table)
{
  char right[MAXINDEX];
  char result[MAXINDEX];
  char *pass;
	// foresome reason the "left" input changes
	// during mull of temp_funct
	char left_fixer[MAXINDEX];
  read_spaces(fp);
  char c = fgetc(fp);

  if(c == '*')
  {
    //consume
		strcpy(left_fixer, left);
    strcpy(right, factor(fp, counter, table, main_table));
    strcpy(result, temp_func(counter, table));
    printf("    %s = mul nsw i32 %s, %s\n", result, left_fixer, right);
    pass = result;

    return term_prime(fp, pass, counter, table, main_table);
  }
  else if (c == '/')
  {
		strcpy(left_fixer, left);
    strcpy(right, factor(fp, counter, table, main_table));
    strcpy(result, temp_func(counter, table));
    printf("    %s = sdiv i32 %s, %s\n", result, left_fixer, right);
    pass = result;

    return term_prime(fp, pass, counter, table, main_table);
  }
  else if (c == '%')
  {
		strcpy(left_fixer, left);
    strcpy(right, factor(fp, counter, table, main_table));
    strcpy(result, temp_func(counter, table));
    printf("    %s = srem i32 %s, %s\n", result, left_fixer, right);
    pass = result;

    return term_prime(fp, pass, counter, table, main_table);
  }
  ungetc(c,fp);

  return left;
}

char *factor(FILE *fp, int *counter, symbol_table *table, symbol_table *main_table)
{
  char right[MAXINDEX];
	char result[MAXINDEX];
  char *pass;
  read_spaces(fp);
  char c = fgetc(fp);

	if (c == '!')
	{
		read_spaces(fp);
	  c = fgetc(fp);
		if(c != '=')
		{
			ungetc(c,fp);
	    strcpy(right, expr(fp, counter, table, main_table));
	    strcpy(result, temp_func(counter, table));
	    printf("    %s = xor i1 %s, 1\n", result, right);
	    pass = result; // or pass = right;
	    return pass; // factor(fp, counter, table, main_table);
		}
		else
		ungetc(c,fp);
	}
  else if (c == '(' )
  {
    //consume
    strcpy(right, expr(fp, counter, table, main_table));
    read_spaces(fp);

    c = fgetc(fp);
    if (c == ')')
        {
            pass = right;
            return pass;
        }
  }
  else if (isdigit(c) || c == '-')
  {
    //consume number
    ungetc(c,fp);
    strcpy(right, num_reader(fp, counter, table));
    pass = right;
    return pass; // OR RETURN PASS
  }
	else if (isalpha(c))
	{
		ungetc(c,fp);
		pass = var_reader(fp, counter, table, main_table);
		return pass;
	}

  //PASS MIGHT BE THE LEFT ARGUMENT INSTEAD
  ungetc(c,fp);
  return pass;
}

char *add_expr(FILE *fp, int *counter, symbol_table *table, symbol_table *main_table)
{
  char *pass_A;
  char *pass_B;
	pass_A = term(fp, counter, table, main_table);
	pass_B = expr_prime(fp, pass_A, counter, table, main_table);
	return pass_B;
}
char *gt_lt_expr(FILE *fp, int *counter, symbol_table *table, symbol_table *main_table)
{
	char *pass_A;
  char *pass_B;
	pass_A = add_expr(fp, counter, table, main_table);
	pass_B = gt_lt_expr_prime(fp, pass_A, counter, table, main_table);
	return pass_B;
}
char *gt_lt_expr_prime(FILE *fp, char *left, int *counter, symbol_table *table, symbol_table *main_table)
{
  char right[MAXINDEX];
  char result[MAXINDEX];
  char *pass;
	char left_fixer[MAXINDEX];

  read_spaces(fp);
  char c = fgetc(fp);

  if (c == '<')
  {
		strcpy(left_fixer, left);
    strcpy(right, add_expr(fp, counter, table, main_table));
    strcpy(result, temp_func(counter, table));
    printf("    %s = icmp slt i32  %s, %s\n", result, left_fixer, right);
    pass = result;

    return gt_lt_expr_prime(fp, pass, counter, table, main_table);
  }
  else if (c == '>')
  {
		strcpy(left_fixer, left);
    strcpy(right, add_expr(fp, counter, table, main_table));
    strcpy(result, temp_func(counter, table));
    printf("    %s = icmp sgt i32  %s, %s\n", result, left_fixer, right);
    pass = result;

    return gt_lt_expr_prime(fp, pass, counter, table, main_table);
  }

  ungetc(c,fp);
  return left;
}

char *eq_nq_expr(FILE *fp, int *counter, symbol_table *table, symbol_table *main_table)
{
	char *pass_A;
  char *pass_B;
	pass_A = gt_lt_expr(fp, counter, table, main_table);
	pass_B = eq_nq_expr_prime(fp, pass_A, counter, table, main_table);
	return pass_B;
}
char *eq_nq_expr_prime(FILE *fp, char *left, int *counter, symbol_table *table, symbol_table *main_table)
{
  char right[MAXINDEX];
  char result[MAXINDEX];
  char *pass;
	char left_fixer[MAXINDEX];

  read_spaces(fp);
  char c = fgetc(fp);

  if (c == '=')
  {
		read_spaces(fp);
	  char c = fgetc(fp);
		if (c == '=')
		{
			//consume
			strcpy(left_fixer, left);
	    strcpy(right, gt_lt_expr(fp, counter, table, main_table));
	    strcpy(result, temp_func(counter, table));
	    printf("    %s = icmp eq i32 %s, %s\n", result, left_fixer, right);
	    pass = result;

	    return eq_nq_expr_prime(fp, pass, counter, table, main_table);
		}
		else
		ungetc(c,fp);
  }
  else if (c == '!')
  {
		read_spaces(fp);
	  char c = fgetc(fp);
		if (c == '=')
		{
			strcpy(left_fixer, left);
	    strcpy(right, gt_lt_expr(fp, counter, table, main_table));
	    strcpy(result, temp_func(counter, table));
	    printf("    %s = icmp ne i32 %s, %s\n", result, left_fixer, right);
	    pass = result;

	    return eq_nq_expr_prime(fp, pass, counter, table, main_table);
	  }
		else
		ungetc(c,fp);
	}
	ungetc(c,fp);
	return left;
}

char *and_expr(FILE *fp, int *counter, symbol_table *table, symbol_table *main_table)
{
	char *pass_A;
  char *pass_B;
	pass_A = eq_nq_expr(fp, counter, table, main_table);
	pass_B = and_expr_prime(fp, pass_A, counter, table, main_table);
	return pass_B;
}
char *and_expr_prime(FILE *fp, char *left, int *counter, symbol_table *table, symbol_table *main_table)
{
  char right[MAXINDEX];
  char result[MAXINDEX];
  char *pass;
	char left_fixer[MAXINDEX];

  read_spaces(fp);
  char c = fgetc(fp);

  if (c == '&')
  {
		// consume next &
		read_spaces(fp);
	  char c = fgetc(fp);
    //consume
		strcpy(left_fixer, left);
    strcpy(right, eq_nq_expr(fp, counter, table, main_table));
    strcpy(result, temp_func(counter, table));
    printf("    %s = and i1 %s, %s\n", result, left_fixer, right);
    pass = result;

    return and_expr_prime(fp, pass, counter, table, main_table);
  }

  ungetc(c,fp);
  return left;
}

char *or_expr_prime(FILE *fp, char *left, int *counter, symbol_table *table, symbol_table *main_table)
{
	char right[MAXINDEX];
	char result[MAXINDEX];
	char *pass;
	char left_fixer[MAXINDEX];

	read_spaces(fp);
	char c = fgetc(fp);

	if (c == '|')
	{
		// consume next
		read_spaces(fp);
		char c = fgetc(fp);
		//consume
		strcpy(left_fixer, left);
		strcpy(right, and_expr(fp, counter, table, main_table));
		strcpy(result, temp_func(counter, table));
		printf("    %s = or i1 %s, %s\n", result, left_fixer, right);
		pass = result;

		return or_expr_prime(fp, pass, counter, table, main_table);
	}

	ungetc(c,fp);
	return left;
}

char *num_reader(FILE *fp, int *counter, symbol_table *table)
{
  char c ;
  char num[MAXINDEX];
  char *pass;
  int i = 0;
  int minus_check = 1;

  c = fgetc(fp);

	// checks numbers and negative numbers
  while(isdigit(c) || (c == '-' && minus_check))
  {
    minus_check = 0;
    num[i] = c;
    i++;
    c = fgetc(fp);
  }

  num[i] = '\0';
	pass = num;
  ungetc(c,fp);

  return pass;
}

char *var_reader(FILE *fp, int *counter, symbol_table *table, symbol_table *main_table)
{
	char c ;
  char num[MAXINDEX];
	char num_address[MAXINDEX];
  char *var;
	char *address;
  int i = 0;
	symbol_table *temp;
	int result;
	int continue_var = 0;
	c = fgetc(fp);

	// checks only variable names
	while (isalpha(c) || isdigit(c))
	{
		continue_var = 1;
		num[i] = c;
		i++;
		c = fgetc(fp);
	}
	num[i] = '\0';
	var = num;

	counter[6] = 1; // set it to thinking were reading a function
	address = search_table(fp, var, counter, table);
	if (counter[6] == 1 && counter[1] != 0)
	{
		// count[1] == 0 checks if we saw int declaration so if we saw an int
		// instead of an assignment then no need to call a function
		search_table_func(fp, var, counter, main_table);
		// we found our function
		ungetc(c,fp); // when we go in we should read a LPAREN
		address = function_caller(fp, counter, var, table, main_table);
		c = fgetc(fp);
	}

	// Difficult to identify undeclared VAR VS FUNCTION "Error"
	// Without having to fgetc and seeing more contents
	if (counter[1] == 0)
	{
		insert_table(var, temp_func(counter, table), 1, table);
	}

	ungetc(c,fp);
  return address;
}

char *temp_func(int *counter, symbol_table *table)
{
  char *pass;
  char temp_var[MAXINDEX];
  char temp_str[MAXINDEX] = "%t";

  sprintf(temp_var, "%d", counter[0]);

  pass = strcat(temp_str, temp_var); // ""%%t"
  counter[0]++;
  return pass;
}

char *label_func(int *counter, symbol_table *table)
{
  char *pass;
  char temp_var[MAXINDEX];
  char temp_str[MAXINDEX] = "branch";

  sprintf(temp_var, "%d", counter[4]);

  pass = strcat(temp_str, temp_var); // ""%%t"
  counter[4]++;
  return pass;
}

symbol_table *create_node(char *var_name, char *address, int len)
{
	symbol_table *node = malloc(sizeof(symbol_table));
	// dynamically allocated node so use "->"
	node->next = NULL;
	// self explanatory
	strcpy(node->var_name, var_name);
	// hold the temp values when allocating the var eg ""%t1"
	strcpy(node->address, address);

	node->param_len = len;
	return node;
}

void insert_table(char *var_name, char *address, int signature, symbol_table *table)
{
	symbol_table *node;
	for (node = table; node != NULL; node = node->next)
	{
    if (node->next == NULL)
    {
			node->next = create_node(var_name, address, 0);
			// string copy inside create node
			if(signature)
			printf("    %s = alloca i32, align 4\n", (node->next)->address);
			break;
    }
	}
	return;
}

void insert_table_func(char *var_name, int len, symbol_table *table)
{
	symbol_table *node;
	for (node = table; node != NULL; node = node->next)
	{
    if (node->next == NULL)
    {
			node->next = create_node(var_name, "invalid", len);
			break;
    }
	}
	return;
}

char *search_table(FILE *fp, char *var, int *counter, symbol_table *table)
{
	symbol_table *temp;
	char address[MAXINDEX];
	char *pass;
	for (temp = table; temp != NULL; temp = temp->next)
	{
		if (strcmp(temp->var_name, var) == 0)
		{
			if (counter[1] == 0)
			{
				fprintf(stderr, "error: multiple definitions of %s\n", var);
				leave(fp, counter, table);
			}
			else
			{
				strcpy(address, temp_func(counter, table));
				printf("    %s = load i32, i32* %s, align 4\n", address, temp->address);
				pass = address;
				counter[6] = 0; // we found a var and not a function
				break;
			}
		}
		if (temp->next == NULL && counter[1] != 0) // were not at int
		{
			if (counter[6] == 1)
			{
				break;
			}
			else
			{
				fprintf(stderr, "error: use of undeclared variable %s\n", var);
				leave(fp, counter, table);
			}
		}
	}
	return pass;
}

char *search_table_func(FILE *fp, char *function, int *counter, symbol_table *table)
{
	symbol_table *temp;
	char address[MAXINDEX];
	char *pass;
	int count = 0;
	for (temp = table; temp != NULL; temp = temp->next)
	{
		if (strcmp(temp->var_name, function) == 0)
		{
			if (counter[1] == 0)
			{
				fprintf(stderr, "error: multiple definitions of %s\n", function);
				leave(fp, counter, table);
			}
			break;
		}
		if (temp->next == NULL && counter[1] != 0) //were not at int
		{
			fprintf(stderr, "error: use of undeclared function %s\n", function);
			leave(fp, counter, table);
		}
	}
	return pass;
}

void leave(FILE *fp, int *counter, symbol_table *table)
{
	destroy_table(table);
  free(counter);
  fclose(fp);
	exit(0);
  return;
}

symbol_table *destroy_table(symbol_table *table)
{
	if (table == NULL || table->next == NULL)
	{
		return NULL;
	}
	free(table->next);
	free(table);
	return NULL;
}
