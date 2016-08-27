/*
 * @author Parishkrit Jain <ptjain02@gmail.com>
 *
 * @section ABOUT
 *
 * This Program is a part of course project for COA course (2014-2015) offered 
 * to 4th semester students at LNMIIT, Jaipur. The project team members are
 * Parishkrit Jain, Nishant Gupta and Nikhil Saxena.
 *
 * @section DESCRIPTION
 * 
 * This is the assembler program for pnn Architecture.
 * 
 * The purpose of assembler is to convert assembly language code into machine
 * language code as defined in pnn architecture. 
 *
 * The assembler returns 0 on successful execution, and exits silently. If the 
 * assembler encounters any error, it prints the corresponding error message on
 * standard error output and exits with a non-zero value. To print (on standard
 * output) the details of execution on each step, invoke this command with -v 
 * option.
 */

#include<stdio.h>
#include<ctype.h>
#include<time.h>
#include "linkedlist.h"
#include "tobinary.h"

#define MAXWORD 6
#define MAXBITS 40
#define MAXADD 65535
#define LARGENUM 1000

int printerr(int errno, FILE *fout, char *filename);
int getins(char *buf, FILE *fin);
int getlabel(char *label, const char *line);
int getword(char *word, const char *line, const int counter, const int ilabel);
void showerr(char *line, const int lno, int col);
int isadd(const char *val);
int ishexadd(const char *val);
int isintadd(const char *val);
void getoutname(char *file);
void housekeep(struct list **uhead, struct list **utail, 
			struct list **shead, struct list **stail,
			FILE *fout, FILE *opref, FILE *fprog);

