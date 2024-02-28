/*
 * The player tries to guess a phrase by suggesting letters. 
 *The phrase to guess is represented by a row of underscores, 
 *indicating the number of letters per word. If the player 
 *guesses a letter that occurs in the word, the letter appears
 * in the correct positions. If the suggested letter does not 
 *occur in the word, one element of a hanged man stick figure
 * is drawn as a tally mark. The game is over when either the 
 *player correctly guesses all the letters in the phrase or 
 *when the hangman is complete (after 5 incomplete guesses).
 *by Adam Thiemann
 */
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
using namespace std;

/*
 *This struct stores the text in every phrase, the minimum guesses
 * needed, and whether or not the phrase has been used.
 * PASSED, DON'T TOUCH
 */
struct Phrase
{
	string text; //Stores the text of the phrase
	unsigned int guessesRequired; //contains the minimum guesses
	bool isUsed = false; //True means the phrase has been used.
};

/*
 *This struct holds the right, wrong, and total guesses and the number 
 *of guesses of the user while playing the game.
 */
struct Guesses
{
	unsigned int numOfGuesses;
	string charGuesses;
};

/*
 *This enum is used to hold the difficulty levels to choose from
 */
enum DifficultyLevel {EASY, MEDIUM, HARD, INVALID_DIFFICULTY};

/**
 * Displays the gallows. As the miss count increases, more of the person is
 * displayed hanging from the gallows. Called in RunGame
 */
void drawGallows(int missCount);

/*
 *Reads in a file where each line is the text of a phrase. 
 *Initializes an array of Phrases. Returns the number of
 *phrases.  Called in main.
 */
int loadPhrasesFromFile(const string FILE_NAME, Phrase phraseArray[],
    const int MAX_LENGTH);

/*
 *The purpose of this function is to only allow unique characters from
 *A-Z to count as case insensitive unique characters. This will filter
 *out punctuation and spaces, and change uppercase to lowercase so that
 *all cases will be considered the same.  UniqueLetterCount will still
 *determine the uniqueness of the character.
 *Called in uniqueLetterCount.
 */
bool maybeUnique(const string UNIQ_CHARS, char& ch);

/*
 *This function takes a single phrase and checks the number of 
 *unique characters in that phrase, returning the amount.
 *Called in main.
 */
int uniqueLetterCount(const string SINGLE_PHRASE);

/*
 *Outputs the phrases after the guesses and use are known
 *Used for testing, so not called.
 */
void printPhrases(const Phrase phraseArray[], const int PHRASE_NUM);

/*
 *This function sorts the phrases based on the number of guesses that are
 *required to complete the phrase. Called in main.
 */
void sortPhrases(Phrase phraseArray[], 
	const int PHRASE_NUM);

/*
 *The purpose of this function is to swap the components of phrase and guess array.
 *Called in sortPhrases.
 */
void swapPhrases(const int INDEX, const int SMALL_INDEX, Phrase phraseArray[]);

/*
 *The purpose of this function is to display the current phrase 
 *with blanks including the correctly guessed character.
 *Function called in runGame
 */
string phraseWithBlanks(const string CURRENT_PHRASE, const string CORRECT_GUESSES);

/*
 *The purpose of this function is to support phraseWithBlanks to find out
 *what value should be passed to the string that holds the blanks.
 *It can only be used when Correct guesses is longer than 0.
 *Called in phraseWithBlanks.
 */
char withGuesses(char currentVal, char guessVal);

/*
 *The purpose of this function is to support phraseWithBlanks to find out
 *what value should be passed to the string that holds the blanks.
 *It can only be used when Correct guesses is 0 length.
 *Called in phraseWithBlanks.
 */
char withoutGuesses(char currentVal);

/*
 *The purpose of this function is to get the difficulty level from the user.
 *Function called in main.
 */
DifficultyLevel getDifficultyLevel();

/*
 *The purpose of this function is to declare values for the enumeration type
 *that correspond to a string.  This function is called in main.
*/
string convertDifficulty(DifficultyLevel diff);

