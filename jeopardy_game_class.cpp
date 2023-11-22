#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>

#include "main.h"
#include "TextTable.h"

using namespace std;


JeopardyGame::JeopardyGame(string fileName)
{
    this->m_total_score = 0;
    this->m_total_tries = 0;
    this->m_run_game = true;
    if(fileName.empty())
        throw "Error: No se puede abrir el nombre del archivo vacio.";
    this->m_file_name = fileName;
    this->readFile();
    this->m_last_message = CHAR_ESC+"[1;32m[ MESSAGE ]"+CHAR_ESC+"[0m - Bienvenidos a Jeopady COBAEP! Seleccione una pregunta escribiendo categoria y numero (ej. A2)";
    this->runGame();
    
    
}

void JeopardyGame::runGame()
{
    int totalAvailableTries = this->m_num_categories * this->m_num_questions_per_cat;

    // While the all tries are exhausted or until user indicates they want ot exit game
    while(this->m_total_tries < totalAvailableTries && this->m_run_game)
    {
        system(CLEAR_S); // Clear screen
        string userInput;
        this->showTitle();
        this->showLastMessage();
        this->showBoard();
        cout << endl;
        cout << CHAR_ESC+"[1;34m>>> "+CHAR_ESC+"[0m ";
        cin >> userInput;
        int returnVal = this->validateInput(userInput);
        if(!returnVal) // Then nothing happened so reloop
        {
            continue;
        }
        else if(returnVal > 0) // Correct answer - Add points to the m_total_score & increment m_total_tries
        {
            this->m_last_message = " "+CHAR_ESC+"[1;32m[ MESSAGE ]"+CHAR_ESC+"[0m  : Eso es correcto! Cual es tu proxima eleccion?";
            this->m_total_score += returnVal;
            this->m_total_tries++;
        }
        else if(returnVal == -1) // Incorrect answer - increment m_total_tries;
        {
            this->m_last_message = " "+CHAR_ESC+"[1;33m[ MESSAGE ]"+CHAR_ESC+"[0m : Eso es incorrecto! Lo siento :( Cual es tu proxima eleccion?";
            this->m_total_tries++;
        }
        else if(returnVal == -2) // Not a valid input
        {
            this->m_last_message = " "+CHAR_ESC+"[1;31m[ MESSAGE ]"+CHAR_ESC+"[0m : Necesita una entrada valida. Intentelo de nuevo:";
        }
        else if(returnVal == -3) // Already tried question
        {
            this->m_last_message = " "+CHAR_ESC+"[1;33m[ MESSAGE ]"+CHAR_ESC+"[0m : Ya intentaste ese. Por favor, prueba con otro:";
        }
    }
    system(CLEAR_S);
    this->showEndScreen(); // Show the end screen
}

void JeopardyGame::showEndScreen() const
{
    this->showTitle();
    TextTable t( '~', '|', '+' );
    const string spacer = "              ";
    const string spacer2 = "                      ";

    t.add(spacer + "Puntuacion Final:" + spacer);
    t.add(spacer + to_string(this->getScore()) + spacer);
    t.endOfRow();
    t.add(spacer + "Numero de intentos:" + spacer);
    t.add(spacer + to_string(this->getTries()) + spacer);
    t.endOfRow();
    t.setAlignment( 2, TextTable::Alignment::LEFT );
    cout << endl;
    cout << t;
    cout << endl << endl << CHAR_ESC+"[1;45m" << spacer2 << "BUEN TRABAJO! Gracias por Jugar Jeopardy COBAEP :)" << spacer2 << CHAR_ESC+"[0m\n\n\n";
}

