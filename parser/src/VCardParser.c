/***
 * 
 * LUCA BOZZETTO - STUDENT ID: 1071607
 * 
***/

#include "VCardParser.h"
#include "HelperFunctions.h"

char* printValue(void* toBePrinted){
    char* tmpStr;
	char* tmpValue;
	int len;
	
	if (toBePrinted == NULL){
		return NULL;
	}
	
	tmpValue = (char*)toBePrinted;
    
	len = strlen(tmpValue);
	tmpStr = (char*)malloc(sizeof(char)*(len+1));
	strcpy(tmpStr,tmpValue);
	
	return tmpStr;
}

int compareValues(const void *first, const void *second){
	char *tmpStr1;
	char *tmpStr2;
	
	if (first == NULL || second == NULL){
		return 0;
	}
	
	tmpStr1 = (char*)first;
	tmpStr2 = (char*)second;
	
	return strcmp(tmpStr1, tmpStr2);
}

void deleteValue(void *toBeDeleted){
    char *tmpValue;
	
	if (toBeDeleted == NULL){
		return;
	}
	
	tmpValue = (char*)toBeDeleted;
	
    if(tmpValue)
	    free(tmpValue);
}

char *printParameter(void *toBePrinted){
	char* tmpStr;
	Parameter* tmpParam;
	int len;
	
	if (toBePrinted == NULL){
		return NULL;
	}
	
	tmpParam = (Parameter*)toBePrinted;
		
	len = strlen(tmpParam->name)+strlen(tmpParam->value)+20;
	tmpStr = (char*)malloc(sizeof(char)*len);
	
	sprintf(tmpStr, "Parameter: %s Value: %s", tmpParam->name, tmpParam->value);
	
	return tmpStr;
}

int compareParameters(const void *first, const void *second){
	Parameter *tmpParam1;
	Parameter *tmpParam2;
	
	if (first == NULL || second == NULL){
		return 0;
	}
	
	tmpParam1 = (Parameter*)first;
	tmpParam2 = (Parameter*)second;

    if(strcmp(tmpParam1->name,tmpParam2->name) == 0)
        return strcmp(tmpParam1->value,tmpParam2->value);
	
    return strcmp(tmpParam1->name,tmpParam2->name);
}

void deleteParameter(void *toBeDeleted){
    Parameter *tmpParameter;
	
	if (toBeDeleted == NULL){
		return;
	}
	
	tmpParameter = (Parameter*)toBeDeleted;
	
	free(tmpParameter);
}

char* printProperty(void* toBePrinted){
    char* tmpStr;
    char* paramStr;
    char* valueStr;
	Property* tmpProp;
	int len;
	
	if (toBePrinted == NULL){
		return NULL;
	}
	
	tmpProp = (Property*)toBePrinted;
    paramStr = toString(tmpProp->parameters);
    valueStr = toString(tmpProp->values);
		
	len = strlen(tmpProp->name)+strlen(tmpProp->group)+strlen(paramStr)+strlen(valueStr)+41;
	tmpStr = (char*)malloc(sizeof(char)*len);
	
	sprintf(tmpStr, "Property: %s Group: %s\nParameters: %s Values: %s", 
            tmpProp->name, tmpProp->group, paramStr, valueStr);
	
    free(paramStr);
    free(valueStr);

	return tmpStr;
}

void deleteProperty(void* toBeDeleted){
    Property *tmpProperty;
	
	if (toBeDeleted == NULL){
		return;
	}
	
	tmpProperty = (Property*)toBeDeleted;
	
    if(tmpProperty->name){
        free(tmpProperty->name);
    }
	if(tmpProperty->group){
        free(tmpProperty->group);
    }
    if(tmpProperty->parameters){
        freeList(tmpProperty->parameters);
    }
    if(tmpProperty->values){
        freeList(tmpProperty->values);
    }
	free(tmpProperty);
}
int compareProperties(const void* first,const void* second){
    Property *tmpProp1;
	Property *tmpProp2;
	
	if (first == NULL || second == NULL){
		return 0;
	}
	
	tmpProp1 = (Property*)first;
	tmpProp2 = (Property*)second;

    if(!tmpProp1->name || !tmpProp2->name)
        return 0;

    return strcmp(tmpProp1->name,tmpProp2->name);

}

void deleteDate(void* toBeDeleted){
    DateTime *tmpDatetime;
	
	if (toBeDeleted == NULL){
		return;
	}

    tmpDatetime = (DateTime*)toBeDeleted;
    free(tmpDatetime);
}

int compareDates(const void* first,const void* second){
    return 0;
}