/*
 *This function is to get the index of the next phrase to be used.
 * It gets this based off the difficulty that the user chooses, and
 * whether or not that phrase has been used.  Called in main.
 */
int randomPhraseIndex(int diff, const Phrase PHRASE_ARRAY[], int phraseNum);

/*
 *Once main has assembled and sorted all information needed to run
 *the game, this function is called to run a single Hangman game.
 * Called in main.
 */ 
void runGame(Phrase singlePhrase);

/*
 *This function checks the user's character guess and determines
 *whether it is valid, previously guessed, right, or wrong.
 *Called in runGame.
 */
void checkGuess(const char USER_GUESS, const string SINGLE_PHRASE, 
	Guesses guess[]);

/*Display Result shows the results of the win or loss.
 *Called in runGame.
 */
void displayResult(int wrongGuesses, 
	const string PHRASE, const string BLANK_PHRASE);

/*
 *Play again asks the user if they want to play again and
 * returns true if so, false if not.  Called in main.
 */
bool playAgain();

/*
 *This function makes Uppercase characters into lowercase.
 * Called in maybe unique and run Game
 */ 
char toLower(char uGuess);

bool checkVictory(string guessedPhrase);

int main()
{
    const string FILE_NAME = "phrases.txt"; //Name of file to load from
    const int MAX_PHRASES = 100; //Max number of phrases array will take

	//holds the number of phrases the user plays
	int phrasesAsked = 0;

	//This enum is the difficulty level.
	DifficultyLevel difficulty;	

	//Holds the phrases, guesses required, and uses.
    Phrase phraseArray[MAX_PHRASES];

	//holds the index value for the next phrase to be played
	int nextPhrase = -1;

	//initializes phraseArray to the number of phrases, max 100
    //Holds the actual length of the phraseArray in MAX_USED_INDEX
	//Initialize the array with the number of unique characters/min guesses.
    const int MAX_USED_INDEX = loadPhrasesFromFile(FILE_NAME, phraseArray,
    MAX_PHRASES);

	//Get all of the guesses required for the phrases.
	for (int index = 0; index < MAX_USED_INDEX; index++)
	{
		phraseArray[index].guessesRequired = 	
			uniqueLetterCount(phraseArray[index].text);
	}

	//Sort the phrases based on their guesses required.
	sortPhrases(phraseArray, MAX_USED_INDEX);

	//Seed the random number generator using the current time.
	srand(static_cast<unsigned int>(time(nullptr)));

	//Find the difficulty the user wants to play at.
	difficulty = getDifficultyLevel();

	do
	{
		//Find the next phrase based on the difficulty
		nextPhrase = randomPhraseIndex(static_cast<int>(difficulty), 
			phraseArray, MAX_USED_INDEX);

		cout << "Next Phrase is: " << phraseArray[nextPhrase].text << endl;
		//Show the user their current hangman, and show the phrase with blanks.
		//Then, ask them for their guesses, and re-display based on that guess.
		runGame(phraseArray[nextPhrase]);

		//Turns the phrase to used so it won't be repeated
		phraseArray[nextPhrase].isUsed = true;

		phrasesAsked++;
	}while (playAgain());

	cout << "You played " << phrasesAsked << " times." << endl;

	return 0;
}

bool playAgain()
{
	char yn;
	cout << "Would you like to play again? (y/n): ";
	cin >> yn;
	cout << endl;
	cin.clear();
	cin.ignore(256, '\n');

	switch (yn)
	{
		case 'y':
			return true;
		case 'n':
			return false;
		default:
			return false;
	}
}

