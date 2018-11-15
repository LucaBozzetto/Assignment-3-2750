/***
 * 
 * LUCA BOZZETTO - STUDENT ID: 1071607
 * 
***/

#include "HelperFunctions.h"

bool unfold(FILE *fptr, char *unfolded, bool *endoffile){
    char buffer[999];
    char c;
    int len;
    bool continueLogicLine = true;

    buffer[0] = '\0';
    unfolded[0] = '\0'; // the unfolded logic line is empty before unfolding
    
    while(continueLogicLine && fgets(buffer,999,fptr)){
        len = strlen(buffer);

        if(strcmp(buffer,"\n")==0 && fgetc(fptr)==EOF){
            *endoffile = true;
            return true;
        }

        if(len >= 2 && buffer[len-2] == '\r' && buffer[len-1] == '\n' ){
            buffer[len-2] = '\0'; //remove crlf
            strcat(unfolded, buffer); //concatenates unfolded with the line parsed

            // c = fgetc(fptr);

            if(( c = fgetc(fptr)) == EOF || (c != ' ' && c != '\t')){
                fseek(fptr, -1L, SEEK_CUR);
                continueLogicLine = false;
            }
        }else{
            return false;
        }
    }
        
    return true;
}

void stolower(char *s){
    for(; *s; s++){
        if(('A' <= *s) && (*s <= 'Z'))
            *s = 'a' + (*s - 'A');
    }
}

bool validFileExtension(char *fileName){
    char *aux,*ext;

    aux = malloc(sizeof(char)*(strlen(fileName)+1));
    strcpy(aux,fileName);

    ext = strrchr(aux, '.');

    if (ext) {
        ext = ext + 1;
        stolower(ext);
        if(! (strcmp(ext,"vcf")==0 || strcmp(ext,"vcard")==0) ){
            free(aux);             
            return false;
        }
    }else{
        free(aux); 
        return false;
    }
    free(aux); 
    return true;
}

void substring(char *s, char **result, int start, int end){
    int len = end-start;
    char *str;

    //may have to fix that
    if (len < 0)
        return;

    str = malloc(sizeof(char)*(len+1));

    int j=0;
    for(int i=start;i<end;i++){
        str[j++] = s[i];
    }
    str[j] = '\0';
    *result = str;
}

int countValElements(char *s, char c){
    int reader;
    int len = strlen(s);
    int count = 0;

    for(reader=0; reader < len; reader++){
        if(s[reader] == '\\'){
            if(reader+1 < len && s[reader+1] == c)
                ++reader;
        }else if( s[reader] == c){
            count++;
        }
    }
    count++;
    return count;
}

// returns true if the are still chars to read false otherwise
bool removeEscapeChar(char *s, char **result, char c, int *start){
    int writer = 0;
    int reader=0;
    int len = strlen(s);
    char *outputStr;

    if(*start == len && s[len-1] == ';'){
        outputStr = malloc(sizeof(char));
        outputStr[0] = '\0';
        *start = len+1;
        *result = outputStr;
        return true;
    }
    if(*start >= len){
        return false;
    }

    outputStr = malloc(sizeof(char) * (len+1));//si puo rimuovere -*start se da problemi

    for(reader=*start; reader < len; reader++){
        if(s[reader] == '\\'){
            if(reader+1 < len && s[reader+1] == c)
                outputStr[writer++] = s[++reader];
        }else if( s[reader] == c){
            *start = reader+1;
            outputStr[writer] = '\0';
            *result = outputStr;
            return true;
        }else
            outputStr[writer++] = s[reader];
    }
    outputStr[writer] = '\0';
    *result = outputStr;
    *start = reader;
    return true;
}

//TODO qui e su tutte le altre con pos verificare che siano anche le singole pos >= 0
bool parsePropertyNameAndGroup(char *s, int start, int end,Property *prop){
    char *sbstr;
    char *ptr;
    char *group;
    char *name;
    int pos=start;
    bool foundGroup=false;

    if(end-start < 1){ //invalid property
        prop->name = malloc(sizeof(char));
        strcpy(prop->name,"\0");
        prop->group = malloc(sizeof(char));
        strcpy(prop->group,"\0");
        return false;
    }

    substring(s,&sbstr,start,end);
    ptr = strchr(sbstr,'.');
    
    if(ptr){ //&& ptr-sbstr > 0 forse non serve fare test con stringa che inizia per .FN o simile
        pos = ptr-sbstr;
        substring(sbstr, &group, 0, pos);
        prop->group = malloc(sizeof(char) * (strlen(group)+1));
        strcpy(prop->group,group);
        free(group);
        foundGroup = true;
    }else{
        prop->group = malloc(sizeof(char));
        strcpy(prop->group,"\0");
    }

    if(end-pos < 1){
        prop->name = malloc(sizeof(char));
        strcpy(prop->name,"\0");
        free(sbstr);
        return false;
    }else{
        if(!foundGroup)
            substring(sbstr, &name, pos, end);
        else
            substring(sbstr, &name, pos+1, end);
        if(strcmp(name,"") == 0){
            free(name);
            free(sbstr);
            return false;
        }
        prop->name = malloc(sizeof(char) * (strlen(name)+1));
        strcpy(prop->name,name);
        free(name);
    }

    free(sbstr);

    return true;

}