int main(int argc, char * argv[])
{	
	int i, len, labellen;
	int lno = 0;	//keep count of line number
	int memloc = 0;	//keep track of memory location
	int loadat = 0;	//to load program at specified location
	int verbose = 0;	//for -v option
	int readstatus = 0;	//maintain counter for last value being read in
				//buffer
	int bytes = 0;	//keep length of instruction
	int halt = 0;	//checks if HLT instruction is present
	int idata = 0;	//keep traack of immediate data
	int ilabel = 0;
	int matchfound = 0;
	int nreg;
	long atadd;
	int totalins = 0;	//keep count of valid instructions
	int errors = 0;	//keep count of all errors
	char buffer[MAXBF];
	char label[MAXBF];
	char word[MAXWORD];
	char bincode[MAXBITS];
	char mnemonic[MAXWORD], mcode[MAXBITS];
	char filename[MAXBF];
	char *type;	//whether forward reference or not
	time_t now;

	struct list *symbol = NULL, *unresolved = NULL;
	struct list *shead = NULL, *stail = NULL;
	struct list *uhead = NULL, *utail = NULL;

	FILE *opref = NULL, *fout = NULL, *fprog = NULL;

	//check arguments
	if(argc == 1) {
		return printerr(1, fout, filename);
	}
	else if(argc == 2) {
		fprog = fopen(argv[1], "r");
		if(fprog == NULL) 
			return printerr(2, fout, filename);
		if(strlen(argv[1]) > (MAXBF - 6))
			return printerr(21, fout, filename);
		strcpy(filename, argv[1]);
		getoutname(filename);
	}
	else if(argc == 3) {
		fprog = fopen(argv[2], "r");
		if(fprog == NULL) 
			return printerr(2, fout, filename);
		if(strlen(argv[2]) > (MAXBF - 6))
			return printerr(21, fout, filename);
		strcpy(filename, argv[2]);
		getoutname(filename);
		if(!strcmp(argv[1], "-v")) {
			verbose = 1;
		} 
		else if(!strncmp(argv[1], "--load=", 7)) {
			if((loadat = isadd(argv[1] + 7))) {
				memloc = loadat = loadat - 1;
			}
			else
				return printerr(18, fout, filename);
		}
		else {
			return printerr(3, fout, filename);
		}
	}
	else if(argc == 4) {
		fprog = fopen(argv[3], "r");
		if(fprog == NULL) 
			return printerr(2, fout, filename);
		if(strlen(argv[3]) > (MAXBF - 1))
			return printerr(21, fout, filename);
		strcpy(filename, argv[3]);
		getoutname(filename);
		if(!strcmp(argv[1], "-v")) {
			verbose = 1;
		} 
		else if(!strcmp(argv[2], "-v")) {
			verbose = 2;
		}
		else {
			return printerr(3, fout, filename);
		}
		if(!strncmp(argv[1], "--load=", 7) || 
				!strncmp(argv[2], "--load=", 7)) {
			if(verbose == 2 && (loadat = isadd(argv[1] + 7))) {
				memloc = loadat = loadat - 1;
			}
			else if(verbose == 1 && (loadat = isadd(argv[2] + 7))) {
				memloc = loadat = loadat - 1;
			}
			else
				return printerr(18, fout, filename);
		}
		else {
			return printerr(19, fout, filename);
		}
	}
	else {
		return printerr(4, fout, filename);
	}
	// print sign-on banner
	if(verbose) {
		printf("-------------------------------------------------------------------\n");
		printf("PNN ASSEMBLER - Version 1.0 - 16-bit Architecture\n");
		printf("Integer standard: 2's complement form (16-bit)\n");
		printf("Floating numbers standard: IEEE754 Half-Precision Standard (16-bit)\n"); 
		printf("Uses 16 registers: 12 General Purpose R1-R12\n");
		printf("-------------------------------------------------------------------\n");
	}
	//load opcode reference file
	opref = fopen("opcode.ref", "r");
	if(opref == NULL) {
		return printerr(5, fout, filename);
	}
	if(verbose) {
		printf("Loaded module \"Opcode.ref\"\n");
	}
	//open output binary file
	fout = fopen(filename, "wb+");
	if(fout == NULL) {
		return printerr(6, fout, filename);
	}
	//loop 1: process each instruction at a time
	if(verbose) {
		printf("\nInitiating Loop 1...\n");
	}
	while((len = getins(buffer, fprog))) {
		++lno;
		matchfound = 0;
		idata = 0;
		ilabel = 0;
		readstatus = 0;
		rewind(opref);
		if (len == -1) {
			showerr(buffer, lno, MAXBF-2);
			return printerr(7, fout, filename);	
		}
		//ignore comments and empty lines
		for(i = 0; buffer[i] == ' ' || buffer[i] == '\t'; i++);
		if(buffer[i] == '\n') {
			if(verbose)
				printf("Parse line %d:\n\tEmpty line\n", lno);
			continue;
		}
		else if(buffer[i] == ';') {
			if(verbose)
				printf("Parse line %d:\n\tComment ignored\n", 
								lno);
			continue;
		}	
		//parse the instruction
		if(verbose) {
			printf("Parse line %d:\n", lno);
		}
		//take care of labels
		if((labellen = getlabel(label, buffer))) {
			if(labellen < 0) {
				if(labellen == -LARGENUM) {
					showerr(buffer, lno, 0);
					printerr(17, fout, filename);
					errors += 1;
					continue;
				}
				showerr(buffer, lno, labellen);
				errors += 1;
				continue;
				printerr(8, fout, filename);
			}
			//set readstatus variable
			for(i = 0; buffer[i] != ':'; i++);
			readstatus = i + 1;
			//add label to symbol table
			if(verbose) {
				printf("\tAdding \'%s\' to symbol table...\t",
									label);
			}
			if(search(&shead, label) != NULL) {
				printerr(13, fout, filename);
				errors += 1;
				continue;
			}
			symbol = makenode(label);
			if(symbol == NULL) {
				return printerr(9, fout, filename);
			}
			insert(&shead, &stail, symbol);
			if(verbose) {
				printf("Done\n");
			}
		}
		//read mnemonic, identify expected operands and write to file
		readstatus = getword(word, buffer, readstatus, ilabel);
		if(readstatus < 0) {
			showerr(buffer, lno, readstatus);
			printerr(10, fout, filename);
			errors += 1;
			continue;
		}
		while(!feof(opref)) {
			fscanf(opref, "%s %d %s", mnemonic, &bytes, mcode);
			if(!strcmp(word, mnemonic)) {
				matchfound = 1;
				//fputs(mcode, fout);
				fwrite(mcode, sizeof(char), strlen(mcode), fout);
				nreg = (16 - strlen(mcode)) / 4;
				break;
			}
		}
		if(!matchfound) {
			showerr(buffer, lno, readstatus);
			printerr(10, fout, filename);
			errors += 1;
			continue;
		}
		if(!strcmp(mnemonic, "HLT"))
			halt += 1;
		if(verbose) {
			printf("\tWriting \'%s\' at 0x%06X...\n\t", mnemonic, 
								memloc);
			printf(" %s ", mcode);
		}
		//update ilc in symbol table
		if(labellen > 0) {
			stail->memadd = memloc;
		}
		//update memloc
		memloc += bytes;
		if(memloc > MAXADD) {
			printerr(23, fout, filename);
			errors += 1;
			continue;
		}
		//update instruction count variable
		totalins += 1;
		//check for presence of immediate data or labels
		if(bytes == 4)	{
			if(mnemonic[strlen(mnemonic) - 1] == 'I')
				idata = 1;
			else
				ilabel = 1;
		}
		//write operands to file
		while(nreg--) {
			matchfound = 0;
			rewind(opref);
			readstatus = getword(word, buffer, readstatus, ilabel);
			if(readstatus < 0) {
				showerr(buffer, lno, readstatus);
				printerr(12, fout, filename);
				errors += 1;
				continue;
			}
			for(i = 0; i < 12; i++) {
				fscanf(opref, "%s %d %s", mnemonic, &bytes, 
								    mcode);
				if(!strcmp(word, mnemonic)) {
					//fputs(mcode, fout);
					fwrite(mcode, sizeof(char), strlen(mcode), fout);
					matchfound = 1;
					break;
				}
			}
			if(!matchfound) {
				showerr(buffer, lno, readstatus);
				if(!strcmp(word, "")) {
					printerr(11, fout, filename);
					errors += 1;
					continue;
				}
				printerr(12, fout, filename);
				errors += 1;
				continue;
			}
			if(verbose) {
				printf("%s ", mcode);
			}
		}
		if(idata || ilabel) {
			readstatus = getword(word, buffer, readstatus, ilabel);
			if(readstatus < 0) {
				showerr(buffer, lno, readstatus);
				if(idata) {
					printerr(14, fout, filename);
					errors += 1;
					continue;
				}
				else {
					printerr(15, fout, filename);
					errors += 1;
					continue;
				}
			}
			fwrite("\n", sizeof(char), 1, fout);
			if(idata) {
				if(chkint(word))
					dec2bin(word, bincode);
				else if(chkfloat(word))
					flt2bin(word, bincode);
				else {
					fprintf(stderr, "\nAt line %d :\n\t", lno);					
					printerr(14, fout, filename);
					errors += 1;
					continue;
				}
				fwrite(bincode, sizeof(char), strlen(bincode), fout);
				if(verbose) {
					printf(" %s", bincode);
				}
			}
			else {
				unresolved = makenode(word);
				if(unresolved == NULL) {
					return printerr(9, fout, filename);
				}
				insert(&uhead, &utail, unresolved);
				if(verbose) {
					printf("<symbol: %s>", word);
				}
				utail->memadd = memloc - 2;
				//write dummy data to be overwritten with label
				//address later
				fwrite("0000000000000000", sizeof(char), 16, fout);
			}
		}
		for(i = readstatus; isspace(buffer[i]); i++);
		if(buffer[i] != '\0' && buffer[i] != ';') {
			showerr(buffer, lno, i);
			printerr(16, fout, filename);
			errors += 1;
			continue;
		}
		fwrite("\n", sizeof(char), 1, fout);
		if(verbose) {
			printf("\n\tDone\n");
		}
	}
	if(!halt) {
		printerr(20, fout, filename);
		errors += 1;
	}
	if(verbose) {
		printf("\nCreated Symbol Table:");
		printtable(shead);
		printf("End of Loop 1.\n\nInitiating Loop 2...\n");
	}
	if(errors) {
		printf("\nErrors Detected - %d", errors);
		printf("\nProgram Status - FAILURE\n");
		if(fout != NULL)
			remove(filename);
		return -1;
	}
	//loop 2: resolve dependencies 
	for(unresolved = uhead; unresolved != NULL; 
					unresolved = unresolved->next) {
		fflush(fout);
		symbol = search(&shead, unresolved->label);
		if(symbol != NULL) {
			atadd = (((unresolved->memadd) - loadat) / 2 ) * 17;
			if(fseek(fout, atadd, SEEK_SET))
				return printerr(-1, fout, filename);
			sprintf(word, "%d", symbol->memadd);
			dec2bin(word, bincode);
			fwrite(bincode, sizeof(char), strlen(bincode), fout);
			if(verbose) {
				type = (symbol->memadd < unresolved->memadd) ?
						"Backward Reference: " : 
						"Forward  Reference: ";
				printf("\t%sSymbol '%s' found at 0x%06X.\n", 
				type, unresolved->label, symbol->memadd);
			}
		}
		else {
			fflush(stdout);
			fprintf(stderr, "\nUnresolved symbol '%s':", 
							unresolved->label);
			return printerr(22, fout, filename);
		}
	} 
	if(verbose) {
		printf("End of Loop 2.\n");
		printf("\nFinalizing Output...\t\t\t");	
	} 
	fflush(fout);
	fseek(fout, 0, SEEK_END);
	// -1 because loadat is 1 greater than start address
	//loadat = (loadat == 0) ? loadat : loadat-1;
	fwrite("\n", sizeof(char), 1, fout);
	fputs("--End of Machine Instructions and Constants\n", fout); 
	fputs("--Idetification Information:\n", fout); 
	fprintf(fout, "-Filename = %s\n", filename);
	fprintf(fout, "-Author = Me\n");
	now = time(NULL);
	fprintf(fout, "-Creation Timestamp = %s", ctime(&now));
	fprintf(fout, "-Total Instructions = %d\n", totalins);
	fprintf(fout, "-Total Size = %d Bytes\n", memloc - loadat);
	fflush(fout);	
	fputs("--Relocation Dictionary:\n", fout); 
	for(unresolved = uhead; unresolved != NULL; 
					unresolved = unresolved->next) {	
		fprintf(fout, "-0x%06X\n", unresolved->memadd);
	}
	
	fprintf(fout, "--Start Address = 0x%06X\n", loadat);
	fputs("--End of Object file.\n", fout);
	if(verbose)
		printf("Done\n");
	// do the housekeeping tasks
	housekeep(&uhead, &utail, &shead, &stail, fout, opref, fprog);
	if(verbose)
		printf("\nProgram Status - SUCCESS\n");
	return 0;
}