char* printDate(void* toBePrinted){
    char *tmpStr;
	DateTime *tmpDatetime;
	int len = 0;
	
	if (toBePrinted == NULL){
		return NULL;
	}
	
	tmpDatetime = (DateTime*)toBePrinted;

    if(tmpDatetime->isText){
        len = strlen(tmpDatetime->text) + 14;
	    tmpStr = (char*)malloc(sizeof(char)*len);
        sprintf(tmpStr, "\nUTC:%d Text: %s", tmpDatetime->UTC, tmpDatetime->text);
    }else{
        len = strlen(tmpDatetime->date) + strlen(tmpDatetime->time) + 21;
	    tmpStr = (char*)malloc(sizeof(char)*len);
        sprintf(tmpStr, "\nUTC:%d date: %s time: %s", tmpDatetime->UTC, tmpDatetime->date, tmpDatetime->time);
    }
	
	
	return tmpStr;
}


char* printCard(const Card* obj){
    char *tmpStr;
    char *fnStr;
    char *optionalPropStr;
    char *birthdayStr;
    char *anniversaryStr;
    bool birthdayAllocated, anniversaryAllocated;
    int len=0;

    birthdayAllocated = anniversaryAllocated = false;

    if(obj == NULL)
        return NULL;

    if(!obj->fn || !obj->optionalProperties)
        return NULL;

    if(obj->birthday != NULL){
        birthdayStr = printDate(obj->birthday);
        len += strlen(birthdayStr);
        birthdayAllocated = true;
    }else{ 
        birthdayStr = "";
    }

    if(obj->anniversary != NULL){
        anniversaryStr = printDate(obj->anniversary);
        len += strlen(anniversaryStr);
        anniversaryAllocated = true;
    }else{
        anniversaryStr = "";
    }

    fnStr = printProperty((void*)obj->fn);

    optionalPropStr = toString(obj->optionalProperties);

    len += strlen(fnStr)+strlen(optionalPropStr)+47;
	tmpStr = (char*)malloc(sizeof(char)*len);
	
	sprintf(tmpStr, "Card:\n%s\n\nProperties: %s\nBirthday: %s\nAnniversary: %s\n", 
            fnStr, optionalPropStr, birthdayStr, anniversaryStr);
	
    free(fnStr);
    free(optionalPropStr);

    if(birthdayAllocated)
        free(birthdayStr);
    if(anniversaryAllocated)
        free(anniversaryStr);
    
    return tmpStr;
    
}

char *printError(VCardErrorCode err){
    char* str = NULL;

    if(err == OK){
        str = malloc(sizeof(char) * 34);
        sprintf(str, "Operation completed successfully\n");
    }else if(err == INV_FILE){
        str = malloc(sizeof(char) * 14);
        sprintf(str, "Invalid File\n");
    }else if(err == INV_CARD){
        str = malloc(sizeof(char) * 14);
        sprintf(str, "Invalid Card\n");
    }else if(err == INV_PROP){
        str = malloc(sizeof(char) * 18);
        sprintf(str, "Invalid Property\n");
    }else if(err == INV_DT){
        str = malloc(sizeof(char) * 14);
        sprintf(str, "Invalid Date\n");
    }else if(err == OTHER_ERROR){
        str = malloc(sizeof(char) * 13);
        sprintf(str, "Other Error\n");
    }else if(err == WRITE_ERROR){
        str = malloc(sizeof(char) * 13);
        sprintf(str, "Write error\n");
    }else{
        str = malloc(sizeof(char) * 19);
        sprintf(str, "Unknow error code\n");
    }

    return str;
}

VCardErrorCode createCard(char* fileName, Card** newCardObject){
    char buffer[999]; //see answer by ta https://moodle.socs.uoguelph.ca/mod/forum/discuss.php?d=1836
    FILE *fileptr;
    bool cardValid, endoffile;
    VCardErrorCode res;
    List *optionalProperties;

    Card *tmpCard;

    cardValid = true;
    endoffile = false;
    res = INV_CARD;
    States state = BEGIN;
    buffer[0] = '\0';

    if(!newCardObject){
        // *newCardObject = NULL;
        return OTHER_ERROR;
    }

    //Check for valid pointer
    if(!fileName){
        *newCardObject = NULL;
        return INV_FILE;
    }

    //check file extensions
    if(!validFileExtension(fileName)){
        newCardObject = NULL;
        return INV_FILE;
    }

    fileptr = fopen(fileName, "r");
    if(fileptr != NULL){
        tmpCard = malloc(sizeof(Card));
        tmpCard->fn=NULL;
        optionalProperties = initializeList(&printProperty,&deleteProperty,&compareProperties);
        tmpCard->optionalProperties = optionalProperties;
        tmpCard->anniversary= NULL;
        tmpCard->birthday = NULL;

        //Start parsing the file
        while (cardValid){
            if (unfold(fileptr, buffer, &endoffile)){
                // printf("%s\n",buffer);
                if(endoffile)
                    break;
                res = parseLine(buffer,&state, tmpCard);
                if(res == INV_CARD || res == INV_PROP)
                    cardValid = false;
            }else{
                cardValid = false;
            }
        }

        if(state != CLOSE)
            cardValid = false;

        fclose(fileptr);
        
        if (!cardValid){
            deleteCard(tmpCard);
            *newCardObject = NULL;
            if(res == OK)
                return INV_CARD;
            return res;
        }

        *newCardObject = tmpCard;
        return OK;
    }else{
        *newCardObject = NULL;
        return INV_FILE;
    }      
}

