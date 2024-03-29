#include <stdio.h>
#include <stdlib.h>
#include "utstring.h"
#include "utarray.h"
#include "Model.h"

void CharProfile_free(void * profile_in){
	CharProfile * profile = (CharProfile *) profile_in;
	utarray_free(profile->CharChoices);
}



UT_string * postProcessing(UT_array * charList)
{
  UT_string * temp;
  UT_string * output;
  CharProfile * currCharProfile = NULL;
  CharProbability * currCharProbability = NULL;
  int percentage = 0;
  char chosen = 'a';
  char quotation = 'a';
  char *tempCharacter;
  
  utstring_new(output);
  
  while((currCharProfile = (CharProfile *) utarray_next(charList, currCharProfile)))
  {
    
    currCharProbability = NULL;
    percentage = 0;
    
    while((currCharProbability = (CharProbability *) utarray_next(currCharProfile->CharChoices, currCharProbability)))
    {
      /* found a character with a higher percentage */
      if (percentage < currCharProbability->Probability)
      {
	percentage = currCharProbability->Probability;
	chosen = currCharProbability->Char;
	
	/* it is 100% why bother searching for a a higher percentage */
	if (percentage == 100)
	{
	  break;
	}
      }
    }
    /* we had a problem with identifying " so this if statement should take care of it */
    /* instead of identifying ", we instead, identify two ' ' */
    if (chosen == '\'')
    {
      currCharProfile = (CharProfile *) utarray_next(charList, currCharProfile);
      while((currCharProbability = (CharProbability *) utarray_next(currCharProfile->CharChoices, currCharProbability)))
      {
	/* found a character with a higher percentage */
	if (percentage < currCharProbability->Probability)
	{
	  percentage = currCharProbability->Probability;
	  quotation = currCharProbability->Char;
	  
	  /* it is 100% why bother searching for a a higher percentage */
	  if (percentage == 100)
	  {
	    break;
	  }
	}
      }
      if (quotation == '\'')
      {
	chosen = '"';
      }
      else
      {
	tempCharacter = &chosen;
	strcat(tempCharacter, &quotation);
      }
    }
    
    /* adding the chosen character to a temp string */
    utstring_new(temp);
    utstring_printf(temp, "%c", chosen);
    
    /* concat the temp character to the real output string */
    utstring_concat(output, temp);
  }
    
  return output;
}


void DisplayCharProfile(CharProfile * input){
	
		CharProbability * Curr2 = (CharProbability *) utarray_front(input->CharChoices);
		while (Curr2)
		{
			printf("Chance of %c is %d percent.\n", Curr2->Char, Curr2->Probability);
			Curr2 = (CharProbability*)utarray_next(input->CharChoices, Curr2);
		}
	
	
	
}

UT_string * postProcessingAdvance(UT_array * charList, UT_array * dictionary, UT_array * specialCharacter)
{
  CharProfile * currCharProfile = NULL;
  /*CharProfile * currCharProfile2 = NULL;*/
  CharProbability * currCharProbability = NULL;
  char chosen = 'a';
  char quotation = 'a';
  
  int isSpecialChar = 0;
  
  UT_array * wordBank;
  UT_string *temp;
  /*UT_string *temp2;*/
  UT_string *output;
  
  utstring_new(output);
  utstring_new(temp);

  currCharProfile = (CharProfile *) utarray_next(charList, currCharProfile);
  while(currCharProfile)
  {
	  /*Quan add this part for 100% prob character*/
	  CharProbability * currCharProb =(CharProbability *) utarray_front(currCharProfile->CharChoices);
	  if (currCharProb->Probability == 100){
		utstring_clear(temp);
		utstring_printf(temp, "%c", currCharProb->Char);
		utstring_concat(output, temp);
		currCharProfile = (CharProfile *) utarray_next(charList, currCharProfile);
		continue;
	  }
	  
	  
    currCharProbability = NULL;
    utarray_new(wordBank, &ut_str_icd);
    wordBank = getThreeKeyword(currCharProfile, charList, specialCharacter);
    isSpecialChar = 0;
    /* highest posibility isn't a special character */
    while ((isSpecialChar == 0) && currCharProfile)
    {
      utstring_clear(temp);
	
	currCharProbability = NULL;	/*Quan put here to solve Seg fault*/
      currCharProbability = (CharProbability *) utarray_next(currCharProfile->CharChoices, currCharProbability);	
#ifdef DEBUG
	printf("look at spot %ld index %ld\n", utarray_eltidx(charList, currCharProfile), utarray_eltidx(currCharProfile->CharChoices, currCharProbability));
	DisplayCharProfile(currCharProfile);
#endif
      chosen = currCharProbability->Char;
      utstring_printf(temp, "%c", chosen);
      isSpecialChar = compareChar(temp, specialCharacter);
      /*if (isSpecialChar)
      {
	(currCharProfile = (CharProfile *) utarray_next(charList, currCharProfile));
      }*/
      (currCharProfile = (CharProfile *) utarray_next(charList, currCharProfile));
    }
    utstring_concat(output, wordCompare(wordBank, dictionary));
    if (isSpecialChar == 1)
    {
      if (chosen == '\'')
      {
	if ((currCharProfile = (CharProfile *) utarray_next(charList, currCharProfile)))
	{
	currCharProbability = NULL;	/*Quan put here to solve Seg fault*/
	  currCharProbability = (CharProbability *) utarray_next(currCharProfile->CharChoices, currCharProbability);
	  utstring_clear(temp);
	  quotation = currCharProbability->Char;
	  utstring_printf(temp, "%c", quotation);
	  if (quotation == '\'')
	  {
	    chosen = '"';
	    utstring_clear(temp);
	    utstring_printf(temp, "%c", chosen);
	  }
	  utstring_concat(output, temp);
	}
      }
      else
      {
	utstring_clear(temp);
	utstring_printf(temp, "%c", chosen);
	utstring_concat(output, temp);
      }
    }
    
  }
    
#ifdef DEBUG
    printf("output is: %s\n", utstring_body(output));
#endif
    
    utstring_free(temp);
  return output;
}


