/***
 * 
 * LUCA BOZZETTO - STUDENT ID: 1071607
 * 
***/

#ifndef _CARDPARSER_H
    #include "VCardParser.h"
#endif

// Used to track the status while parsing the vcard
typedef enum states {BEGIN, VERSION, FN, END, CLOSE} States;

/***
 * 
 * Removes CRLF chars and eventually unfolds. The resulst will be available in unfolded if produced. 
 * Returns true if the line(s) parsed was(were) valid, false otherwise.
 * If the end of file is reached the endoofile var will be set to true.
 * 
 ***/ 
bool unfold(FILE *fptr, char *unfolded, bool *endoffile);

// Simply convert the string to lower case.
void stolower(char *s);

// Checks if the provided string has either vcf or vcard extension
bool validFileExtension(char *fileName);

/***
 * 
 * Allocates in result the substring between start included and end excluded.
 * If the substring is of length 0 result will contain just the null terminator.
 * If the substring is not valid(negative length) the function will do nothing.
 * 
 ***/
void substring(char *s, char **result, int start, int end);

/***
 * 
 * parse the parameters from start included to end excluded.
 * If the parameter has VALUE=text the the text variable is set to true.
 * 
 ***/
// bool parseParameter(char *s, int start, int end,Property *prop, bool *text);

/***
 * 
 * Parse the value of s from start included to end excluded.
 * If the value is supposed to be a compound one the variable compound must be set to true
 * 
 ***/
// bool parseValue( char *s, int start, int end, Property *prop, bool compound);

/***
 * 
 * Despite the name this function actually parse the entire logical line s.
 * It will parse in separeted functions the logical structs of the line (property name,value and parameters)
 * If the parsing of the line went wrong for any reason (invalid property name, value or special property expected)
 * the function returns false, true otherwise.
 * 
 ***/
// bool parseProperty(char *s, Card* tmpCard, bool *parsedFN);

/***
 * 
 * This function is used to easyly check if the given lines exactly matches the specialProp string.
 * This is useful for lines like BEGIN or VERSION.
 * 
 ***/
// bool checkSpecialProperty(char *string, char *specialProp, States *state);

/***
 * 
 * This is the function called by the createCard function for every logical line.
 * Its purpose is to go trough the various possible states the card parsing may be in.
 * If an unexpected property is parsed or if the line parsed is not recognized as correct
 * the function return the appropriate error.
 * 
 ***/
VCardErrorCode parseLine(char *s, States *state, Card* tmpCard);

/***
* 
* ASSIGNMENT 2 FUNCTIONS
* 
***/

/***
* 
* Modified to obtain the right vCard to string format for values
* 
***/
char* valuesToString(List * list);

/***
* 
* Modified to obtain the right vCard to string format for parameters 
* 
***/
char *writeCardPrintParameter(void *toBePrinted);

// given a property it returns the right strings to be printed in a vCard file
// the strings must be freed by the caller
void writeCardPrintProperty(Property *prop, char **group, char **name, char **parameters, char **values);

// Checks if the FN property is valid
VCardErrorCode validateFN(Property *prop);

// Checks if the optional property is valid
VCardErrorCode validateProperty(Property *elem, bool *kind,bool *n, bool *gender, bool *prodID, bool *rev, bool *group, bool *member);

// Checks if a DateTime is in the correct format
VCardErrorCode validateDateTime(DateTime *date);

// escapes all occurences of the char c in the given string
char* escapeCharacter(char *string, char c);

// Given a string it returns the substring between start and end excluded and removes the escape char '\\'
char* unescapeString(char *string, int start,int end);

// Returns the number of digits in an integer
int numPlaces (int n);

// Extract a substring from start included to end excluded
// void substring(char *s, char **result, int start, int end);