/* 
 * Checks if a string is valid representation of some machine address (starting 
 * at 0 and ending at value defined by MAXADD).
 * 
 * @param val The string to be checked.
 * 
 * @return integer representation of string + 1; 0 if string is invalid. 
 */
int isadd(const char *val)
{
	int n;
	if((n = (ishexadd(val) + isintadd(val))) < MAXADD)
		return n;
	return 0;
}

/* 
 * Supportive function to isadd(const char *); checks for hexadimal addresses 
 * which always end with 'h' or 'H'.
 * 
 * @param val The string to be checked.
 * 
 * @return integer representation of string + 1; 0 if string is invalid. 
 */
int ishexadd(const char *val)
{
	int c, i = 0;
	int num = 0;
	if(val[i] == '\0')
		return 0;
	while((c = toupper(val[i++])) != 'H') {
		if(isdigit(c)) {
			num = 16 * num + (c - '0');	
		}
		else if(c < 'F' && c > 'A') {
			num = 16 * num + (c - 55);	//ASCII for A is 65
		}
		else 
			return 0;
	}
	//not yet reached end of string
	if(val[i] != '\0')
		return 0;
	return (num + 1);
}

/* 
 * Supportive function to isadd(const char *); checks for decimal addresses.
 * 
 * @param val The string to be checked.
 * 
 * @return integer representation of string + 1; 0 if string is invalid. 
 */