UT_array * getThreeKeyword(CharProfile *currCharProfile, UT_array * charList, UT_array * specialCharacter)
{
  char * top;
  char * top2;
  char * second;
  CharProfile * tempPosition = currCharProfile;
  CharProfile * tempPosition2 = currCharProfile;
  char chosen;
  char chosen2;
  UT_string *temp;
  UT_string *temp2;
  UT_string *output;

  UT_array * wordBank;
  utarray_new(wordBank, &ut_str_icd);

  int i, j;
  int wordLength = 8;
  int wordLengthFlag = 0;
  int specialCharFlag = 0;
  

  /* doesn't grab the highest probability.... */
  for (i = 0; i < wordLength; i++)
  {
    utstring_new(output);
    top = getTopProb(currCharProfile);
    second = getSecondProb(currCharProfile);
    tempPosition = currCharProfile;
    j = 0;
    while (tempPosition && j < 8)
    {
      if (j > 0)
      {
	tempPosition = (CharProfile *)utarray_next(charList, tempPosition);
	if (tempPosition)
	{
	  top = getTopProb(tempPosition);
	  second = getSecondProb(tempPosition);
	}
      }
      if (j == i)
      {
	chosen = *second;
      }
      else
      {
	chosen = *top;
      }
      utstring_new(temp);
      utstring_printf(temp, "%c", chosen);
      specialCharFlag = compareChar(temp, specialCharacter);
      
      if (specialCharFlag == 0)
      {
	utstring_concat(output,temp);
      }
      else
      {
	break;
      }
      j++;
    }
    if (wordLengthFlag == 0)
    {
      wordLength = j;
      wordLengthFlag = 1;
    }
    utarray_push_back(wordBank, &utstring_body(output));
  }
  
  
  /* grabbing highest prob */
  utstring_new(output);
  i = 0;
  while (i < wordLength && tempPosition2)
  {
    utstring_new(temp2);
    top2 = getTopProb(tempPosition2);
    chosen2 = *top2;
    tempPosition2 = (CharProfile *)utarray_next(charList, tempPosition2);
    utstring_printf(temp2, "%c", chosen2);
    utstring_concat(output, temp2);
    i++;
  } 
  utarray_push_back(wordBank, &utstring_body(output));
  currCharProfile = tempPosition;
  
  return wordBank;
}

char * getTopProb(CharProfile * currCharProfile)
{
  CharProbability * currCharProbability = NULL;
  currCharProbability = (CharProbability *) utarray_next(currCharProfile->CharChoices, currCharProbability);
  return &currCharProbability->Char;
}
char * getSecondProb(CharProfile * currCharProfile)
{
	CharProbability * currCharProbability = NULL;
	currCharProbability = (CharProbability *) utarray_next(currCharProfile->CharChoices, currCharProbability);
	currCharProbability = (CharProbability *) utarray_next(currCharProfile->CharChoices, currCharProbability);
	return &currCharProbability->Char;
}

int compareChar(UT_string *chosen, UT_array * specialCharacter)
{
  char ** specialChar = NULL;
  int i;
  
  UT_string * temp;
  
  for (i = 0; i < 32; i++)
  {
    specialChar = (char **)utarray_next(specialCharacter, specialChar);
    utstring_new(temp);
    utstring_printf(temp, "%c", *(*specialChar));
    
    if (strcmp(utstring_body(temp), utstring_body(chosen)) == 0)
    {
      return 1;
    }
  }
  return 0;
  
}

