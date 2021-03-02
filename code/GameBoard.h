#ifndef _STRATEGY_BOARD_H_
#define _STRATEGY_BOARD_H_

#include <iostream>
#include <cstring>
#include <cmath>

#include "Judge.h"
#include "MCTS.h"

#define USER 0
#define MACHINE 1
#define TIME_LIMIT 2.9
#define INF 2147483647
#define C 0.8


class MyBoardState: public MCSState{
    private:
        static int **board;
        u_short *chessState, *top;

        std::pair<int, int> RandomPlace();
        std::pair<int, int> Place();
        std::pair<int, int> placeLocation;

        int nextSonState;
        bool player;
        void ToBoard();
        void InitBoardState();
        void InitBoardState2(const MyBoardState &predstate, bool player, int x, int y);
        int CheckEmpty();
        void InitToBoard();
        bool CheckEnd(bool _player, std::pair<int, int> location);
        
    
    public:
        static int BoardWidth, BoardHeight, noX, noY;

        MyBoardState(bool player);
        MyBoardState(const MyBoardState &predstate, int x, int y);

        int DefaultPolicy();
        std::pair<int, int> GetPlaceLocation();
        MCSState *NextSonState();

        virtual ~MyBoardState();
};

class BoardTreeNode: public MCSTNode{
    public:
        BoardTreeNode(MCSState *state, MCSTNode *parent): MCSTNode(state, parent){
        }

        MCSTNode *Expand();
};

class BoardTree: public MCSTree{
    private:
        float UCB1(MCSTNode *parent, MCSTNode *son);

        MCSTNode *BestSon(MCSTNode *parent);
    
        void BackUp(MCSTNode *node, int value);
        void Simulate();
    
    public:
        static clock_t StartTime;
        BoardTree(bool player): MCSTree(new BoardTreeNode(new MyBoardState(player), NULL)){}

        void MoveRoot(std::pair<int, int> place);
        MCSTNode *TreePolicy();
        std::pair<int, int> MCTS();
};

#endif
