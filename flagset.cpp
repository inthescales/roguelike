#include "flagset.h"

flagset::flagset(){
    the_set = new set<flags_t>;
}

flagset::flagset(flagset * copied) {

    the_set = new set<flags_t>(*(copied->the_set));
}

bool flagset::add_flag(flags_t nflag) {

    if (has_flag(nflag)) {
        return true;
    } else {
        the_set->insert(nflag);
        return false;
    }
}

bool flagset::has_flag(flags_t qflag) {

    return the_set->count(qflag) == 1;
}
