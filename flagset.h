#ifndef FLAGSET_H
#define FLAGSET_H

#include <set>

using std::set;

enum flags_t {
    FLAG_NULL = 0,
    FLAG_ACT_CAN_WALK,
    FLAG_ACT_CAN_SWIM,
    FLAG_ACT_CAN_FLY,
    FLAG_ACT_OPAQUE,
    FLAG_OBJ_IN_INVENTORY,
    FLAG_TILE_CAN_WALK, // TILES ==============
    FLAG_TILE_CAN_SWIM,
    FLAG_TILE_CAN_FLY,
    FLAG_TILE_OPAQUE,
    FLAG_FEAT_NO_WALK, // FEATURES ==============
    FLAG_FEAT_NO_SWIM,
    FLAG_FEAT_NO_FLY,
    FLAG_FEAT_CAN_OPEN,
    FLAG_FEAT_CAN_CLOSE,
    FLAG_FEAT_OPAQUE,
    FLAG_MENU_SORT, // MENUS ==============
    FLAG_MENU_PLAYER

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