int isintadd(const char *val)
{
	int c, i = 0;
	int num = 0;
	if(val[i] == '\0')
		return 0;
	while((c = val[i++]) != '\0') {
		if(isdigit(c))
			num = 10 * num + (c - '0');			
		else
			return 0;
	}
	return (num + 1);
}

/* 
 * Generates output file name with extension '.pnno' from source file name. 
 * 
 * @param file The string containig source file name to be modified.
 */
void getoutname(char *file)
{
	int len = strlen(file);
	while(len >= 0 && file[len--] != '.');
	if(file[len + 1] == '.')
		file[len + 1] = '\0';
	strcat(file, ".pnno");
}

/* 
 * Reads an instruction from file and puts into a character stream. Ignores 
 * comments, that is anything after ';' is encountered in a line, is ignored.
 * 
 * @param buf A poiner to the character stream.
 * @param fin A pointer to the input file.
 * 
 * @return length of the line read including the newline but excluding the
 *	   null character at the end, 0 if EOF is encountered, -1 in case of 
 * 	   buffer overflow error.  
 */
int getins(char *buf, FILE *fin)
{
	int c, i, j;

	for(i = 0; i < MAXBF-1 && (c = fgetc(fin)) != EOF && c != '\n' && 
				c != ';'; i++) {
		buf[i] = c;
	}
	if(i == MAXBF-1) {
		buf[MAXBF-1] = '\0';
		return -1;
	}
	if(c == '\n' || c == ';') 
		buf[i++] = c;
	if(c == ';')
		for(j = 0; (c = fgetc(fin)) != EOF && c != '\n'; j++);
	buf[i] = '\0';
	return i;
}

