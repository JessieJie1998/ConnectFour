#ifndef DECSTATIC_H_
#define DECSTATIC_H_

#include <iostream>

int MyBoardState::BoardHeight;
int MyBoardState::BoardWidth;
int MyBoardState::noX;
int MyBoardState::noY;
int **MyBoardState::board;
time_t BoardTree::StartTime;

struct timeval now;

BoardTree *boardTree = NULL;

std::pair<int, int> place;


#endif
