#ifndef STRATEGY_MCTS_H
#define STRATEGY_MCTS_H

#include <iostream>

class MCSState
{
public:
	MCSState(){}
	virtual ~MCSState(){}
    
	virtual int DefaultPolicy() = 0;
	virtual MCSState *NextSonState() = 0;

};

class MCSTNode
{
protected:
	MCSTNode *firstSon, *nextSibling, *parent;
	MCSState *state;
	int value, times;

public:
    MCSTNode(MCSState *state, MCSTNode *parent): state(state), parent(parent), value(0), times(0)
    {
        firstSon = NULL;
        nextSibling = NULL;
    }
    virtual ~MCSTNode()
    {
	    for (MCSTNode *son = firstSon, *next; son != NULL; son = next)
	    {
		    next = son->nextSibling;
		    delete son;
	    }
	    delete state;
    }

	int GetValue() const { return value; }
	int GetTimes() const { return times; }

	void AddValue(const int delta) { value += delta; times++; }

	virtual MCSTNode *Expand() = 0;

	MCSTNode *GetFirstSon() const
	{
		return firstSon;
	}

	MCSTNode *GetNextSibling() const
	{
		return nextSibling;
	}

	MCSTNode *GetParent() const
	{
		return parent;
	}

	MCSState *GetState() const
	{
		return state;
	}

	void SetFirstSon(MCSTNode *firstSon)
	{
		MCSTNode::firstSon = firstSon;
	}

	void SetNextSibling(MCSTNode *nextSibling)
	{
		MCSTNode::nextSibling = nextSibling;
	}

};

class MCSTree{
    protected:
        MCSTNode *root;

    public:
        MCSTree(MCSTNode *root): root(root){
        }

        virtual ~MCSTree(){
            delete root;
        }

        virtual MCSTNode *TreePolicy() = 0;

//	virtual void BackUp() = 0;
};




#endif //STRATEGY_MCTS_H