//TODO chiedere se ; puo' essere escaped
bool parseParameter(char *s, int start, int end,Property *prop, bool *text){
    char *paramString;
    char *paramToBeParsed;
    char *ptr;
    char *value;
    char *name;
    int lastSemiColonEncountered, equals, offset=0;
    bool moreParamExpected = true;
    Parameter *tmpParam;

    if(end-start < 1)
        return false;

    //while moreParamExpected && string not terminated
    while(moreParamExpected && start+offset < end ){
        substring(s,&paramString,start+offset,end);

        ptr = strchr(paramString, ';');
        if(ptr){
            moreParamExpected = true;
            lastSemiColonEncountered = ptr - paramString;
            lastSemiColonEncountered++;
            if(lastSemiColonEncountered+1 >= end){
                free(paramString);
                return false;
            }
        }else{
            moreParamExpected = false;
            lastSemiColonEncountered = strlen(paramString)+1;
        }

        substring(paramString, &paramToBeParsed, 0, lastSemiColonEncountered);

        ptr = strchr(paramToBeParsed, '=');
        if(!ptr){
            free(paramToBeParsed);
            free(paramString);
            return false;
        }
        equals = ptr - paramToBeParsed;
        if(equals > 0){
            substring(paramToBeParsed, &name, 0, equals);
        }else{
            free(paramToBeParsed);
            free(paramString);
            return false;
        }

        substring(paramToBeParsed, &value, equals+1, strlen(paramToBeParsed));
        if(strlen(value) < 1){
            free(paramToBeParsed);
            free(paramString);
            free(value);
            free(name);
            return false;
        }

        int valueLen = strlen(value);
        if(valueLen > 0 && value[valueLen-1] == ';'){
            value[valueLen-1] = '\0';
        }

        int length;
        length = strlen(value)+1;
        tmpParam = malloc(sizeof(struct param) + sizeof(char) * length);
        strcpy(tmpParam->value,value);
        strcpy(tmpParam->name,name);

        char *loweredName, *loweredValue;
        loweredName = malloc(sizeof(char) * (strlen(tmpParam->name)+1));
        strcpy(loweredName,tmpParam->name);
        stolower(loweredName);

        loweredValue = malloc(sizeof(char) * (strlen(tmpParam->value)+1));
        strcpy(loweredValue,tmpParam->value);
        stolower(loweredValue);

        if(strcmp(loweredName,"value") == 0 && strcmp(loweredValue,"text") == 0)
            *text = true;
        
        free(loweredName);
        free(loweredValue);

        insertBack(prop->parameters,tmpParam);

        free(value);
        free(name);
        free(paramToBeParsed);
        free(paramString);

        offset += lastSemiColonEncountered;
    }

    return true;
}

//parse a value from pos till length
bool parseValue( char *s, int pos, int length, Property *prop, bool compound){
    char *sbstr;
    char *value = NULL;
    char **strings = NULL;
    int start = 0;
    int i = 0;
    int count = 0;
    
    substring(s,&sbstr,pos,length);

    count = countValElements(sbstr,';');

    if(count > 1 && !compound){
        free(sbstr);
        return false;
    }

    strings = malloc(sizeof(char*)*count);
    while(removeEscapeChar(sbstr,&value,';',&start)){
        strings[i] = malloc(sizeof(char)*(strlen(value)+1));
        strcpy(strings[i],value);
        insertBack(prop->values,(void*)strings[i]);
        free(value);
        i++;
    }

    free(strings);
    free(sbstr);
    return true;
}