void deleteCard(Card* obj){
	
	if (obj == NULL)
		return;
	
    if(obj->fn){
        deleteProperty(obj->fn);
    }
	if(obj->optionalProperties){
        freeList(obj->optionalProperties);
    }
    if(obj->birthday){
        deleteDate(obj->birthday);
    }
    if(obj->anniversary){
        deleteDate(obj->anniversary);
    }
	free(obj);
}

/***
 * 
 * ASSIGNMENT 2 FUNCTIONS
 * 
 ***/

VCardErrorCode writeCard(const char* fileName, const Card* obj){
    FILE *filePtr;
    int charsWritten;

    if(obj == NULL){
        return WRITE_ERROR;
    }

    // I open the file overwriting a possible previous file with the same name
    // see TA answer: https://moodle.socs.uoguelph.ca/mod/forum/discuss.php?d=1903#p3992
    filePtr = fopen(fileName, "w+");

    if(filePtr == NULL){
        return WRITE_ERROR;
    }else{
        char *groupString, *nameString, *valuesString, *parametersString;
        ListIterator iter;

        groupString = NULL;
        nameString = NULL;
        valuesString = NULL;
        parametersString = NULL;

        fprintf(filePtr, "BEGIN:VCARD\r\n");
        fprintf(filePtr, "VERSION:4.0\r\n");

        // Print FN prop
        writeCardPrintProperty(obj->fn, &groupString, &nameString, &parametersString, &valuesString);

        charsWritten = fprintf(filePtr,"%s%s%s:%s\r\n",groupString, nameString, parametersString, valuesString);
        if(charsWritten < 0){
            fclose(filePtr);
            return WRITE_ERROR;
        }

        free(groupString);
        free(nameString);
        free(valuesString);
        free(parametersString);

        // Print birthday and anniversary
        if(obj->anniversary != NULL){
            DateTime *anniversary = obj->anniversary;
            if(anniversary->isText){
                charsWritten = fprintf(filePtr,"ANNIVERSARY;VALUE=text:%s\r\n", anniversary->text);
            }else{
                int utc = anniversary->UTC?1:0;
                int timeNotNull = strlen(anniversary->time)>0?1:0;
                int len = strlen(anniversary->date) + strlen(anniversary->time) + 1 + utc + timeNotNull;
                char *tmpStr = (char*)malloc(sizeof(char)*len);
                
                if(timeNotNull > 0 ){
                    sprintf(tmpStr, "%sT%s", anniversary->date, anniversary->time);
                }else{
                    sprintf(tmpStr, "%s", anniversary->date);
                }

                if(utc > 0)
                    strcat(tmpStr,"Z");

                charsWritten = fprintf(filePtr,"ANNIVERSARY:%s\r\n", tmpStr);
                free(tmpStr);
                if(charsWritten < 0){
                    fclose(filePtr);
                    return WRITE_ERROR;
                }
            }
        }

        if(obj->birthday != NULL){
            DateTime *birthday = obj->birthday;
            if(birthday->isText){
                charsWritten = fprintf(filePtr,"BDAY;VALUE=text:%s\r\n", birthday->text);
            }else{
                int utc = birthday->UTC?1:0;
                int timeNotNull = strlen(birthday->time)>0?1:0;
                int len = strlen(birthday->date) + strlen(birthday->time) + 1 + utc + timeNotNull;
                char *tmpStr = (char*)malloc(sizeof(char)*len);
                
                if(timeNotNull > 0 ){
                    sprintf(tmpStr, "%sT%s", birthday->date, birthday->time);
                }else{
                    sprintf(tmpStr, "%s", birthday->date);
                }

                charsWritten = fprintf(filePtr,"BDAY:%s\r\n", tmpStr);
                free(tmpStr);
                if(charsWritten < 0){
                    fclose(filePtr);
                    return WRITE_ERROR;
                }
            }
        }
    
        //print optionalProperties
        iter = createIterator(obj->optionalProperties);
        void* elem;
        while((elem = nextElement(&iter)) != NULL){
            writeCardPrintProperty(elem, &groupString, &nameString, &parametersString, &valuesString);

            charsWritten = fprintf(filePtr,"%s%s%s:%s\r\n",groupString, nameString, parametersString, valuesString);
            if(charsWritten < 0){
                return WRITE_ERROR;
            }

            free(groupString);
            free(nameString);
            free(valuesString);
            free(parametersString);
        }

        fprintf(filePtr, "END:VCARD\r\n");

        fclose(filePtr);
    }

    return OK;
}