void drawGallows(int missCount)
{
	// Output the top of the gallows
	cout
		<< "   +----+     \n"
		<< "   |    |     \n";

	// Output the stand and body
	switch (missCount)
	{
	case 0:
		cout
			<< "   |       \n"
			<< "   |       \n"
			<< "   |       \n"
			<< "   |       \n";
		break;
	case 1:
		cout
			<< "   |    O  \n"
			<< "   |    |  \n"
			<< "   |       \n"
			<< "   |       \n";
		break;
	case 2:
		cout
			<< "   |    O  \n"
			<< "   |   /|  \n"
			<< "   |       \n"
			<< "   |       \n";
		break;
	case 3:
		// The '\\' will translate as \, because it is a special char
		cout
			<< "   |    O   \n"
			<< "   |   /|\\ \n"
			<< "   |        \n"
			<< "   |        \n";
		break;
	case 4:
		cout
			<< "   |    O   \n"
			<< "   |   /|\\ \n"
			<< "   |     \\ \n"
			<< "   |        \n";
		break;
	default:
		cout
			<< "   |    O    \n"
			<< "   |   /|\\  \n"
			<< "   |   / \\  \n"
			<< "   |You're Dead\n";
	}

	// Output the base
	cout << "  =============\n" << endl;
}

//FUNCTION PASSED, DON'T TOUCH
int loadPhrasesFromFile(const string FILE_NAME, Phrase phraseArray[],
    const int MAX_LENGTH)
{
    int length = 0;
	int count = 0;
    ifstream fileIn;
    
    //Open the file
    fileIn.open(FILE_NAME);

    //what to do if the file doesn't exist.
    if (fileIn.fail())
        cout << "File does not exist" << endl;
    else
    {
        //read the file into phraseArray by line until the file
        //ends or the array maximum is met.
        while (getline(fileIn, phraseArray[length].text, '\n') && length < MAX_LENGTH)
		{
			phraseArray[length].guessesRequired = uniqueLetterCount(phraseArray[length].text);
			length++;
		}

		//Initialize remaining unused values.
		count = length;
		if (count != MAX_LENGTH)
		{
			while (count < MAX_LENGTH)
			{
				phraseArray[count++].text = 'A';
			}
		}
    }
    //Close the file
    fileIn.close();
    return length;
}

//WORKING, DON'T TOUCH
bool maybeUnique(const string UNIQ_CHARS, char& ch)
{

	//make the character lowercase
	toLower(ch);

	//Return false if its not a character
	if (ch <= 97 || ch >= 122)
		return false;

	//Check to see if the lowercase values equal anything already
	//considered unique
	else
	{
		for (unsigned int index = 0; index < UNIQ_CHARS.length(); index++)
		{
			//if the lowercase value is already in uniqChars
			if (ch == UNIQ_CHARS[index])
				return false;
		}
	}
	//character is eligible after being converted to lowercase, and
	//wasn't found in the UNIQ_CHARS string.
	return true;
}

//PASSED, DON'T TOUCH
int uniqueLetterCount(const string SINGLE_PHRASE)
{
    char ch; //Holds value of current character to test against holdChar
	string holdChar = " "; //Holds the unique Characters
	bool uniq = true; //Is true if character is unique.

    //Outer loop to go through each value
    for(unsigned int index = 0; index < SINGLE_PHRASE.length(); index++)
    {
		//set ch to character value of the phrase at the index
		ch = SINGLE_PHRASE[index];

		//used to test ch character values against that of holdChar
		for (unsigned int iter = 0; iter < holdChar.length(); iter++)
		{	
			//if the character equals anything in the held characters,
			//it is not unique.
			if (ch == holdChar[iter])
			{
				uniq = false;
			}
		} //end inner loop

		//if the unique bool remained true, check validity of the ch.
		//add the ch value to the string that holds unique characters.
		if (uniq && maybeUnique(holdChar, ch))
			holdChar += ch;
		
		//Assume that every new character is unique until proven otherwise.
		uniq = true;

    } //End outer loop

	//Erase the space that holdChar is initialized to.
	holdChar.erase(0, 1);

    return holdChar.length();
} //End unique letter count