bool parseDate(char *s, int pos, int length, Card *tmpCard, bool text, bool bday){
    char *sbstr;
    char *timeStr, *dateStr;
    char *ptr;
    bool utc=false;
    DateTime *dateTimePtr;
    
    substring(s,&sbstr,pos,length);
    if(text){
        int lenght = strlen(sbstr);
        dateTimePtr = malloc(sizeof(DateTime) + sizeof(char)*(lenght+1));

        dateTimePtr->time[0] = '\0';
        dateTimePtr->date[0] = '\0';
        
        strcpy(dateTimePtr->text,sbstr);
    }else{
        dateTimePtr = malloc(sizeof(DateTime) + sizeof(char));
        dateTimePtr->text[0] = '\0';

        //parse time and date
        ptr = strchr(sbstr, 'T');
        if(ptr){
            int pos = ptr - sbstr;
            substring(sbstr, &timeStr, pos+1, strlen(sbstr));
            if(strlen(timeStr) < 1){
                free(timeStr);
                free(sbstr);
                free(dateTimePtr);
                return false;
            }

            //check for utc option
            ptr = strchr(timeStr, 'Z');
            if(ptr){
                int zPos = ptr - timeStr;
                if(zPos == strlen(timeStr)-1){
                    utc = true;
                    timeStr[strlen(timeStr)-1] = '\0';
                }else{
                    free(timeStr);
                    free(sbstr);
                    free(dateTimePtr);
                    return false;
                }
            }

            //check for timezone info and strip em out
            ptr = strrchr(timeStr,'-');
            if(ptr){
                int lastDash = ptr - timeStr;
                if(lastDash > 0){
                    if(timeStr[lastDash-1] != '-'){
                        timeStr[lastDash] = '\0';
                    }
                }
            }
                
            if(pos > 0){
                substring(sbstr, &dateStr, 0, pos);
            }else{
                dateStr = malloc(sizeof(char));
                strcpy(dateStr,"");
            }
        }else{
            substring(sbstr, &dateStr, 0, strlen(sbstr));

            //check for utc option
            ptr = strchr(dateStr, 'Z');
            if(ptr){
                int zPos = ptr - dateStr;
                if(zPos == strlen(dateStr)-1){
                    utc = true;
                    dateStr[strlen(dateStr)-1] = '\0';
                }else{
                    free(dateStr);
                    free(sbstr);
                    free(dateTimePtr);
                    return false;
                }
            }

            timeStr = malloc(sizeof(char));
            strcpy(timeStr,"");
        }

        //controllare constraints 8  o 9? 7 o 6?testare
        if(strlen(dateStr) > 8|| strlen(timeStr) > 6){
            free(timeStr);
            free(dateStr);
            free(sbstr);
            free(dateTimePtr);
            return false;
        }

        strcpy(dateTimePtr->date,dateStr);
        strcpy(dateTimePtr->time,timeStr);
        free(dateStr);
        free(timeStr);
    }


    dateTimePtr->isText = text;
    dateTimePtr->UTC = utc;
    //save in bday or anniversary
    if(bday)
        tmpCard->birthday = dateTimePtr;
    else
        tmpCard->anniversary = dateTimePtr;
    free(sbstr);
    return true;
}


bool parseProperty(char *s, Card* tmpCard, bool *parsedFN){
    char *sbstr=NULL;
    char *ptr;
    int pos=0;
    bool fn = false;
    bool compound = false;
    bool text = false;
    bool date = false;
    bool bday = false;
    Property *prop=NULL;
    List* parameters;
    List* values;

    prop = malloc(sizeof(Property));
    values = initializeList(&printValue, &deleteValue, &compareValues);
    parameters = initializeList(&printParameter, &deleteParameter, &compareParameters);
    prop->values = values;
    prop->parameters = parameters;
    prop->name = NULL;
    prop->group = NULL;


    ptr=strchr(s,':');
    if(ptr){
        pos = ptr-s;
        substring(s,&sbstr,0,pos);

        //separate param list from prop name and group
        ptr=strchr(sbstr,';');
        if(ptr){
            int startingParameterPos = ptr-sbstr;

            if(!parsePropertyNameAndGroup(sbstr, 0, startingParameterPos, prop)){
                free(sbstr);
                deleteProperty(prop);
                return false;
            }
            if(!parseParameter(sbstr, startingParameterPos+1, strlen(sbstr), prop, &text)){
                free(sbstr);
                deleteProperty(prop);
                return false;
            }

        }else{
            if(!parsePropertyNameAndGroup(sbstr, 0, pos, prop)){
                free(sbstr);
                deleteProperty(prop);
                return false;
            }
        }
        
        free(sbstr);
        sbstr = malloc(sizeof(char) * (strlen(prop->name)+1));
        strcpy(sbstr,prop->name);

        stolower(sbstr);

        if(strcmp(sbstr,"fn") == 0 && !(*parsedFN)){
            fn = true;
        }else if(strcmp(sbstr,"fn") == 0 && *parsedFN){
            free(sbstr);
            deleteProperty(prop);
            return false;
        }

       if(strcmp(sbstr,"n") == 0 || strcmp(sbstr,"gender") == 0 || strcmp(sbstr,"adr") == 0 || strcmp(sbstr,"tel") == 0
         || strcmp(sbstr,"org") == 0 || strcmp(sbstr,"clientpidmap") == 0)
            compound = true;

        
        if(strcmp(sbstr,"bday") == 0){
            date = true;
            bday = true;
        }

        if(strcmp(sbstr,"anniversary") == 0)
            date = true;

        if(date){
            if(pos+1 < strlen(s)){
                if(!parseDate(s, pos+1, strlen(s), tmpCard,text, bday)){
                    free(sbstr);
                    deleteProperty(prop);
                    return false;
                }
            }else{// property must have at least a 1 char value
                free(sbstr);
                deleteProperty(prop);
                return false;
            }
        }

        free(sbstr);

        //value parsing
        if(!date){
            if(pos+1 < strlen(s)){
                if(!parseValue(s, pos+1, strlen(s), prop,compound)){
                    deleteProperty(prop);
                    return false;
                }
            }else{// property must have at least a 1 char value
                deleteProperty(prop);
                return false;
            }
        }

    }else{
        deleteProperty(prop);
        return false;
    }


    if(fn)
        tmpCard->fn = prop;
    else{
        if(!date)
            insertBack(tmpCard->optionalProperties, prop);
        else
            deleteProperty(prop);
    }

    *parsedFN = fn;
    return true;
}