VCardErrorCode validateCard(const Card* obj){
    VCardErrorCode result;
    bool kind, n, gender, prodID, rev, group, member;

    kind = n = gender = prodID = rev = group = member = false;

    if(obj == NULL)
        return INV_CARD;
    
    // check for fn prop to exists and be valid
    if(obj->fn == NULL){
        return INV_CARD;
    }else{
        result = validateFN(obj->fn);
        if(result != OK){
            return result;
        }
    }

    //check that optionalParameters exists
    if(obj->optionalProperties == NULL){
        return INV_CARD;
    }else{
        ListIterator iter = createIterator(obj->optionalProperties);

        void* elem;
        while((elem = nextElement(&iter)) != NULL){
            result = validateProperty((Property *)elem, &kind, &n, &gender, &prodID, &rev, &group, &member); // validateProperty must check for some prop cardinality, how to?
            if(result != OK){
                return result;
            }
        }
    }

    // check that if birthday and anniversary exist are valid
    if(obj->anniversary != NULL){
        result = validateDateTime(obj->anniversary);
        if(result != OK){
            return result;
        }
    }

    if(obj->birthday != NULL){
        result = validateDateTime(obj->birthday);
        if(result != OK){
            return result;
        }
    }

    if(member && !group)
        return INV_PROP;


    return OK;
}

char* strListToJSON(const List* strList){
    char *output = NULL;

    if (strList == NULL)
        return NULL;

    output = malloc(sizeof(char) * 3);
    output[0] = '[';
    output[1] = '\0';

    ListIterator iter = createIterator((List*)strList);

    void* elem;
    bool first = true;
    while((elem = nextElement(&iter)) != NULL){
        char* currDescr = strList->printData(elem);
        char *escaped = escapeCharacter(currDescr, '"');

        int newLen = strlen(output)+50+strlen(escaped);
        output = (char*)realloc(output, newLen);

        if( !first ){
            strcat(output, ",");
        }else{
            first = false;
        }

        strcat(output, escaped);

        free(escaped);
        free(currDescr);
    }

    int length = strlen(output);
    output[length] = ']';
    output[length+1] = '\0';

    return output;
    
}

List* JSONtoStrList(const char* str){
    bool parsed = true;
    bool escaped = false;
    bool started = false;
    bool comma = false;
    bool extract = false;
    bool next = false;

    int valueStart = 0;
    int valueEnd = 0;

    if(str == NULL)
        return NULL;

    List *list = initializeList(&printValue,&deleteValue,&compareValues);
    int length = strlen(str);

    // printf("\n%d",length);

    for(int i = 0; i < length && parsed; i++){
        char c = str[i];
        
        // printf("\n%d %c",i,c);

        if(i == 0 && c != '['){
            parsed = false;
        }else if(i == length-1 && c != ']'){
            parsed = false;
        }else if(i == length-1 && c == ']'){
            parsed = true;
        }else if( i == 1 && c == '"'){
            started = true;
            valueStart = i;
        }else if(i == 1 && c != '"'){
            parsed = false;
        }else if( i > 1){
            
            if( !started && c != ',' && !next){
                parsed = false;
            }else if( !started && c == ',' && !next){
                comma = true;
                next = true;
            }else if(!started && comma && c == '"'){
                comma = false;
                started = true;
                next = false;
                valueStart = i;
            }else if(!started && comma && c != '"'){
                parsed = false;
            }else if(started && c == '\\'){
                escaped = true;
            }else if(started && escaped && c == '"'){
                escaped = false;
            }else if(started && c == '"'){
                started = false;
                extract = true;
                valueEnd = i;
            }

        }

        if(!parsed)
            break;

        if(extract){
            char *value = unescapeString((char*)str,valueStart,valueEnd);
            // printf("\n%s\n",value);
            insertBack(list,(void*)value);
            extract = false;
        }
    }

    if(!parsed){
        freeList(list);
        return NULL;
    }


    return list;
}

char* propToJSON(const Property* prop){
    char *output;
    bool valid = true;

    if(prop == NULL || prop->group == NULL || prop->name == NULL || prop->values == NULL)
        valid = false;
        

    if(valid == true){
        char *valuesStr = strListToJSON(prop->values);
        int len = strlen(prop->group) + strlen(prop->name) + strlen(valuesStr) + 33;
        output = malloc(sizeof(char) * len);

        sprintf(output,"{\"group\":\"%s\",\"name\":\"%s\",\"values\":%s}",prop->group,prop->name,valuesStr);
        free(valuesStr);

    }else{
        output = malloc(sizeof(char) * 1);
        output[0] = 0;
    }

    return output;
}