void JeopardyGame::readFile()
{
    string line;
    int numOfCategories = 0;

    this->m_game_file.open(this->m_file_name);
    if(this->m_game_file.fail())
        throw "Error: Fallo al abrir el archivo: " + this->m_file_name;

    while(!this->m_game_file.eof()) // Read file and populate the m_num_questions_per_cat and m_num_answers_per_ques so that can dynamically allocate mem
    {
        getline(this->m_game_file, line);
        if(line[0] == '#') // Number of categories
            numOfCategories++;
        else if(line[0] == '^') // Number of questions per category
            this->m_num_questions_per_cat = stoi(line.substr(2));
        else if(line[0] == '$') // Number of answers per question
            this->m_num_answers_per_ques = stoi(line.substr(2));

        if(numOfCategories > 7) // Limitation due to category m_cat_letters only going from A to G because H is used as help key
            throw "Error: el juego solo admite 7 categorias. Reduzca el numero de categorias e intentelo de nuevo.";
    }
    const int NUM_OF_CATEGORIES = numOfCategories;
    const int NUM_OF_QUESTIONS_PER_CAT = this->m_num_questions_per_cat;
    vector<string> catNamesVector (numOfCategories);
    this->m_num_categories = numOfCategories;
    this->m_category_names = catNamesVector;
    this->m_game_elements = new JeopardyQuestion*[NUM_OF_CATEGORIES];
    for(int i = 0; i < NUM_OF_CATEGORIES; i++)
    {
        this->m_game_elements[i] = new JeopardyQuestion[NUM_OF_QUESTIONS_PER_CAT];
    }

    this->m_game_file.clear();
    this->m_game_file.seekg(0, ios::beg);

    int currGameElement = 0;
    int currPointVal = 0;
    int catNameCount = 0;
    int currCategoryCount = -1;
    int currPossibleAnswersCount = 0;
    int currCorrectAnswer = 0;
    string currQuestion;
    vector<string> currPossibleAnswers(this->m_num_questions_per_cat);

    while(!this->m_game_file.eof()) // Read file to populate question objects with the proper data
    {
        
        getline(this->m_game_file, line);
        if(line[0] == '#') // Category name
        {
            this->m_category_names[catNameCount] = line.substr(2);
            catNameCount++;
        }
        else if(line[0] == '~') // New category signaler
        {
            currGameElement = 0;
            currCorrectAnswer = 0;
            currCategoryCount++;
        }
        else if(line[0] == '%') // Point value for current question
        {
            currPointVal = stoi(line.substr(2));
        }
        else if(line[0] == '*') // New question signaler
        {
            currQuestion = line.substr(2);
            currPossibleAnswersCount = 0;
            currPossibleAnswers.clear();
        }
        else if(line[0] == '@') // an Incorrect choice 
        {
            currPossibleAnswers.push_back(line.substr(2));
            currPossibleAnswersCount++;
            if(currPossibleAnswersCount == this->m_num_answers_per_ques) // If end of questions, setup question object & increment to the next game element
            {
                this->m_game_elements[currCategoryCount][currGameElement].setup(currQuestion, currPossibleAnswers, currCorrectAnswer, currPointVal, currCategoryCount);
                currGameElement++;
            }
        }
        else if(line[0] == '&') // a Correct choice 
        {
            currPossibleAnswers.push_back(line.substr(2));
            currCorrectAnswer = currPossibleAnswersCount;
            currPossibleAnswersCount++;
            if(currPossibleAnswersCount == this->m_num_answers_per_ques) // If end of questions, setup question object & increment to the next game element
            {
                this->m_game_elements[currCategoryCount][currGameElement].setup(currQuestion, currPossibleAnswers, currCorrectAnswer, currPointVal, currCategoryCount);
                currGameElement++;
            }
        }

        if(currGameElement > this->m_num_questions_per_cat) // If number of questions per category exceeds the amount specified (possible memory overflow/corruption)
            throw "Error: hay un problema con el archivo de texto en peligro. Excediste la cantidad de preguntas por categoria que especificaste. Por favor solucione y vuelva a intentarlo.";
        if(currPossibleAnswersCount > this->m_num_answers_per_ques)
            throw "Error: hay un problema con el archivo de texto en peligro. Excediste la cantidad de respuestas por pregunta que especificaste. Por favor solucione y vuelva a intentarlo.";
    }
}

int JeopardyGame::getScore() const
{
    return this->m_total_score;
}

