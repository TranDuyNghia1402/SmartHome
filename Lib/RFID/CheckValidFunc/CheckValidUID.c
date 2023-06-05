#include "CheckValidUID.h"

unsigned char checkValidUid(unsigned char *uid)
{
    unsigned char idx, countCheck = 0;
    for (idx = 0; idx < 5; idx++)
        if (uid[idx] == validUID[idx])
            countCheck++;
    if (countCheck == 5)
        return 1;
    else
        return 0;
}