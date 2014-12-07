#ifndef FLAGSET_H
#define FLAGSET_H

#include <set>

using std::set;

enum flags_t {
    FLAG_NULL = 0,
    FLAG_ACT_CAN_WALK,
    FLAG_ACT_CAN_SWIM,
    FLAG_ACT_CAN_FLY,
    FLAG_TILE_CAN_WALK,
    FLAG_TILE_CAN_SWIM,
    FLAG_TILE_CAN_FLY,
    FLAG_TITLE_OPAQUE
};

class flagset {

    private:
    set<flags_t> * the_set;

    public:
    // Functions =====
    flagset();
    flagset(flagset *);
    
    bool add_flag(flags_t);
    bool has_flag(flags_t);

};

#endif