int JeopardyGame::getTries() const
{
    return this->m_total_tries;
}

void JeopardyGame::showTitle() const
{
    cout << endl 
            <<    "  888888 8888888888  .d88888b.  8888888b.      d8888 8888888b.  8888888b. Y88b   d88P "
    << endl << "    \"88b 888        d88P\" \"Y88b 888   Y88b    d88888 888   Y88b 888  \"Y88b Y88b d88P  "
    << endl << "     888 888        888     888 888    888   d88P888 888    888 888    888  Y88o88P   "
    << endl << "     888 8888888    888     888 888   d88P  d88P 888 888   d88P 888    888   Y888P    "
    << endl << "     888 888        888     888 8888888P\"  d88P  888 8888888P\"  888    888    888     "
    << endl << "     888 888        888     888 888       d88P   888 888 T88b   888    888    888     "
    << endl << "     88P 888        Y88b. .d88P 888      d8888888888 888  T88b  888  .d88P    888     "
    << endl << "     888 8888888888  \"Y88888P\"  888     d88P     888 888   T88b 8888888P\"     888     "
    << endl << "   .d88P                                                                              "
    << endl << " .d88P\"                                                                               "
    << endl << "888P\"                                                                                 "
    << endl << endl;
}

void JeopardyGame::showBoard() const
{
    /*
        All this below is code to display the board correctly
    */
    const string spacer = "      ";
    const string small_spacer = "   ";
    const string filler = "###################";
    const int totalAvailableTries = this->m_num_categories * this->m_num_questions_per_cat;
    TextTable t( '~', '|', '+' );
    t.add("Puntaje:" + to_string(this->getScore())); // Display score on top of table
    t.add(" ");
    t.add("Los intentos disponibles: " + to_string(totalAvailableTries - this->getTries())); // Display tries remaining on top of table
    t.add(spacer + spacer + " Keys:" ); // Helper keys
    t.add(" X=Salir  |  H=Ayuda");
    t.endOfRow();
    t.add(filler);
    for(int i = 0; i < this->m_num_categories; i++) // Populate table with a filler "#"
    {
        t.add(filler);
    }
    t.endOfRow();
    t.add(small_spacer + "Categorias:" + small_spacer);
    for(int i = 0; i < this->m_num_categories; i++) // Populate table with category names
    {
        t.add(spacer + this->m_category_names[i] + spacer);
    }
    t.endOfRow();
    t.add(filler);
    for(int i = 0; i < this->m_num_categories; i++)
    {
        t.add(filler);
    }
    t.endOfRow();
    t.add(" ");
    for(int i = 0; i < this->m_num_categories; i++) // Populate category letters (ex. A, B, C.. etc.)
    {
        t.add(spacer + "[ "+ this->m_cat_letters[i] +" ]" + spacer); 
    }
    t.endOfRow();
    for(int i = 0; i < this->m_num_questions_per_cat; i++) // Populate the question points in table
    {
        t.add(spacer + to_string(i + 1) + spacer);
        for(int n = 0; n < this->m_num_categories; n++)
        {
            int _ptVal = this->m_game_elements[n][i].getPointVal();
            string ptVal;
            if(!_ptVal) // If point value is 0 (in case of tried already), display an X 
                ptVal = " X";
            else
                ptVal = to_string(_ptVal);
            t.add(spacer + ptVal + spacer);
        }
        t.endOfRow();
    }
    t.setAlignment( 2, TextTable::Alignment::LEFT );
    cout << endl;
    cout << t;
}

int JeopardyGame::validateInput(string userInput)
{
    if(!userInput.size()) // If nothing supplied, return nothing
        return 0;
    else if(toupper(userInput[0]) == 'X') // User indicated they want to exit game.
    {
        this->endGame();
        return 0;
    }
    else if(toupper(userInput[0]) == 'H') // User indicated they want to see help screen
    {
        this->showHelp();
        return 0;
    }
    else if(userInput.size() < 2 || userInput.size() > 2) // Not a valid input
        return -2;
    else 
    {
        for(int i = 0; i < this->m_num_categories; i++) 
        {
            if(toupper(userInput[0]) == this->m_cat_letters[i]) // Check to see if the letter inputted is in category
            {
                for(int n = 0; n < this->m_num_questions_per_cat; n++)
                {
                    if((stoi(userInput.substr(1,2)) - 1) == n) // Check to see if number inputter (after letter) is in questions
                    {
                        return this->tryQuestion(i, n);
                    }
                }
            }
        }
    }
    return -2; // Else return invalid input
}