bool checkSpecialProperty(char *s, char *specialProp, States *state){
    char *string;
    
    string = malloc(sizeof(char)*(strlen(s)+1));
    strcpy(string,s);
    stolower(string);
    if(strcmp(string,specialProp) == 0)
        *state = *state+1;
    else{
        free(string);
        return false;
    }

    free(string);
    return true;
}

VCardErrorCode parseLine(char *s, States *state, Card* tmpCard){
    char *string;
    bool parsed = false;
    bool parsedFN = false;

    string = malloc(sizeof(char)*(strlen(s)+1));
    strcpy(string,s);

    if(*state == BEGIN){
        if(!checkSpecialProperty(string,"begin:vcard", state)){
            free(string);
            return INV_CARD;
        }   
    }else if(*state == VERSION){
        if(!checkSpecialProperty(string,"version:4.0", state)){
            free(string);
            return INV_CARD;
        }   
    }else if(*state == FN){
        if(checkSpecialProperty(string,"version:4.0", state) || checkSpecialProperty(string,"begin:vcard",state)){
            free(string);
            return INV_CARD;
        }     

        parsedFN = false;
        parsed = parseProperty(string, tmpCard, &parsedFN);
        if(!parsed){
            free(string);
            return INV_PROP;
        }

        if(parsedFN)
            *state = *state+1;
    }else if(*state == END){
        parsedFN = true;
        
        if(checkSpecialProperty(string,"end:vcard", state)){
            free(string);
            return OK;
        }

        if(checkSpecialProperty(string,"version:4.0", state) || checkSpecialProperty(string,"begin:vcard",state)){
            free(string);
            return INV_CARD;
        }      

        parsed = parseProperty(string, tmpCard, &parsedFN);
        if(!parsed){
            free(string);
            return INV_PROP;
        }

        if(parsedFN){
            free(string);
            return INV_PROP;
        }
    }else if(*state == CLOSE){
        free(string);
        return INV_CARD;
    }
    
    free(string);
    return OK;
}

/***
* 
* ASSIGNMENT 2 FUNCTIONS
* 
***/

// given a List of values(strings) it returns the right string to be printed in a vCard file
// the string must be freed by the caller
char* valuesToString(List * list){
	ListIterator iter = createIterator(list);
	char* str;
		
	str = (char*)malloc(sizeof(char));
	strcpy(str, "");
	
	void* elem;
	bool first = true;
	while((elem = nextElement(&iter)) != NULL){
		char* currDescr = list->printData(elem);
		int newLen = strlen(str)+50+strlen(currDescr);
		str = (char*)realloc(str, newLen);
		
		if( !first ){
			strcat(str, ";");
		}else{
			first = false;
		}
		// strcat(str, "\n");
		strcat(str, currDescr);

		free(currDescr);
	}
	
	return str;
}

// given a Parameter it returns the right strings to be printed in a vCard file
// the string must be freed by the caller
char *writeCardPrintParameter(void *toBePrinted){
	char* tmpStr;
	Parameter* tmpParam;
	int len;
	
	if (toBePrinted == NULL){
		return NULL;
	}
	
	tmpParam = (Parameter*)toBePrinted;
		
	len = strlen(tmpParam->name)+strlen(tmpParam->value)+3;
	tmpStr = (char*)malloc(sizeof(char)*len);
	
	sprintf(tmpStr, ";%s=%s", tmpParam->name, tmpParam->value);
	
	return tmpStr;
}

