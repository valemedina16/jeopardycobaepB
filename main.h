
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

extern const char CLEAR_S[];
extern const string CHAR_ESC;

class JeopardyQuestion {
    private:
        string m_question;
        vector<string> m_possible_answers;
        int m_correct_answer;
        int m_category;
        int m_point_value;
    public:
        JeopardyQuestion();
        JeopardyQuestion(string question, vector<string> possibleAnswers, int correctAnswer, int pointValue, int category);
        void setup(string question, vector<string> possibleAnswers, int correctAnswer, int pointValue, int category);
        string getQuestion() const;
        vector<string> getPossibleAnswers() const;
        int answerQuestion(int answerSelected);
        int getPointVal() const;
        void setToTried();
};

class JeopardyGame {
    private:
        const char m_cat_letters[8] = "ABCDEFG";
        string m_file_name;
        int m_num_categories;
        vector<string> m_category_names;
        ifstream m_game_file;
        JeopardyQuestion **m_game_elements;
        int m_total_score;
        int m_total_tries;
        int m_num_questions_per_cat;
        int m_num_answers_per_ques;
        bool m_run_game;
        string m_last_message;
        void readFile();
        void runGame();
        void endGame();
        void showHelp() const;
        void showLastMessage();
        void showTitle() const;
        void showBoard() const;
        int validateInput(string userInput);
        int getScore() const;
        int getTries() const;
        void displayQuestion(int cat, int ques, vector<string> &answers) const;
        void showEndScreen() const;
        int tryQuestion(int category, int question);
    public:
        JeopardyGame(string fileName);
};

