#ifndef AI_H
#define AI_H

#include "action.h"
#include "entity.h"

#include <map>
#include <vector>

using std::vector;

class entity;
class requirement;

class motivator {

    public:
    bool self;
    requirement * req;
    int value;

    motivator(bool, requirement *, int);
};

class idea {

    public:
    entity_type e_type;
    actionPurpose_t purpose;
    int base;
    vector<motivator*> * motivators;
    
    idea(entity_type, actionPurpose_t, int);
};

class aimodule {

    public:
    std::map<entity_type, vector<idea*>* > * ideas;
    
    void add_idea(idea *);
    aimodule();
};

class goal {

    public:
    entity * target;
    actionPurpose_t purpose;
    
    goal(entity *, actionPurpose_t);
};

class mind {

    public:
    aimodule * module;
    goal * cur_goal;
    
    mind(aimodule *);
};

class AI {

    public:
    static vector<entity*> * sense_entities(actor*);
    static goal * select_goal(actor*);
    static int idea_value(actor * decider, entity * target, idea * thought);
    static void take_action(actor *, goal *);
};

extern aimodule * aimod[];

#endif