/*
 * Reads the label from a instruction and puts into a separate string. 
 * The string remians unchanged if no label is found.
 * 
 * @param label A poiner to the string for storing label.
 * @param line A pointer to the string containing the instruction.
 * 
 * @return length of the label excluding the null character at the end, 
 * 	   0 if no label is detected, -ve value indicating the position of ':',
 * 	   in case of parse error.
 */
int getlabel(char *label, const char *line)
{
	int i, j = 0;
	int c;
	int errorhere = 0;
	char temp[MAXBF];
	//ignore starting white spaces
	for(i = 0; (c = line[i]) != '\0' && isspace(c); i++);
	//parse label
	for( ; (c = line[i]) != '\0' && (isalnum(c) || c == '_'); i++) {
		temp[j++] = c;
	}
	//ignore ending white spaces
	for( ; (c = line[i]) != '\0' && isspace(c); i++);
	if(c == ':') {
		temp[j] = '\0';
		strcpy(label, temp);
		if(strlen(label) == 0)
			return -(LARGENUM);	
	}
	else {	
		errorhere = i;	
		for( ; (c = line[i]) != '\0' && c != ':'; i++);
		if(c == '\0')		
			return 0;
		else
			return -(errorhere);
	}
	return j;
}

/*
 * Reads a word from a instruction string starting at counter. 
 * The string remians unchanged if no word is found.
 * 
 * @param word A poiner to the string for storing word.
 * @param line A pointer to the string containing the instruction.
 * @param counter An int indicating current position of counter.
 * @param ilabel An int indicating whether data read is a label. 
 * 
 * @return current position of counter; -ve value indicating the position
 * 	   of error, in case of parse error.
 */
int getword(char *word, const char *line, const int counter, const int ilabel)
{
	int i = counter;
	int j = 0;
	int c;
	char temp[MAXBF];
	if(line[i] == ':')
		i++;
	for(; (c = line[i]) != '\0' && isspace(c); i++);
	if(c == ',')
		i++;
	for(; (c = line[i]) != '\0' && isspace(c); i++);		
	for( ; (c = line[i]) != '\0' && (isalnum(c) || c == '.' || c == '_'); i++) {
		if(!ilabel)
			temp[j++] = toupper(c);	
		else
			temp[j++] = c;
	}
	if(!isspace(line[i]) && (line[i] != ';') && (line[i] != ',')) {
		return (-i);				
	}
	temp[j] = '\0';
	strcpy(word, temp);
	return i;
}

