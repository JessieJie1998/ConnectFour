#include "GameBoard.h"
#include <ctime>

using namespace std;

void MyBoardState::InitBoardState(){
    chessState = new u_short[BoardWidth];
	top = new u_short[BoardWidth];

    //初始化
    for(int i=0; i<BoardWidth; i++){
        chessState[i] = 0;
        top[i] = 0;
    }

    if (noX == 0){
        top[noY] = 1;
    }
}

MyBoardState::MyBoardState(bool _player){
	player = _player;

	InitBoardState();

	nextSonState = 0;
	MyBoardState::board = new int*[MyBoardState::BoardHeight];

    for (int i = 0; i < MyBoardState::BoardHeight; i++){
		MyBoardState::board[i] = new int[MyBoardState::BoardWidth];
    }
    //生成随机种子
	srand((unsigned int) time(NULL));
}

void MyBoardState::InitBoardState2(const MyBoardState &predstate, bool player, int x, int y){
    chessState = new u_short[BoardWidth];
    top = new u_short[BoardWidth];

    for(int i=0; i<BoardWidth; i++){
        chessState[i]  = predstate.chessState[i];
        top[i] = predstate.top[i];
    }

    if(!player){
        chessState[y] = chessState[y] | (1 << x);
    }

    top[y] = (u_short) ((x + 1 == noX && y == noY) ? (x + 2) : (x + 1));
}


MyBoardState::MyBoardState(const MyBoardState &predstate, int x, int y){
    
	placeLocation.first = x;
	placeLocation.second = y;
	player = 1 - predstate.player;

    InitBoardState2(predstate, player, x, y);
    
	nextSonState = 0;
    //判断终态
	ToBoard();
    //若当前0落子，则上一步1落子，此时1有可能赢
    if (!player && machineWin(BoardHeight - placeLocation.first - 1,
                              placeLocation.second,
                              BoardHeight,
                              BoardWidth,
                              board)){
        nextSonState = BoardWidth;
    }else if(player && userWin(BoardHeight - placeLocation.first - 1,
                               placeLocation.second,
                               BoardHeight,
                               BoardWidth,
                               board)){
		nextSonState = BoardWidth;
    }else{
        //确定子状态可行的位置
        while(nextSonState < BoardWidth){
            if (top[nextSonState] < BoardHeight){
                break;
            }
            nextSonState++;
        }
	}
}

MyBoardState::~MyBoardState(){
    //释放内存
	delete[] chessState;
	delete[] top;
}

int MyBoardState::CheckEmpty(){
    int count = 0;

    for (int i = 0; i < BoardWidth; ++i){
        if (top[i] < BoardHeight){
            count++;
        }
    }
    return count;
}
pair<int, int> MyBoardState::RandomPlace(){

    int count = CheckEmpty();

    if (count == 0){
		return make_pair(-1, -1);
    }

	count = rand() % count;

    for (int i = 0; i < BoardWidth; ++i){
		if (top[i] < BoardHeight){
            if (count-- == 0){
                return make_pair(top[i], i);
            }
		}
    }

	return make_pair(-1, -1);
}

pair<int, int> MyBoardState::Place(){
    //落子
	pair<int, int> &&location = make_pair((int) top[nextSonState], nextSonState);
    nextSonState++;

    while(nextSonState < BoardWidth){
        if (top[nextSonState] < BoardHeight){
            break;
        }
        nextSonState++;
    }

	return location;
}

void MyBoardState::InitToBoard(){
    for (int i = 0; i < BoardHeight; i++){
        for(int j=0; j < BoardWidth; j++){
            board[i][j] = 0;
        }
    }
}

void MyBoardState::ToBoard(){
    //判断最终状态

    InitToBoard();
    
    for (int j = 0; j < BoardWidth; j++){
        for (int i = 0; i < top[j]; i++){
            if (!(i == noX && j == noY)){
				board[BoardHeight - i - 1][j] = (((1 << i) & chessState[j]) > 0) + 1;
            }
        }
    }
}

inline pair<int, int> MyBoardState::GetPlaceLocation(){
	return placeLocation;
};

MCSState *MyBoardState::NextSonState(){
    if (nextSonState == BoardWidth){
		return NULL;
    }

	pair<int, int> location = Place();

	return new MyBoardState(*this, location.first, location.second);
}