Property* JSONtoProp(const char* str){
    bool parsed = true;
    bool escaped = false;
    bool started = false;
    bool comma = false;
    bool extract = false;
    bool next = false;

    bool done = false;

    bool colon = false;
    bool colonRequired = true;
    bool commaRequired = false;

    bool groupExpected = true;
    bool nameExpected = false;
    bool valuesExpected = false;
    bool extractValues = false;
    bool extractGroup = false;
    bool extractName = false;

    int valueStart = 0;
    int valueEnd = 0;

    if(str == NULL)
        return NULL;

    Property *prop=NULL;
    List* parameters;
    List* list = NULL;

    prop = malloc(sizeof(Property));
    parameters = initializeList(&printParameter, &deleteParameter, &compareParameters);
    prop->parameters = parameters;
    prop->values = list;
    prop->name = NULL;
    prop->group = NULL;

    int length = strlen(str);

    // printf("\n%d",length);

    for(int i = 0; i < length && parsed; i++){
        char c = str[i];

        // printf("\n%d %c",i,c);

        if(i == 0 && c != '{'){
            parsed = false;
        }else if(i == length-1 && c != '}'){
            parsed = false;
        }else if(i == length-1 && c == '}'){
            parsed = true;
            done = true;
        }else if( i == 1 && c == '"'){
            started = true;
            valueStart = i;
        }else if(i == 1 && c != '"'){
            parsed = false;
        }else if( i > 1){

            if(extractValues){
                if(i == length-1 && (c != '}' || c != ':')){
                    parsed = false;
                }else{
                    valueStart = i;
                    valueEnd = length-1;
                    extract = true;
                }
            }else{
                // printf("\nstarted %d c %c next %d colonRequired %d\n",started,c,next,colonRequired);
                if( !started && c != ',' && c != ':' && !next){
                    parsed = false;
                }
                else if( !started && c == ':' && !next && colonRequired){
                    colon = true;
                    next = true;
                }else if( !started && colon && c == '"' && colonRequired){
                    colon = false;
                    started = true;
                    next = false;
                    valueStart = i;
                    colonRequired = false;
                    commaRequired = true;
                }else if( !started && c == ',' && !next && commaRequired){
                    comma = true;
                    next = true;
                }else if(!started && comma && c == '"'){
                    comma = false;
                    started = true;
                    next = false;
                    valueStart = i;
                    commaRequired = false;
                    colonRequired = true;
                }else if(!started && comma && c != '"'){
                    parsed = false;
                }else if(started && c == '\\'){
                    escaped = true;
                }else if(started && escaped && c == '"'){
                    escaped = false;
                }else if(started && c == '"'){
                    started = false;
                    extract = true;
                    valueEnd = i;
                }
            }
        }

        if(!parsed)
            break;

        if(extract){
            char *value = unescapeString((char*)str,valueStart,valueEnd);

            if(groupExpected){
                if(strcmp(value,"group") != 0)
                    parsed = false;
                else{
                    groupExpected = false;
                    extractGroup = true;
                }
            }else if(nameExpected){
                if(strcmp(value,"name") != 0)
                    parsed = false;
                else{
                    nameExpected = false;
                    extractName = true;
                }
            }else if(valuesExpected){
                if(strcmp(value,"values") != 0)
                    parsed = false;
                else{
                    valuesExpected = false;
                    extractValues = true;
                }
            }else if(extractGroup){
                prop->group = malloc(sizeof(char) *(strlen(value)+1));
                strcpy(prop->group,value);

                extractGroup = false;
                nameExpected = true;
            }else if(extractName){
                prop->name = malloc(sizeof(char) *(strlen(value)+1));
                strcpy(prop->name,value);

                extractName = false;
                valuesExpected = true;
            }else if(extractValues){
                char *substr = NULL;
                substring((char*)str,&substr,valueStart+1,valueEnd);
                list = JSONtoStrList(substr);
                // printf("\nsottostringa %s\n",substr);
                if(list == NULL){
                    parsed = false;
                }
                free(substr);
                free(value);

                done = true;
                break;
            }
            free(value);
            extract = false;
        }
    }

    if(!parsed || !done){
        deleteProperty(prop);
        return NULL;
    }
    
    prop->values = list;

    return prop;
}

char* dtToJSON(const DateTime* prop){
    char *output;
    bool valid = true;

    if(prop == NULL)
        valid = false;

    if(valid){
        char *isTextValue;
        char *isUTCValue;

        if(prop->isText)
            isTextValue = "true";
        else
            isTextValue = "false";
        
        if(prop->UTC)
            isUTCValue = "true";
        else
            isUTCValue = "false";

        int len = strlen(prop->date) + strlen(prop->time) + strlen(prop->text) 
            + strlen(isTextValue) + strlen(isUTCValue) + 100;

        output = malloc(sizeof(char) * len);

        sprintf(output,"{\"isText\":%s,\"date\":\"%s\",\"time\":\"%s\",\"text\":\"%s\",\"isUTC\":%s}",
            isTextValue, prop->date, prop->time, prop->text, isUTCValue);

    }else{
        output = malloc(sizeof(char) * 1);
        output[0] = '\0';
    }

    return output;
}