// given a property it returns the right strings to be printed in a vCard file
// the strings must be freed by the caller
void writeCardPrintProperty(Property *prop, char **group, char **name, char **parameters, char **values){

    char *groupString, *nameString, *propValuesString, *propParametersString;
    groupString = NULL;
    nameString = NULL;
    propValuesString = NULL;
    propParametersString = NULL;

    // Check for group
    if(strcmp(prop->group,"") != 0){
        groupString = malloc(sizeof(char) * (strlen(prop->group)+2));
        strcpy(groupString,prop->group);
        strcat(groupString,".");
    }else{
        groupString = malloc(sizeof(char));
        groupString[0] = '\0';
    }

    //Check for parameters
    if(getLength(prop->parameters) > 0){
        // create ';parameter=value' string

        ListIterator iter = createIterator(prop->parameters);
        char* str;

        str = (char*)malloc(sizeof(char) * 1);
        strcpy(str, "");

        void* elem;
        while((elem = nextElement(&iter)) != NULL){
            char* currDescr = writeCardPrintParameter(elem);
            int newLen = strlen(str)+50+strlen(currDescr);
            str = (char*)realloc(str, newLen);

            // strcat(str, "\n");
            strcat(str, currDescr);

            free(currDescr);
        }

        propParametersString = malloc(sizeof(char) * (strlen(str) + 1));
        strcpy(propParametersString,str);
        free(str);

    }else{
        propParametersString = malloc(sizeof(char));
        propParametersString[0] = '\0';
    }

    nameString = malloc(sizeof(char) * (strlen(prop->name) + 1)); // +1 ??
    strcpy(nameString,prop->name);


    //Check values
    propValuesString = valuesToString(prop->values);

    *group = groupString;
    *name = nameString;
    *parameters = propParametersString;
    *values = propValuesString;

    return;
}

VCardErrorCode validateParameters(List *list){
    if(list == NULL)
        return INV_PROP;

    ListIterator iter = createIterator(list);
    void* elem;
    // All list entries must be not null strings
    while((elem = nextElement(&iter)) != NULL){
        Parameter *param = (Parameter*)elem;
        char *name = param->name;
        char *value = param->value;

        if(name == NULL && value == NULL)
            return INV_PROP;

        if((strcmp(name,"") == 0) || (strcmp(value,"") == 0))
            return INV_PROP;
    }

    return OK;
}

// Checks if the FN property is valid
VCardErrorCode validateFN(Property *prop){
    char *name;
    VCardErrorCode result;
    
    // If we don't have any string
    if (prop->name == NULL)
        return INV_PROP;

    name = malloc(sizeof(char) * (strlen(prop->name) + 1));
    strcpy(name,prop->name);
    stolower(name);

    //if the name is not exactly fn
    if(strcmp(name,"fn") != 0){
        free(name);
        return INV_PROP;
    }

    free(name);

    //group must not be null
    if(prop->group == NULL)
        return INV_PROP;

    // parameters list must not be null
    if(prop->parameters == NULL)
        return INV_PROP;

    // If the list contains parameters they must be valid (empty string at least)
    if((result = validateParameters(prop->parameters)) != OK)
        return result;

    //if the values list doesn't exist
    if(prop->values == NULL)
        return INV_PROP;
    
    //if the list has more than one element
    if(prop->values->length != 1)
        return INV_PROP;

    // if (erroneously) the list has length 1 but no elements
    if (prop->values->head == NULL)
        return INV_PROP;
    
    // if the value stored is null (no string)
    if(prop->values->head->data == NULL)
        return INV_PROP;

    /* TODO ask TA if fn empty string is valid 
    if(strcmp(prop->values->head->data,"") == 0)
        return INV_PROP;
    */

    return OK;
}

/***
 * if n == -1 it means multiple without specification
 * if n == number it means it has to have exactly that number of values
 ***/
VCardErrorCode validateValues(List *list, int n){
    if(list == NULL)
        return INV_PROP;
    
    if(n == -1){
        if(list->length < 1)
            return INV_PROP;
    }else{
        //if the list has not exactly n elements
        if(list->length != n)
            return INV_PROP;
    }

    ListIterator iter = createIterator(list);
    void* elem;
    int i=0;
    // All list entries must be not null strings
    while((elem = nextElement(&iter)) != NULL){
        char* currDescr = list->printData(elem);

        if(currDescr == NULL)
            return INV_PROP;

        i++;
        free(currDescr);
    }

    // The list must contains exactly n entries
    if(i != n && n != -1)
        return INV_PROP;

    return OK;
}