void JeopardyGame::endGame()
{
    this->m_run_game = false;
}

void JeopardyGame::showHelp() const
{
    system(CLEAR_S);
    string anyInp;
    this->showTitle();
    TextTable t( '-', '|', '+' );
    t.add(" Selecciona una pregunta: ");
    t.add(" Utilice el valor de letra y numero para la pregunta que desea seleccionar (por ejemplo, B3). ");
    t.endOfRow();
    t.add(" Reintentos: ");
    t.add(" Solo se le permite un intento por pregunta, asi que elija sabiamente antes de seleccionar. ");
    t.endOfRow();
    t.add(" Suma/deduccion de puntos: ");
    t.add(" Obtienes puntos agregados por las respuestas correctas pero no pierdes ningun punto por las incorrectas. ");
    t.endOfRow();
    t.setAlignment( 2, TextTable::Alignment::LEFT );
    cout << endl;
    cout << t;
    cout << endl << endl << CHAR_ESC+"[1;34mIngrese cualquier clave y presione Enter para regresar. "+CHAR_ESC+"[0m" << endl;
    cin >> anyInp;
}

void JeopardyGame::displayQuestion(int cat, int ques, vector<string> &answers) const
{
    const string small_spacer = "        ";
    TextTable t( '-', '|', '+' );
    t.add("##  Pregunta Jeopardy: " + small_spacer); // Display the question itself
    t.endOfRow();
    t.add(small_spacer + this->m_game_elements[cat][ques].getQuestion() + small_spacer);
    t.endOfRow();
    t.add(" ");
    t.endOfRow();
    t.add("##  Respuestas Posibles:" + small_spacer);
    t.endOfRow();
    for(int i = 0; i < answers.size(); i++) // Display each possible answer
    {
        t.add(small_spacer + to_string(i+1) + ". " + answers[i] + small_spacer);
        t.endOfRow();
    }
    t.setAlignment( 2, TextTable::Alignment::LEFT );
    cout << endl;
    cout << t;
    cout << endl << CHAR_ESC+"[1;34mCual es tu respuesta:"+CHAR_ESC+"[0m ";
}

int JeopardyGame::tryQuestion(int cat, int ques)
{
    vector<string> answers = this->m_game_elements[cat][ques].getPossibleAnswers();
    system(CLEAR_S);
    if(!this->m_game_elements[cat][ques].getPointVal()) // Question has been tried already
        return -3;
    int userInput = 0;
    this->showTitle();
    this->displayQuestion(cat, ques, answers);
    cin >> userInput;
    while(cin.fail() || userInput < 1 || (userInput - 1) >= answers.size()) // If not valid input, keep looping until user provides valid input.
    {
        system(CLEAR_S);
        this->m_last_message = " "+CHAR_ESC+"[1;31m[ MESSAGE ]"+CHAR_ESC+"[0m : Necesita una entrada valida. Intentelo de nuevo:";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(),'\n');
        this->showTitle();
        this->showLastMessage();
        this->displayQuestion(cat, ques, answers);
        cin >> userInput;
        if(userInput < 1 || (userInput - 1) >= answers.size())
            continue;
        if(!cin.fail())
            break;
    }

    int retVal = this->m_game_elements[cat][ques].answerQuestion(userInput-1);
    this->m_game_elements[cat][ques].setToTried();
    return retVal;
}

void JeopardyGame::showLastMessage() // Display the last message and then set m_last_message to null (because it's been displayed already)
{
    cout << this->m_last_message << endl;
    this->m_last_message = "";
}