DateTime* JSONtoDT(const char* str){
    bool parsed = true;
    bool escaped = false;
    bool started = false;
    bool comma = false;
    bool extract = false;
    bool next = false;

    bool done = false;

    bool colon = false;
    bool colonRequired = true;
    bool commaRequired = false;

    bool isTextExpected = true;
    bool dateExpected = false;
    bool timeExpected = false;
    bool textExpected = false;
    bool isUTCExpected = false;
    bool extractIsText = false;
    bool extractDate = false;
    bool extractTime = false;
    bool extractText = false;
    bool extractIsUTC = false;


    int valueStart = 0;
    int valueEnd = 0;

    DateTime *date=NULL;

    bool isText = false;
    bool isUTC = false;
    char *dateStr = NULL;
    char *timeStr = NULL;
    char *textStr = NULL;
    

    if(str == NULL)
        return NULL;

    int length = strlen(str);

    // printf("\n%d",length);

    for(int i = 0; i < length && parsed; i++){
        char c = str[i];

        // printf("\n%d %c",i,c);

        if(i == 0 && c != '{'){
            parsed = false;
        }else if(i == length-1 && c != '}'){
            parsed = false;
        }else if(i == length-1 && c == '}'){
            parsed = true;
            valueEnd = i;
            extract = true;
        }else if( i == 1 && c == '"'){
            started = true;
            valueStart = i;
        }else if(i == 1 && c != '"'){
            parsed = false;
        }else if( i > 1){

            // printf("\nstarted %d c %c next %d colonRequired %d\n",started,c,next,colonRequired);
            if( !started && c != ',' && c != ':' && !next){
                parsed = false;
            }
            else if( !started && c == ':' && !next && colonRequired){
                colon = true;
                next = true;
            }else if( !started && colon && (c == '"' || extractIsText || extractIsUTC) && colonRequired){
                colon = false;
                started = true;
                next = false;
                valueStart = i;
                colonRequired = false;
                commaRequired = true;
            }else if( !started && c == ',' && !next && commaRequired){
                comma = true;
                next = true;
            }else if(!started && comma && c == '"'){
                comma = false;
                started = true;
                next = false;
                valueStart = i;
                commaRequired = false;
                colonRequired = true;
            }else if(!started && comma && c != '"'){
                parsed = false;
            }else if(started && c == '\\'){
                escaped = true;
            }else if(started && escaped && c == '"'){
                escaped = false;
            }else if(started && c == '"'){
                // printf("\nentrato\n");
                started = false;
                extract = true;
                valueEnd = i;
            }else if(started && c == ',' && extractIsText){
                started = false;
                valueEnd = i;
                comma = true;
                next = true;
                extract = true;
            }
        }

        // printf("\n started %d c %c comma %d colon %d commaReq %d colonReq %d extract %d\n", started, c, comma, colon, commaRequired, colonRequired, extract);

        if(!parsed)
            break;

        if(extract){
            char *value = unescapeString((char*)str,valueStart,valueEnd);

            if(isTextExpected){
                // printf("\nisTextExpected\n");
                if(strcmp(value,"isText") != 0)
                    parsed = false;
                else{
                    isTextExpected = false;
                    extractIsText = true;
                }
            }else if(dateExpected){
                // printf("\ndateExpected\n");
                if(strcmp(value,"date") != 0)
                    parsed = false;
                else{
                    dateExpected = false;
                    extractDate = true;
                }
            }else if(timeExpected){
                // printf("\ntimExpected\n");
                if(strcmp(value,"time") != 0)
                    parsed = false;
                else{
                    timeExpected = false;
                    extractTime = true;
                }
            }else if(textExpected){
                // printf("\ntextExpected\n");
                if(strcmp(value,"text") != 0)
                    parsed = false;
                else{
                    textExpected = false;
                    extractText = true;
                }
            }else if(isUTCExpected){
                // printf("\nUTCExpected\n");
                if(strcmp(value,"isUTC") != 0)
                    parsed = false;
                else{
                    isUTCExpected = false;
                    extractIsUTC = true;
                }
            }else if(extractIsText){
                //extract text
                char *substr = NULL;
                substring((char*)str,&substr,valueStart,valueEnd);
                // printf("\nsottostringa %s\n",substr);
                if(strcmp(substr, "true") == 0)
                    isText = true;
                else if(strcmp(substr, "false") == 0)
                    isText = false;
                else{
                    parsed = false;
                    // printf("\nfalse\n");
                }
                
                free(substr);
                extractIsText = false;
                dateExpected = true;

            }else if(extractDate){
                // printf("\nextractDate\n");
                dateStr = malloc(sizeof(char) * (strlen(value) +1));
                strcpy(dateStr,value);

                extractDate = false;
                timeExpected = true;
            }else if(extractTime){
                timeStr = malloc(sizeof(char) * (strlen(value) +1));
                strcpy(timeStr,value);

                extractTime = false;
                textExpected = true;
            }else if(extractText){
                textStr = malloc(sizeof(char) * (strlen(value) +1));
                strcpy(textStr,value);

                extractText = false;
                isUTCExpected = true;
            }else if(extractIsUTC){
                //extract text
                char *substr = NULL;
                substring((char*)str,&substr,valueStart,valueEnd);
                // printf("\nsottostringa %s\n",substr);
                if(strcmp(substr, "true") == 0)
                    isUTC = true;
                else if(strcmp(substr, "false") == 0)
                    isUTC = false;
                else
                    parsed = false;

                free(substr);
                extractIsUTC = false;
                done = true;
            }
            free(value);
            extract = false;
        }
    }

    if( !parsed || !done){
        free(timeStr);
        free(dateStr);
        free(textStr);
        return NULL;
    }

    if(done && (strlen(dateStr) > 8 || strlen(timeStr) > 6)){
        free(timeStr);
        free(dateStr);
        free(textStr);
        return NULL;
    }

    date = malloc(sizeof(DateTime) + (strlen(textStr) + 1));
    strcpy(date->date, dateStr);
    strcpy(date->time, timeStr);
    strcpy(date->text, textStr);
    free(dateStr);
    free(timeStr);
    free(textStr);
    date->isText = isText;
    date->UTC = isUTC;

    return date;
}