VCardErrorCode validateProperty(Property *prop, bool *kind,bool *n, bool *gender, bool *prodID, bool *rev, bool *group, bool *member){
    char *name;
    VCardErrorCode result;

    // this should be useless, elem is checked in the while condition to not be null but in case something changes...
    if(prop == NULL)
        return INV_PROP;

    //group must not be null
    if(prop->group == NULL)
        return INV_PROP;

    // parameters list must not be null
    if(prop->parameters == NULL)
        return INV_PROP;

    // If the list contains parameters they must be valid (empty string at least)
    if((result = validateParameters(prop->parameters)) != OK)
        return result;

    //if the values list doesn't exist
    if(prop->values == NULL)
        return INV_PROP;

    // If we don't have any string
    if (prop->name == NULL)
        return INV_PROP;

    name = malloc(sizeof(char) * (strlen(prop->name) + 1));
    strcpy(name,prop->name);
    stolower(name);

    // The prop name has to be one of the following:
    if((strcmp(name,"source") == 0) || (strcmp(name,"xml") == 0) || (strcmp(name,"nickname") == 0) 
        || (strcmp(name,"photo") == 0) || (strcmp(name,"email") == 0) || (strcmp(name,"impp") == 0)
        || (strcmp(name,"lang") == 0) || (strcmp(name,"tz") == 0) || (strcmp(name,"geo") == 0)
        || (strcmp(name,"title") == 0) || (strcmp(name,"role") == 0) || (strcmp(name,"logo") == 0)
        || (strcmp(name,"related") == 0) || (strcmp(name,"categories") == 0)
        || (strcmp(name,"note") == 0) || (strcmp(name,"sound") == 0) || (strcmp(name,"uid") == 0)
        || (strcmp(name,"url") == 0) || (strcmp(name,"key") == 0)
        || (strcmp(name,"fburl") == 0) || (strcmp(name,"caladruri") == 0) || (strcmp(name,"caluri") == 0)){

            // Multiple cardinality, exactly one value
            free(name);

            result = validateValues(prop->values, 1);
            if(result != OK)
                return INV_PROP;
    }else if(strcmp(name,"adr") == 0){
        //adr must have exactly 7 values
        free(name);

        result = validateValues(prop->values, 7);
        if(result != OK)
            return INV_PROP;
    }else if(strcmp(name,"clientpidmap") == 0){
        //clientpidmap must have exactly 2 values
        free(name);

        result = validateValues(prop->values, 2);
        if(result != OK)
            return INV_PROP;
    }else if((strcmp(name,"tel") == 0) || (strcmp(name,"org") == 0)){
        //multiple values
        free(name);

        result = validateValues(prop->values, -1);
        if(result != OK)
            return INV_PROP;
    }else if((strcmp(name,"member") == 0)){
        // member must appear only if kind prop with value group is present
        free(name);
        
        result = validateValues(prop->values, 1);
        if(result != OK)
            return INV_PROP;

        *member = true;
    }else if((strcmp(name,"n") == 0) && !(*n)){
        // n must appear only once with exactly 5 values
        free(name);

        result = validateValues(prop->values, 5);
        if(result != OK)
            return INV_PROP;
        
        *n = true;
    }else if((strcmp(name,"gender") == 0) && !(*gender)){
        // gender must appear only once with maximum 2 values
        free(name);

        result = validateValues(prop->values, 1);
        int result2 = validateValues(prop->values, 2);
        if(result != OK && result2 != OK)
            return INV_PROP;
        
        *gender = true;
    }else if((strcmp(name,"prodid") == 0) && !(*prodID)){
        // prodid must appear only once with exactly 1 value
        free(name);

        result = validateValues(prop->values, 1);
        if(result != OK)
            return INV_PROP;

        *prodID = true;
    }else if((strcmp(name,"rev") == 0) && !(*rev)){
        // rev must appear only once with exactly 1 value
        free(name);

        result = validateValues(prop->values, 1);
        if(result != OK)
            return INV_PROP;
        
        *rev = true;
    }else if((strcmp(name,"kind") == 0) && !(*kind)){
        // kind must appear only once with exactly 1 value
        free(name);

        result = validateValues(prop->values, 1);
        if(result != OK)
            return INV_PROP;

        char *loweredValue;

        // since validateValues(list, 1) returned OK it means we have a value list
        // with exactly one element, we then check the string
        loweredValue = malloc(sizeof(char) * (strlen(prop->values->head->data) + 1));
        strcpy(loweredValue,prop->values->head->data);

        *group = strcmp(loweredValue,"group") == 0?true:false;
        free(loweredValue);

        *kind = true;
    }else if((strcmp(name,"bday") == 0) || (strcmp(name,"anniversary") == 0)){
        return INV_DT;
    }else if((strcmp(name,"version") == 0) || (strcmp(name,"begin") == 0) || (strcmp(name,"end") == 0)){
        free(name);

        return INV_CARD;
    }else{
        free(name);

        return INV_PROP;
    }

    return OK;
}

bool isLeap(int year){
    return ((year & 3) == 0 && ((year % 25) != 0 || (year & 15) == 0));
}

bool checkDate(int year, int month, int day){

    if ( month < 1 || month > 12)
        return false;

    if(month == 4 || month == 6 || month == 9 || month == 11){
        return (day <= 30);
    }else if ( month == 2){
        if(year != -1 && isLeap(year))
            return (day <= 29);
        else
            return (day <= 28);
    }else{
        return (day <= 31);
    }

    return false;
}