//PASSED, DON'T TOUCH
void printPhrases(const Phrase PHRASE_ARRAY[], const int PHRASE_NUM)
{
	string strUse; //Holds the value for t/f in IS_USED

	//Display the header
	cout << setw(3) << right << "#" << " " << setw(49) << left 
        << "Phrase" << setw(5) << left << "State" << endl;
	
    //Display the guesses required, text, and whether or not it is used.
	for (int index = 0; index < PHRASE_NUM; index++)
	{
		if (PHRASE_ARRAY[index].isUsed)
			strUse = "used";
		else 
			strUse = "unused";

		cout << setw(3) << right << PHRASE_ARRAY[index].guessesRequired << " " 
			<< setw(49) << left << PHRASE_ARRAY[index].text 
			<< setw(4) << strUse << endl;
	}
}

//PASSED, DON'T TOUCH
void sortPhrases(Phrase phraseArray[], const int PHRASE_NUM)
{
	//Hold the location of the smallest number
	int smallestIndex, iter;

	for (int index = 0; index < PHRASE_NUM; index++)
	{
		smallestIndex = index;

		//If the iteration value is less than the index, swap the values.
		for (iter = index + 1; iter < PHRASE_NUM; iter++)
			if (phraseArray[iter].guessesRequired < 
				phraseArray[smallestIndex].guessesRequired)
				smallestIndex = iter;

		//Swap the values in the outer loop 		
		swapPhrases(index, smallestIndex, phraseArray);
	}
}

//WORKING, DON'T TOUCH
void swapPhrases(const int INDEX, const int SMALL_INDEX, Phrase phraseArray[])
{
    Phrase holdGuess;

    //Takes the current index (smaller number) and moves it to holdGuess.
	//Makes the index (smaller number) into the greater number (iter)
    //Replaces iter with the index value (smaller number)
    holdGuess = phraseArray[INDEX];
	phraseArray[INDEX] = phraseArray[SMALL_INDEX];
	phraseArray[SMALL_INDEX] = holdGuess;
}

//PASSED, DON'T TOUCH
string phraseWithBlanks(const string CURRENT_PHRASE, const string CORRECT_GUESSES)
{
	//character to build the actual phrase
	char buildPhrase;

	//string to hold the final product.
	string holdPhrase = "";

	//bool to know what function to call.
	bool noLength = false;

	//If there is no guess value make no length true.
	if (CORRECT_GUESSES.length() == 0)
		noLength = true;

	//at every character in holdphrase, loop through every character held in
	//the guessed characters to determine how to display the output.
	for (unsigned int index = 0; index < CURRENT_PHRASE.length(); index++)
	{//Begin outer for loop
		//What to do if the guesses have length.
		if (!noLength)
		{
			for (unsigned int iter = 0; iter < CORRECT_GUESSES.length(); iter++)
			{
				buildPhrase = withGuesses(CURRENT_PHRASE[index], CORRECT_GUESSES[iter]);

				//Break the loop if buildphrase is a character or punctuation.
				if (buildPhrase != '_')
					break;
			}
		}

		//there is no length to the correct guesses.
		else
		{
			buildPhrase = withoutGuesses(CURRENT_PHRASE[index]);
		}
		
		//Use buildPhrase to create the whole phrase in holdPhrase.
		holdPhrase += buildPhrase;
		if (index + 1 != CURRENT_PHRASE.length())
			holdPhrase += ' ';
	}//End outer loop

	return holdPhrase;
}

//WORKING, DON'T TOUCH
char withGuesses(char currentVal, char guessVal)
{
	//Character to build the whole phrase
	char buildPhrase;

	//If the character in the actual phrase is equal to the upper or lowercase
	//value of the character in correct guess. break loop if true because
	//character should be placed into the variable and no longer tested.
	if (currentVal == guessVal || 
		currentVal == guessVal - 32 ||
		currentVal == guessVal + 32)
	{
		buildPhrase = currentVal;
	}
	//If the character in the current phrase is any punctuation or space,
	//put it into the phrase with blanks and break the loop because it should
	//no longer be tested.
	else if ((currentVal >= 32 && currentVal <= 64) || 
		(currentVal >= 91 && currentVal <= 96))
	{
		buildPhrase = currentVal;
	}
	//Put the character in the current phrase as an underscore until all 
	//possibilities of guesses are tested.
	else
	{
		buildPhrase = '_';
	}
	return buildPhrase;
}