Card* JSONtoCard(const char* str){
    bool parsed = true;
    bool escaped = false;
    bool started = false;
    bool colon = false;
    bool extract = false;
    bool next = false;
    bool fn = false;
    bool done = false;

    int valueStart = 0;
    int valueEnd = 0;

    if(str == NULL)
        return NULL;

    Card *card;
    card = malloc(sizeof(Card));
    card->fn= NULL;
    card->optionalProperties = initializeList(&printProperty,&deleteProperty,&compareProperties);
    card->anniversary= NULL;
    card->birthday = NULL;

    Property *p = malloc(sizeof(Property));
    p->parameters = initializeList(&printParameter, &deleteParameter, &compareParameters);
    p->values = initializeList(&printValue, &deleteValue, &compareValues);
    p->name = NULL;
    p->group = malloc(sizeof(char));
    p->group[0] = '\0';

    card->fn = p;

    int length = strlen(str);

    // printf("\n%d",length);

    for(int i = 0; i < length && parsed; i++){
        char c = str[i];
        
        // printf("\n%d %c",i,c);

        if(i == 0 && c != '{'){
            parsed = false;
        }else if(i == length-1 && c != '}'){
            parsed = false;
        }else if(i == length-1 && c == '}'){
            parsed = true;
        }else if( i == 1 && c == '"'){
            started = true;
            valueStart = i;
        }else if(i == 1 && c != '"'){
            parsed = false;
        }else if( i > 1){
            
            if( !started && c != ':' && !next){
                parsed = false;
            }else if( !started && c == ':' && !next){
                colon = true;
                next = true;
            }else if(!started && colon && c == '"'){
                colon = false;
                started = true;
                next = false;
                valueStart = i;
            }else if(!started && colon && c != '"'){
                parsed = false;
            }else if(started && c == '\\'){
                escaped = true;
            }else if(started && escaped && c == '"'){
                escaped = false;
            }else if(started && c == '"'){
                started = false;
                extract = true;
                valueEnd = i;
            }

        }

        if(!parsed)
            break;

        if(extract){
            char *value = unescapeString((char*)str,valueStart,valueEnd);
            // printf("\n%s\n",value);

            if(!fn){
                char *lowered = malloc(sizeof(char) * (strlen(value) +1 ));
                strcpy(lowered,value);
                stolower(lowered);

                if(strcmp(lowered,"fn") == 0 ){
                    fn = true;
                    card->fn->name = malloc(sizeof(char) * (strlen(value) +1 ));
                    strcpy(card->fn->name,value);
                }else{
                    parsed = false;
                    
                }

                free(lowered);
                free(value);
            }else{ //value
                insertBack(card->fn->values,(void*)value);
                done = true;
            }
            extract = false;
        }
    }

    if(!parsed || !done){
        deleteCard(card);
        return NULL;
    }

    return card;
}

void addProperty(Card* card, const Property* toBeAdded){
    if(card == NULL || toBeAdded == NULL)
        return;

    if(card->optionalProperties == NULL)
        return;
    
    insertBack(card->optionalProperties, (void*)toBeAdded);
    return;
}

/***
 * 
 * ASSIGNMENT 3 - NodeJS helper functions
 *
 ***/

