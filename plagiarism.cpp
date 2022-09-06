//#define DEBUG

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <memory>

using namespace std;

const int Z_CAPITAL = 90;
const int A_CAPITAL = 65;
const int UPPERCASE_TO_LOWERCASE_DIFFERENCE = 32;
const int SHINGLE_SIZE = 3;
const int PREPOSITIONS_NUMBER = 52;
const int MAX_LENGTH = 8192;

char garbage[] = { ',', ';', '.', '!', '-', ' ', '?', ':', '&',
                  '_', '%', '$', '@', '^', '<', '>', '(', '\"',
                  ')', '[', ']', '\\', '*', '`', '~', '=', '-', '+',
                  '\t', '\n', '\v', '\f', '\f', '|', '}', '{', '#',
                  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' };

string prepositions[] = { "the", "with", "without", "ago", "apart", "aside", "away",
                         "hence", "notwithstanding", "over", "through", "withal",
                         "next", "beside", "under", "above", "across", "into",
                         "towards", "onto", "from", "since", "for", "past", "till",
                         "until", "along", "amid", "among", "around", "before", "behind",
                         "below", "beneath", "besides", "between", "but", "concerning",
                         "considering", "despite", "down", "during", "following", "inside",
                         "outside", "opposite", "regarding", "via", "versus", "underneath",
                         "unlike", "under" };

double antiPlagiarism(string, string);
void normalize(string, char*);
bool isGarbage(char);
int getStringLength(string);
void getWords(string, std::unique_ptr<string[]>&);
double compareShingles(std::unique_ptr<string[]>&, std::unique_ptr<string[]>&, int, int, int);
int countWordsInProcessedString(string);
bool strCmp(string, string);
char toLowerCase(char);
void removeDuplicates(std::unique_ptr<string[]>&, int);
int getSurvivedWordsNumber(std::unique_ptr<string[]>&, int);
bool isPreposition(string);
void removePrepositions(std::unique_ptr<string[]>&, int);
void clearOfGarbage(std::unique_ptr<string[]>&, std::unique_ptr<string[]>&, int);
bool isEmpty(string text);

double antiPlagiarism(string text, string fragment)
{
    double borrowingPercentage = 0.0;
    double shinglesMatches = 0.0;
    double uniqueness = 0.0;
    int shinglesToCheck = 0;
    int shinglesToCheckInText = 0;
    int textCharLength = getStringLength(text);
    int fragmentCharLength = getStringLength(fragment);
    int wordsInText = 0;
    int wordsInFragment = 0;
    int wordsSurvivedInText = 0;
    int wordsSurvivedInFragment = 0;
    string textTypeConversion;
    string fragmentTypeConversion;
    string singleLineCharArrayOfText;
    singleLineCharArrayOfText.resize(textCharLength + 1);
    string singleLineCharArrayOfFragment;
    singleLineCharArrayOfFragment.resize(fragmentCharLength + 1);

    normalize(text, const_cast<char*>(singleLineCharArrayOfText.c_str()));
    normalize(fragment, const_cast<char*>(singleLineCharArrayOfFragment.c_str()));

    textTypeConversion = singleLineCharArrayOfText;
    fragmentTypeConversion = singleLineCharArrayOfFragment;

    wordsInText = countWordsInProcessedString(textTypeConversion);
    wordsInFragment = countWordsInProcessedString(fragmentTypeConversion);

    std::unique_ptr<string[]> textWords(new string[wordsInText]);
    std::unique_ptr<string[]> fragmentWords(new string[wordsInFragment]);

    getWords(textTypeConversion, textWords);
    getWords(fragmentTypeConversion, fragmentWords);

    removeDuplicates(textWords, wordsInText);
    removeDuplicates(fragmentWords, wordsInFragment);

    removePrepositions(textWords, wordsInText);
    removePrepositions(fragmentWords, wordsInFragment);

#ifdef DEBUG
    for (int i = 0; i < wordsInFragment; i++) {
        cout << "%" << fragmentWords[i] << "%" << endl;
#endif // DEBUG

    wordsSurvivedInText = getSurvivedWordsNumber(textWords, wordsInText);
    wordsSurvivedInFragment = getSurvivedWordsNumber(fragmentWords, wordsInFragment);

    std::unique_ptr<string[]> clearedOfGarbageText(new string[wordsSurvivedInText]);
    std::unique_ptr<string[]> clearedOfGarbageFragment(new string[wordsSurvivedInFragment]);

    clearOfGarbage(textWords, clearedOfGarbageText, wordsInText);
    clearOfGarbage(fragmentWords, clearedOfGarbageFragment, wordsInFragment);

#ifdef DEBUG
    for (int i = 0; i < wordsSurvivedInText; i++)
        cout << "%" << clearedOfGarbageText[i] << "%" << " ";
    cout << endl;
    for (int i = 0; i < wordsSurvivedInFragment; i++)
        cout << "%" << clearedOfGarbageFragment[i] << "%" << " ";
#endif // DEBUG

    if (wordsSurvivedInFragment < 3)
        return 100;

    shinglesToCheck = wordsSurvivedInFragment - SHINGLE_SIZE + 1;
    shinglesToCheckInText = wordsSurvivedInText - SHINGLE_SIZE + 1;

    shinglesMatches = compareShingles(clearedOfGarbageText, clearedOfGarbageFragment,
        wordsSurvivedInText, shinglesToCheck, shinglesToCheckInText);
    borrowingPercentage = (shinglesMatches / shinglesToCheck) * 100;
    return borrowingPercentage;
}

void normalize(string input, char* normalized)
{
    int counter = 0;

    for (int i = 0; i < getStringLength(input); i++) {
        if (!isGarbage(input[i]))
        {
            normalized[counter] = input[i];
            counter++;
            if (isGarbage(input[i + 1]))
            {
                normalized[counter] = ' ';
                counter++;
            }
        }
    }
    if (normalized[counter - 1] == ' ')
    {
        normalized[counter - 1] = '\0';
    }
    else {
        normalized[counter] = '\0';
    }

    for (int i = 0; normalized[i] != '\0'; i++) {
        normalized[i] = toLowerCase(normalized[i]);
    }
}

bool isGarbage(char ch)
{
    for (int i = 0; garbage[i] != '\0'; i++)
    {
        if (garbage[i] == ch)
            return true;
    }
    return false;
}

int getStringLength(string input)
{
    int counter = 0;

    for (int i = 0; input[i] != '\0'; i++) {
        counter++;

        if (counter > MAX_LENGTH)
            break;
    }

    return counter;
}

void getWords(string input, std::unique_ptr<string[]>& destination)
{
    int wordCounter = 0;
    string buffer = "";
    int iterNum = getStringLength(input);

    for (int i = 0; i < iterNum; i++) {
        if (input[i] != ' ')
        {
            buffer += input[i];
            if (input[i + 1] == ' ' or input[i + 1] == '\0')
            {
                destination[wordCounter] = buffer;
                wordCounter++;
                buffer = "";
            }
        }
    }
}

bool strCmp(string dst, string src)
{
    int srcLength = 0;
    int dstLength = 0;

    for (int i = 0; src[i] != '\0'; i++)
    {
        srcLength++;
    }

    for (int i = 0; dst[i] != '\0'; i++)
    {
        dstLength++;
    }

    if (dstLength != srcLength)
        return false;

    for (int i = 0; src[i] != '\0'; i++)
    {
        if (src[i] != dst[i])
            return false;
    }
    return true;
}

double compareShingles(std::unique_ptr<string[]>& textWords,
    std::unique_ptr<string[]>& fragmentWords, int wordsInText, int shinglesToCheck, int shinglesToCheckInText)
{
    double matches = 0;

    string shingleFromText = "";
    string shingleFromFragment = "";

    for (int i = 0; i < shinglesToCheck; i++) {
        shingleFromFragment = fragmentWords[i] + fragmentWords[i + 1] + fragmentWords[i + 2];

#ifdef DEBUG
        cout << shingleFromFragment << " :fragment" << endl;
#endif // DEBUG

        for (int j = 0; j < shinglesToCheckInText; j++) {
            shingleFromText = textWords[j] + textWords[j + 1] + textWords[j + 2];

#ifdef DEBUG
            cout << shingleFromText << " :text" << endl;
#endif // DEBUG

            if (strCmp(shingleFromText, shingleFromFragment))
            {
                matches++;
                break;
            }
            shingleFromText = "";
        }
        shingleFromFragment = "";
    }
    return matches;
}

int countWordsInProcessedString(string words)
{
    int wordsCounter = 0;

    for (int i = 0; words[i] != '\0'; i++) {
        if (words[i] == ' ')
        {
            wordsCounter++;
        }
    }
    return wordsCounter + 1;
}

void removeDuplicates(std::unique_ptr<string[]>& source, int wordsInText)
{
    int counter = 0;
    int deep = 0;

    for (int i = 0; i < wordsInText - 1; i++) {
        deep = 1;
        if (strCmp(source[i], source[i + deep])) {
            while (((i + deep) < wordsInText) and (strCmp(source[i], source[i + deep])))
            {
                source[i + deep] = "";
                deep++;
            }
            i = i + deep - 1;
        }
    }
}

int getSurvivedWordsNumber(std::unique_ptr<string[]>& source, int words)
{
    int wordsSurvived = 0;

    for (int i = 0; i < words; i++) {
        if (!strCmp(source[i], "")) {
            wordsSurvived++;
        }
    }
    return wordsSurvived;
}

char toLowerCase(char ch)
{
    if (ch >= A_CAPITAL and ch <= Z_CAPITAL)
    {
        ch += UPPERCASE_TO_LOWERCASE_DIFFERENCE;
    }

    return ch;
}

bool isPreposition(string word)
{
    int charCounter = 0;

    while (word[charCounter] != '\0')
    {
        charCounter++;
    }

    if (charCounter < 3)
    {
        return true;
    }

    for (int i = 0; i < PREPOSITIONS_NUMBER; i++)
    {
        if (strCmp(word, prepositions[i]))
        {
            return true;
        }
    }
    return false;
}

void removePrepositions(std::unique_ptr<string[]>& source, int wordsInText)
{
    for (int i = 0; i < wordsInText; i++) {
        if (isPreposition(source[i]))
        {
            source[i] = "";
        }
    }
}

void clearOfGarbage(std::unique_ptr<string[]>& source, std::unique_ptr<string[]>& destination, int srcLength)
{
    int counter = 0;

    for (int i = 0; i < srcLength; i++) {
        if (!strCmp(source[i], ""))
        {
            destination[counter] = source[i];
            counter++;
        }
    }
}

bool isEmpty(string text)
{
    return strCmp(text, "");
}