bool validateDate(char *string){
    int length =  strlen(string);
    int month = 0;
    int day = 0;
    int year = 0;

    bool emptyYear = false;
    bool emptyYearMonth = false;
    bool emptyDay = false;

    // The minimum string is ---1
    if(length < 4)
        return false;

    for(int i = 0; i < length; i++){
        char c = string[i];

        if(i == 0 && c == '-')
            emptyYear = true;

        if(i == 1 && c != '-' && emptyYear)
            return false;

        // c must be a digit or a dash
        if ( c != '-' && !(c >= '0' && c <= '9'))
            return false;

        

        //the string starts with -- and can its length is maximum 6
        if(emptyYear){ // --

            if( length > 6) // "--mmdd"
                return false;

            if(i < 2) // the first 2 digits are dashes
                continue;

            if(i == 2 && c == '-'){ // empty month and year
                emptyYearMonth = true;
            }

            if(emptyYearMonth){

                if(i < 3) // the first 3 digits are dashes
                    continue;

                if(length != 5) // "---dd"
                    return false;

                if(c == '-') // dd cannot be truncated
                    return false;

                if( i == 3 && c > '3') //---cd
                    return false;

                if( i == 3 || i == 4){
                    if(i == 4)
                        day *= 10;
                    day += c - '0';
                }

                if( i == 4 && day > 31) 
                    return false;


            }else{ // !emptyYearMonth --mmdd

                if(length != 6) // "--mmdd"
                    return false;


                if( i == 2 && c > '1') //--cmdd month between 0 - 12
                    return false;
                
                if( i == 2 || i == 3){
                    if(i == 3)
                        month *= 10;
                    month += c - '0';
                }

                if(i == 3 && month > 12)
                    return false;

                if(c == '-')
                    return false;

                if( i == 4 && c > '3') //--mmcd
                    return false;

                if( i == 4 || i == 5){
                    if(i == 5)
                        day *= 10;
                    day += c - '0';
                }

                if( i == 5 && day > 31) 
                    return false;

                if(i == 5)
                    if(!checkDate(-1, month, day))
                        return false;
                
            }

        }else{ // !emptyYear yyyy

            if( i < 4){ 
                if(c == '-') // first 4 must be digits yyyy
                    return false;
                else{
                    if(i != 0)
                        year *= 10;
                    year += c - '0';
                    continue;
                }
                    
            }

            if(i == 4 && c == '-') // empty day and year
                emptyDay = true;
            
            if(emptyDay){ //yyyy-mm
                if( i < 5)
                    continue;

                if(length != 7)
                    return false;

                if(c == '-') // mm cannot be truncated
                    return false;

                if( i == 5 && c > '1') //yyyy-cm
                    return false;

                if( i == 5 || i == 6){
                    if(i == 6)
                        month *= 10;
                    month += c - '0';
                }

                if( i == 6 && month > 12) 
                    return false;
                

            }else{ // !emptyMonth yyyymmdd

                if(length != 8) // "yyyymmdd"
                    return false;

                if( i == 4 && c > '1') //yyyycmdd month between 0 - 12
                    return false;

                if( i == 4 || i == 5){
                    if(i == 5)
                        month *= 10;
                    month += c - '0';
                }

                if(i == 5 && month > 12)
                    return false;

                if(c == '-')
                    return false;

                if( i == 6 && c > '3') //--mmcd
                    return false;

                if( i == 6 || i == 7){
                    if(i == 7)
                        day *= 10;
                    day += c - '0';
                }

                if( i == 7 && day > 31) 
                    return false;

                if( i == 7)
                    if (!checkDate(year, month, day))
                        return false;

            }
        }
    }

    if(emptyYear){
        
        if(!emptyYearMonth && length == 6)
            return true;

        if(emptyYearMonth && length == 5)
            return true;
            
    }else{

        if(!emptyDay && length == 8)
            return true;

        if(emptyDay && length == 7)
            return true;

        if(length == 4)
            return true;
    }

    return false;
}