UT_string *wordCompare(UT_array * wordBank, UT_array * dictionary)
{
  char ** tempWord1 = NULL;
  char ** tempWord2 = NULL;
  
  int i;
  
  UT_string *temp1;
  UT_string *temp2;
  
  while((tempWord1 = (char**)utarray_next(wordBank, tempWord1)))
  {
    utstring_new(temp1);
    utstring_printf(temp1, "%s", (*tempWord1));
    tempWord2 = NULL;
    //printf("check please: %s\n", utstring_body(temp1));
    for (i = 0; i < 32; i++)
    {
      tempWord2 = (char **)utarray_next(dictionary, tempWord2);
      utstring_new(temp2);
      utstring_printf(temp2, "%s", (*tempWord2));
      
      if (strcmp(utstring_body(temp1), utstring_body(temp2)) == 0)
      {
	return temp1;
      }
    }
  }
  tempWord1 = NULL;
  while ((tempWord1 = (char**)utarray_next(wordBank,tempWord1)))
  {
    utstring_new(temp1);
    utstring_printf(temp1, "%s", (*tempWord1)); 
  }
  
  return temp1; 
}

UT_array * postProcessingInitializeSpecialChar()
{
  char *specialCharacter;

  UT_array *specialChar;
  utarray_new(specialChar, &ut_str_icd);

  // all the special characters
  specialCharacter = "`";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = "!";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = "@";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = "#";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = "$";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = "%";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = "^";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = "&";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = "*";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = "(";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = ")";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = "-";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = "_";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = "=";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = "+";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = "[";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = "{";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = "]";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = "}";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = "\\";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = "|";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = ";";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = ":";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = "'";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = "\"";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = ",";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = "<";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = ".";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = ">";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = "/";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = "?";
  utarray_push_back(specialChar, &specialCharacter);
  specialCharacter = "~";
  utarray_push_back(specialChar, &specialCharacter);
  
  return specialChar;
  
}

UT_array * postProcessingInitializeDictionary()
{
  char * word;
  /*char ** p;*/

  UT_array *dictionary;
  utarray_new(dictionary, &ut_str_icd);

  //all ansi c keyword 	
  word = "auto";
  utarray_push_back(dictionary, &word);
  word = "break";
  utarray_push_back(dictionary, &word);
  word = "case";
  utarray_push_back(dictionary, &word);
  word = "char";
  utarray_push_back(dictionary, &word);
  word = "const";
  utarray_push_back(dictionary, &word);
  word = "continue";
  utarray_push_back(dictionary, &word);
  word = "default";
  utarray_push_back(dictionary, &word);
  word = "do";
  utarray_push_back(dictionary, &word);
  word = "double";
  utarray_push_back(dictionary, &word);
  word = "else";
  utarray_push_back(dictionary, &word);
  word = "enum";
  utarray_push_back(dictionary, &word);
  word = "extern";
  utarray_push_back(dictionary, &word);
  word = "float";
  utarray_push_back(dictionary, &word);
  word = "for";
  utarray_push_back(dictionary, &word);
  word = "goto";
  utarray_push_back(dictionary, &word);
  word = "if";
  utarray_push_back(dictionary, &word);
  word = "int";
  utarray_push_back(dictionary, &word);
  word = "long";
  utarray_push_back(dictionary, &word);
  word = "register";
  utarray_push_back(dictionary, &word);
  word = "return";
  utarray_push_back(dictionary, &word);
  word = "short";
  utarray_push_back(dictionary, &word);
  word = "signed";
  utarray_push_back(dictionary, &word);
  word = "sizeof";
  utarray_push_back(dictionary, &word);
  word = "static";
  utarray_push_back(dictionary, &word);
  word = "struct";
  utarray_push_back(dictionary, &word);
  word = "typedef";
  utarray_push_back(dictionary, &word);
  word = "union";
  utarray_push_back(dictionary, &word);
  word = "unsigned";
  utarray_push_back(dictionary, &word);
  word = "void";
  utarray_push_back(dictionary, &word);
  word = "volatile";
  utarray_push_back(dictionary, &word);
  word = "while";
  utarray_push_back(dictionary, &word);
  word = "switch";
  utarray_push_back(dictionary, &word);
  
  return dictionary;
  
}

void postProcessingCleanUP(UT_array * dictionary, UT_array * specialCharacter)
{
  utarray_free(dictionary);
  utarray_free(specialCharacter);
}