//WORKING, DON'T TOUCH
char withoutGuesses(char currentVal)
{
	//Returns the value to the entire string with blanks.
	char buildPhrase;

	//If the current index value is punctuation or a space,
	//display it.
	if ((currentVal >= 32 && currentVal <= 64) || 
			(currentVal >= 91 && currentVal <= 96))
		buildPhrase = currentVal;
	
	//Anything that isn't a space or punctuation, make an underscore.
	else
		buildPhrase = '_';

	return buildPhrase;
}

//PASSED, DON'T TOUCH
DifficultyLevel getDifficultyLevel()
{
	//Holds difficulty level
	int diff;
	DifficultyLevel val;

	//Display banner
	cout << "Difficulty levels:" << endl;
	cout << setw(5) << right << "1" << "  Easy" << endl; 
	cout << setw(5) << right << "2" << "  Medium" << endl;
	cout << setw(5) << right << "3" << "  Hard" << endl;

	//Get difficulty
	cout << "Pick a difficulty level (1 to 3): ";
	cin >> diff;
	cin.clear();
	cin.ignore(256, '\n');
	cout << endl;

	//Only occurs if the user inputs a number outside of 1-3
	while (diff < 1 || diff > 3)
	{
		cout << "Please choose a difficulty level between 1 and 3: ";
		cin >> diff;
		cin.clear();
		cin.ignore(256, '\n');
		cout << endl;
	}

	//convert the diff to corresponding enum value after subtracting 1.
	switch (--diff)
	{
		case 0:
			val = EASY;
			break;
		case 1:
			val = MEDIUM;
			break;
		case 2:
			val = HARD;
			break;
		default:
			val = INVALID_DIFFICULTY;

	}
	return val;
}

//PROBABLY WORKING, DON'T TOUCH
string convertDifficulty(DifficultyLevel diff)
{
	string level;

	switch (diff)
	{
		case EASY:
			level = "Easy";
			break;
		case MEDIUM:
			level = "Medium";
			break;
		case HARD:
			level = "Hard";
			break;
		default:
			level = "Invalid";
	}
	return level;
}

//PASSED, DON'T TOUCH
int randomPhraseIndex(int diff, const Phrase PHRASE_ARRAY[], int phraseNum)
{
	//Random number variable that is also the return value for the index.
	int randi = -1;

	//variable to make sure that ever index gets used if division rounds down.
	int leftover = phraseNum % 3;

	//counter as a break for the do while loop in case ever variable has been used.
	int counter = 0;

	//Get the random number based on the difficulty.
	do
	{
		switch (diff)
		{
			case 0:
				randi = rand() % (phraseNum / 3);
				break;
			case 1:
				randi = rand() % (phraseNum / 3) + (phraseNum / 3);
				break;

			//Add in the leftover guesses to the hardest difficulty
			case 2:
				randi = rand() % ((phraseNum / 3) + leftover) + (phraseNum / 3 * 2);
				break;
			default:
				cout << convertDifficulty(static_cast<DifficultyLevel>(diff));
		}
		counter++;
	} while (PHRASE_ARRAY[randi].isUsed && counter < 100);

	if (counter == 100)
	{
		cout << "Every phrase has been used." << endl;
	}
	return randi;
}