bool validateTime(char *string){
    int length =  strlen(string);
    int hours = 0;

    bool hasHour = true;
    bool hasMinute = false;
    bool hasSecond = false;

    bool range = false;

    // The minimum string is --ss
    if(length < 2)
        return false;

    for(int i = 0; i < length; i++){
        char c = string[i];

        if(i == 0 && c == '-')
            hasHour = false;

        // c must be a digit or a dash
        if ( c != '-' && !(c >= '0' && c <= '9'))
            return false;

        if(!hasHour){

            if(i < 1)
                continue;

            if (i == 1 && c == '-')
                hasMinute = false;

            if (i == 1 && c != '-')
                hasMinute = true;


            if (!hasMinute){ // emptyMinute

                if( i >= 2 && c == '-')
                    return false;

                if(i == 2 && c >= '0' && c <= '6'){
                    range = c == '6' ? true : false;
                    hasSecond = true;
                }

                if(i == 3 && c >= '0' && c <= '9')
                    if(c > '0' && range)
                        return false;


            }else{ // !emptyMinute
                
                if( c == '-')
                    return false;

                if(i == 1 && c >= '0' && c <= '6')
                    range = c == '6' ? true : false;

                if(i == 2 && c >= '0' && c <= '9')
                    if(c > '0' && range)
                        return false;

                if(i == 3 && c >= '0' && c <= '6'){
                    range = c == '6' ? true : false;
                    hasSecond = true;
                }

                if(i == 4 && c >= '0' && c <= '9')
                    if(c > '0' && range)
                        return false;
            }

        }else{ // !emptyHour
            if(i < 2 && c != '-'){
                if(i == 1)
                    hours *= 10;
                hours += c - '0';
            }

            if (i < 2 && c == '-')
                return false;

            if (i == 1 && !(hours >= 0 && hours <= 23))
                return false;

            if (i == 2 && c == '-')
                hasMinute = false;

            if(i == 2 && c != '-')
                hasMinute = true;

            if (!hasMinute){ // emptyMinute

                if(i > 2 &&  c == '-')
                    return false;

                if(i == 3 && c >= '0' && c <= '6'){
                    range = c == '6' ? true : false;
                    hasSecond = true;
                }

                if(i == 4 && c >= '0' && c <= '9')
                    if(c > '0' && range)
                        return false;
                

            }else{ // !emptyMinute

                if( c == '-')
                    return false;

                if(i == 2 && c >= '0' && c <= '6')
                    range = c == '6' ? true : false;

                if(i == 3 && c >= '0' && c <= '9')
                    if(c > '0' && range)
                        return false;
                
                if(i == 4 && c >= '0' && c <= '6'){
                    range = c == '6' ? true : false;
                    hasSecond = true;
                }

                if(i == 5 && c >= '0' && c <= '9')
                    if(c > '0' && range)
                        return false;
            }

        }

    }

    if(hasHour){
        if(hasMinute){
            if(hasSecond){
                return (length == 6);
            }else{
                return (length == 4);
            }
        }else{
            if(hasSecond){
                return (length == 4);
            }else{
                return (length == 2);
            }
        }
    }else{
        if(hasMinute){
            if(hasSecond){
                return (length == 5);
            }else{
                return (length == 3);
            }
        }else{
            if(hasSecond){
                return (length == 4);
            }else{
                return (length == 0);
            }
        }

    }

    return false;
}

VCardErrorCode validateDateTime(DateTime *date){
    if(date == NULL)
        return INV_DT;

    
    if(date->isText){ // text date

        if(date->UTC) // text date cannot have utc parameter true
            return INV_DT;

        // date and time string must be empty
        if( (strcmp(date->date,"") != 0 ) || (strcmp(date->time,"") != 0))
            return INV_DT;

        //TODO ask TA if a text datetime can have an empty string value
        if(strcmp(date->text, "") == 0)
            return INV_DT;        

    }else{

        // text must be the empty string
        if(strcmp(date->text,"") != 0)
            return INV_DT;

        if( (strcmp(date->date, "")) == 0 && (strcmp(date->time, "") == 0))
            return INV_DT;

        if( strcmp(date->date, "") != 0){
            if( !validateDate(date->date))
                return INV_DT;   
        }

        if( strcmp(date->time, "") != 0){
            if( !validateTime(date->time))
                return INV_DT;   
        }

    }

    return OK;
}

// escapes all occurences of the char c in the given string
char* escapeCharacter(char *string, char c){
    char *output;
    bool escaped = false;

    if (!string)
        return NULL;
    
    int length = strlen(string);

    output = malloc(sizeof(char) * (length * 2 + 3));
    output[0] = '"';

    int j = 1;
    for(int i=0; i < length; i++){
        if(string[i] == c && !escaped)
            output[j++] = '\\';

        if(string[i] == '\\')
            escaped = true;
        else
            escaped = false;

        output[j++] = string[i];
    }
    
    output[j++] = '"';
    output[j] = '\0';

    return output;
}

char* unescapeString(char *string, int start,int end){
    char *output;

    if (string == NULL)
        return NULL;
    
    if (end - start < 0)
        return NULL;

    output = malloc(sizeof(char) * ((end-start) + 1));
    
    int j = 0;
    for(int i = start+1; i < end; i++){
        if(string[i] == '\\' && i+1 < end && string[i+1] == '"'){
            output[j++] = string[i+1];
        }else{
            output[j++] = string[i];
        }
    }

    output[j] = '\0';
    return output;
}

//return the number of digits in an integer
int numPlaces (int n){
    int r = 1;
    // if (n < 0) n = (n == INT_MIN) ? INT_MAX: -n;
    while (n > 9) {
        n /= 10;
        r++;
    }
    return r;
}