char* cardToJSON(Card *obj){
    char *output = NULL;
    char *fn = NULL;
    char *anniversary = NULL;
    char *birthday = NULL;

    if (obj == NULL || obj->fn == NULL || obj->optionalProperties == NULL){
        output = malloc(sizeof(char) * 1);
        output[0] = '\0';
        return output;
    }

    fn = propToJSON(obj->fn);
    if(obj->anniversary == NULL){
        anniversary = malloc(sizeof(char) * 3);
        sprintf(anniversary,"{}");
    }else{
        anniversary = dtToJSON(obj->anniversary);
    }

    if(obj->birthday == NULL){
        birthday = malloc(sizeof(char) * 3);
        sprintf(birthday,"{}");
    }else{
        birthday = dtToJSON(obj->birthday);
    }
    
    output = malloc(sizeof(char) * (strlen(fn) + strlen(anniversary) + strlen(birthday) + 60));
    sprintf(output,"{\"fn\":%s,\"anniversary\":%s,\"birthday\":%s,\"optionalProperties\":[",fn,anniversary,birthday);
    free(fn);
    free(anniversary);
    free(birthday);


    ListIterator iter = createIterator((List*)(obj->optionalProperties));

    void* elem;
    bool first = true;
    while((elem = nextElement(&iter)) != NULL){

        char* currDescr = propToJSON((Property*)elem);
        // char *escaped = escapeCharacter(currDescr, '"');

        int newLen = strlen(output)+50+strlen(currDescr);
        output = (char*)realloc(output, newLen);

        if( !first ){
            strcat(output, ",");
        }else{
            first = false;
        }

        strcat(output, currDescr);

        free(currDescr);
    }

    int length = strlen(output);
    output[length] = ']';
    output[length+1] = '}';
    output[length+2] = '\0';

    return output;
}


char* getSummaryFromFile(char *fileName){
    VCardErrorCode err;

    char *filePath;
    char *individualsName = NULL;
    char *JSONResponse;

    int additionalProperties = 0;

    Card *objCard = malloc(sizeof(Card));
    
    
    if(fileName == NULL){
        err = INV_FILE;
        JSONResponse = malloc(sizeof(char) * (numPlaces(err) + 20));
        sprintf(JSONResponse, "{\"status\":%d}", err);
        deleteCard(objCard);
        return JSONResponse;
    }

    filePath = malloc(sizeof(char) * ((strlen(fileName) + 9)));
    strcpy(filePath,"uploads/");
    // filePath[8] = 0;
    strcat(filePath,fileName);

    err = createCard(filePath, &objCard);
    free(filePath);
    
    if(err != OK){
        JSONResponse = malloc(sizeof(char) * (numPlaces(err) + 20));
        sprintf(JSONResponse, "{\"status\":%d}", err);
        deleteCard(objCard);
        return JSONResponse;
    }

    err = validateCard(objCard);
    if(err != OK){
        JSONResponse = malloc(sizeof(char) * (numPlaces(err) + 20));
        sprintf(JSONResponse, "{\"status\":%d}", err);
        deleteCard(objCard);
        return JSONResponse;
    }


    individualsName = propToJSON(objCard->fn);
    
    additionalProperties = objCard->optionalProperties->length;

    if(objCard->anniversary != NULL)
        additionalProperties++;
    
    if(objCard->birthday != NULL)
        additionalProperties++;

    // {individual:string,additional:num}
    JSONResponse = malloc(sizeof(char) * (strlen(individualsName) + numPlaces(additionalProperties) + numPlaces(err) + 65));
    sprintf(JSONResponse, "{\"status\":%d,\"individual\":%s,\"additional\":%d}", err , individualsName, additionalProperties);

    free(individualsName);
    deleteCard(objCard);
    return JSONResponse;


} 

char* getCardDetails(char *fileName){
    VCardErrorCode err;

    char *filePath;
    char *JSONResponse;

    Card *objCard = malloc(sizeof(Card));


    if(fileName == NULL){
        err = INV_FILE;
        JSONResponse = malloc(sizeof(char) * (numPlaces(err) + 20));
        sprintf(JSONResponse, "{\"status\":%d}", err);
        deleteCard(objCard);
        return JSONResponse;
    }

    filePath = malloc(sizeof(char) * ((strlen(fileName) + 9)));
    strcpy(filePath,"uploads/");
    // filePath[8] = 0;
    strcat(filePath,fileName);

    err = createCard(filePath, &objCard);
    free(filePath);

    if(err != OK){
        JSONResponse = malloc(sizeof(char) * (numPlaces(err) + 20));
        sprintf(JSONResponse, "{\"status\":%d}", err);
        deleteCard(objCard);
        return JSONResponse;
    }

    err = validateCard(objCard);
    if(err != OK){
        JSONResponse = malloc(sizeof(char) * (numPlaces(err) + 20));
        sprintf(JSONResponse, "{\"status\":%d}", err);
        deleteCard(objCard);
        return JSONResponse;
    }

    char *cardJSON = cardToJSON(objCard);
    JSONResponse = malloc(sizeof(char) * (strlen(cardJSON) + 30));
    sprintf(JSONResponse,"{\"status\":%d,\"card\":%s}",err,cardJSON);

    free(cardJSON);
    deleteCard(objCard);
    return JSONResponse;
}