/*
 * A separate function to perform all housekeeping tasks bfore exit.
 */
void housekeep(struct list **uhead, struct list **utail, 
			struct list **shead, struct list **stail,
			FILE *fout, FILE *opref, FILE *fprog)
{
	if(uhead != NULL && utail != NULL)
		cleanup(uhead, utail);	
	if(uhead != NULL && utail != NULL)
		cleanup(shead, stail);
	if(fout != NULL)
		fclose(fout);
	if(opref != NULL)
		fclose(opref);
	if(fprog != NULL)
		fclose(fprog);
}

/*
 * A separate function to show details about errors to standard error output. 
 * Takes pointer to line, line number and column number containing the error.
 */
void showerr(char *line, const int lno, int col)
{
	fflush(stdout);
	fprintf(stderr, "\nAt line %d :\n\t", lno);
	if(line[strlen(line) - 1] == '\n')
		line[strlen(line) - 1] = '\0';
	fprintf(stderr, "%s\n\t", line);
	col = (col < 0) ? -(col) : col;
	while(col--)
		fprintf(stderr, " ");
	fprintf(stderr, "^");
}

/*
 * A separate function to print errors to standard error output. Takes an int
 * value indicating the type of error, and returns the same.
 */
int printerr(int errno, FILE *fout, char *filename) 
{
	fflush(stdout);
	fprintf(stderr, "\n\tERROR: ");
	switch(errno) {
		case 1:
			fprintf(stderr, "No argument to the assembler!");
			if(fout != NULL)
				remove(filename);
			break;
		case 2:
			fprintf(stderr , "File does not exist!");
			if(fout != NULL)
				remove(filename);
			break;
		case 3:
			fprintf(stderr, "Invalid option- try '-v'");
			if(fout != NULL)
				remove(filename);
			break;
		case 4:
			fprintf(stderr, "Invalid arguments to the assembler!");
			if(fout != NULL)
				remove(filename);
			break;
		case 5:
			fprintf(stderr, "'opcode.ref' could not be loaded!");
			if(fout != NULL)
				remove(filename);
			break;
		case 6:
			fprintf(stderr, 
				"Output file could not be opened for writing!");
			if(fout != NULL)
				remove(filename);
			break;	
		case 7:
			fprintf(stderr, "Buffer Overflow!");
			if(fout != NULL)
				remove(filename);
			break;
		case 8: 
			fprintf(stderr, "Invalid Syntax for adding label!");
			break;
		case 9: 
			fprintf(stderr, "Memory could not be allocated!");
			if(fout != NULL)
				remove(filename);
			break;
		case 10: 
			fprintf(stderr, "Looks like an invalid opcode!");
			break;
		case 11: 
			fprintf(stderr, "Expected a register here!");
			break;
		case 12: 
			fprintf(stderr, "Invalid name for register!");
			break;
		case 13: 
			fprintf(stderr, "Label name already used!");
			break;
		case 14: 
			fprintf(stderr, "Invalid number!");
			break;
		case 15: 
			fprintf(stderr, "Does not look like a valid label name!");
			break;
		case 16: 
			fprintf(stderr, "Invalid Syntax!");
			break;
		case 17: 
			fprintf(stderr, "Label name cannot be empty!");
			break;
		case 18: 
			fprintf(stderr, "Invalid address specified!");
			break;
		case 19:
			fprintf(stderr, "Invalid option- try '--load=[ADD]'");
			break;
		case 20:
			fprintf(stderr, "No 'HLT' instruction detected!");
			break;
		case 21:
			fprintf(stderr, "Filename too large!");
			if(fout != NULL)
				remove(filename);
			break;
		case 22:
			fprintf(stderr, "Undefined Symbol!");
			break;
		case 23:
			fprintf(stderr, "Address range exceeded %d!", MAXADD);
			break;
		default:
			fprintf(stderr, "Unknown error!");
			break;
	}
	fprintf(stderr, "\n");
	return errno;
}