void runGame(Phrase singlePhrase)
{
	//Struct to track the guesses and intitialized
	//Index 0 is correct, index 1 is wrong, index 2 holds all
	Guesses guess[3];
	for (int index = 0; index < 3; index++)
	{
		guess[index].charGuesses = "";
		guess[index].numOfGuesses = 0;
	}

	//Char guess from the user.
	char currentGuess;

	//Stores the blank phrase to be shown to the user
	string blankPhrase;

	do
	{//Begin do...while loop
		//Draw the gallows based on the number of incorrect guesses.
		drawGallows(guess[1].numOfGuesses);

		//Get and output the blank phrase
		blankPhrase = phraseWithBlanks(singlePhrase.text, guess[0].charGuesses);
		cout << blankPhrase << endl;
		cout << "Previous incorrect guesses: " << guess[1].charGuesses << endl;

		//Find the user's character guess
		cout << "Enter guess: ";
		cin >> currentGuess;
		cin.clear();
		cin.ignore(256, '\n');
		cout << endl;

		//convert to lowercase if needed.
		currentGuess = toLower(currentGuess);

		//Clear the screen
		for (int index = 0; index < 19; index++)
			cout << endl;

		//Check the guess
		checkGuess(currentGuess, singlePhrase.text, guess);
		//Check for victory

	} while (!checkVictory(phraseWithBlanks(singlePhrase.text, guess[0].charGuesses)) && guess[1].numOfGuesses < 5); 
	/*end do...while loop*/ 

	//Show the results and get the final phrase with blanks
	blankPhrase = phraseWithBlanks(singlePhrase.text, guess[0].charGuesses);
	displayResult(guess[1].numOfGuesses, singlePhrase.text, blankPhrase);	
}

void displayResult(int wrongGuesses, 
	const string PHRASE, const string BLANK_PHRASE)
{
	//Show the gallows one last time
	drawGallows(wrongGuesses);

	//Display the blank phrase onelast time
	cout << BLANK_PHRASE << endl;

	//If they guessed 5 incorrectly
	if (wrongGuesses == 5)
		cout << "You're Dead! The phrase was:" << endl << "\"" <<
			phraseWithBlanks(PHRASE, PHRASE) << "\"" << endl;

	//If they won
	else 
		cout << "You Win!" << endl;
}

//PROBABLY WORKING
void checkGuess(const char USER_GUESS, const string SINGLE_PHRASE, 
	Guesses guess[])
{
	//If the guess is not a letter
	if ((USER_GUESS >= 32 && USER_GUESS <= 64) || 
			(USER_GUESS >= 91 && USER_GUESS <= 96))
	{
		cout << "'" << USER_GUESS << "' is not a valid guess. Please enter a letter."
			<< endl;
		return;
	}

	//If the guess has already been guessed.
	for (unsigned int index = 0; index < guess[2].charGuesses.length(); index++)
	{
		if(USER_GUESS == guess[2].charGuesses[index] || USER_GUESS == guess[2].charGuesses[index] + 32 ||
			USER_GUESS == guess[2].charGuesses[index] - 32)
		{
			cout << "You have already guessed an '" << guess[2].charGuesses[index] << "'." << endl;
			return;
		}		
	}

	//Go through ever letter in the phrase and check if it is equal to the guess
	//if it is, increment the number of correct guesses and total guesses, and
	//Add the guessed character to the total guesses string and the correct
	//guesses string.  If it is not, increment and update the total and
	//wrong guesses.
	for (unsigned int index = 0; index < SINGLE_PHRASE.length(); index++)
	{
		if(USER_GUESS == SINGLE_PHRASE[index] || USER_GUESS == SINGLE_PHRASE[index] + 32 ||
			USER_GUESS == SINGLE_PHRASE[index] - 32)
		{
			cout << "Good guess!" << endl;
			guess[2].charGuesses += USER_GUESS;
			guess[2].numOfGuesses++;
			guess[0].charGuesses += USER_GUESS;
			guess[0].numOfGuesses++;
			return;
		}
		else if (index == SINGLE_PHRASE.length() - 1)
		{
			cout << "Sorry, bad guess." << endl;
			guess[2].charGuesses += USER_GUESS;
			guess[2].numOfGuesses++;
			guess[1].charGuesses += USER_GUESS;
			guess[1].numOfGuesses++;
		}
	}
}

char toLower(char uGuess)
{
	if (uGuess >= 65 && uGuess <= 90)
		uGuess += 32;

	return uGuess;
}

bool checkVictory(string guessedPhrase){
	for (int i = 0; i < guessedPhrase.length(); i++)
		if (guessedPhrase[i] == '_')
			return false;
	return true;
}