bool MyBoardState::CheckEnd(bool _player, pair<int, int> location){
    bool b1 = _player && machineWin(BoardHeight - location.first - 1, location.second, BoardHeight, BoardWidth, board);
    bool b2 = (!_player && userWin(BoardHeight - location.first - 1, location.second, BoardHeight, BoardWidth, board));
    return b1||b2;
    
}

int MyBoardState::DefaultPolicy(){
    //模拟
	if (nextSonState == BoardWidth){
		return -1;
	}

	bool _player = player;
	pair<int, int> location;
	ToBoard();

	u_short *top = new u_short[BoardWidth];
    for(int i=0; i<BoardWidth; i++){
        top[i] = this->top[i];
    }

	while ((location = RandomPlace()).first != -1){
		this->top[location.second] = (u_short) ((location.first + 1 == noX && location.second == noY) ? (location.first + 2) : (location.first + 1));

		board[BoardHeight - location.first - 1][location.second] = _player + 1;

		if (CheckEnd(_player, location)){
			break;
		}

		_player = !_player;
	}

    for(int i=0; i<BoardWidth; i++){
        this->top[i] = top[i];
    }

	delete[] top;

    //输赢
	return location.first == -1 ? 0 : (_player == player ? 1 : -1);
}


MCSTNode *BoardTreeNode::Expand(){
    //扩张节点
	MCSState *newState = state->NextSonState();

    if (!newState){
		return NULL;
    }

	MCSTNode *newNode = new BoardTreeNode(newState, this);

	if (!firstSon){
		firstSon = newNode;
	}else{
		newNode->SetNextSibling(firstSon);
		firstSon = newNode;
	}
	return newNode;
}



float BoardTree::UCB1(MCSTNode *parent, MCSTNode *son){
    //更新UCB
    int &&N = son->GetTimes();
    int &&Q = son->GetValue();
	return -(float)Q / N + C * sqrtf(2 * logf(parent->GetTimes()) / N);
}

MCSTNode *BoardTree::BestSon(MCSTNode *parent){
    //选择
	float maxValue = -INF;
	MCSTNode *bestSon = NULL;
    MCSTNode *son = parent->GetFirstSon();
    while(son){
        float &&ucb1 = UCB1(parent, son);
        //从对手的角度
        if (ucb1 > maxValue){
            maxValue = ucb1;
            bestSon = son;
        }
        son = son->GetNextSibling();
    }
	return bestSon;
}

void BoardTree::BackUp(MCSTNode *node, int value){
	while (node){
		node->AddValue(value);
		value = -value;
		node = node->GetParent();
	}
}

MCSTNode *BoardTree::TreePolicy(){
    MCSTNode *current = root;
    MCSTNode *newNode = NULL;

	while ((newNode = current->Expand()) == NULL && current->GetFirstSon() != NULL){
		current = BestSon(current);
	}

	return current->GetFirstSon() == NULL ? current : newNode;
}

void BoardTree::MoveRoot(pair<int, int> place){
	if (not root->GetFirstSon()){
		MCSTNode *newNode = NULL;
		while (true){
			newNode = root->Expand();
            if (newNode == NULL){
				break;
            }
		}
	}

	MCSTNode *newRoot = root->GetFirstSon();
    while(newRoot){
        if (((MyBoardState *)newRoot->GetState())->GetPlaceLocation() == place){
            break;
        }
        newRoot = newRoot->GetNextSibling();
    }
    MCSTNode *son = root->GetFirstSon();
    MCSTNode *next = NULL;
    while(son){
        next = son->GetNextSibling();
        if (son != newRoot){
            delete son;
        }
        son = next;
    }
	root->SetFirstSon(newRoot);
	newRoot->SetNextSibling(NULL);
	root = newRoot;
}

void BoardTree::Simulate(){
    MCSTNode *newNode = TreePolicy();

    BackUp(newNode, newNode->GetState()->DefaultPolicy());
}

pair<int, int> BoardTree::MCTS(){
    int per_clock = 0;
    //while ((++per_clock % 10 != 0) || (time(NULL) - StartTime) < TIME_LIMIT){
    while ((clock() - StartTime) < TIME_LIMIT){
        Simulate();
    }
    
	int maxTime = -INF;
	MCSTNode *bestSon = NULL;
    MCSTNode *son = root->GetFirstSon();
    while(son){
        if (son->GetTimes() > maxTime){
            maxTime = son->GetTimes();
            bestSon = son;
        }
        son = son->GetNextSibling();
    }

	pair<int,int> place = dynamic_cast<MyBoardState*>(bestSon->GetState())->GetPlaceLocation();

	return place;
};


