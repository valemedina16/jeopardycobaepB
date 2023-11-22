
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "main.h"

using namespace std;

JeopardyQuestion::JeopardyQuestion()
{
    vector<string> emptyVec;
    this->m_question = "";
    this->m_possible_answers = emptyVec;
    this->m_correct_answer = 0;
    this->m_point_value = 0;
    this->m_category = 0;
}

JeopardyQuestion::JeopardyQuestion(string question, vector<string> possibleAnswers, 
                                    int correctAnswer, int pointValue, int category)
{
    this->m_question = question;
    this->m_possible_answers = possibleAnswers;
    this->m_correct_answer = correctAnswer;
    this->m_point_value = pointValue;
    this->m_category = category;

}

// Setup object if not initialized with correct data when allocated
void JeopardyQuestion::setup(string question, vector<string> possibleAnswers, 
                                    int correctAnswer, int pointValue, int category)
{
    this->m_question = question;
    this->m_possible_answers = possibleAnswers;
    this->m_correct_answer = correctAnswer;
    this->m_point_value = pointValue;
    this->m_category = category;
}

string JeopardyQuestion::getQuestion() const
{
    return this->m_question;
}

vector<string> JeopardyQuestion::getPossibleAnswers() const
{
    return this->m_possible_answers;
}

int JeopardyQuestion::answerQuestion(int answerSelected) // Check to see if answer is correct or not
{
    if(answerSelected == this->m_correct_answer)
        return this->m_point_value;
    else
        return -1;
}

int JeopardyQuestion::getPointVal() const
{
    return this->m_point_value;
}

void JeopardyQuestion::setToTried()
{
    this->m_point_value = 0;
}