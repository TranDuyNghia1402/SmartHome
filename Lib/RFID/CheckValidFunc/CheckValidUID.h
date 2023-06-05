#ifndef CHECK_VALID_UID_H
#define CHECK_VALID_UID_H

static unsigned char validUID_1[] = {35, 16, 240, 171, 104}; // Tran Duy Nghia
static unsigned char validUID_2[] = {83, 12, 220, 167, 36};  // Nguyen Tien Dat

unsigned char checkValidUid(unsigned char *uid)
{
    unsigned char idx = 0, countCheck = 0, isCard = 0;
    for (idx = 0; idx < 5; idx++) {
        if (uid[idx] != validUID_1[idx]) {
            countCheck = 0;
            break;
        }
        else {
            countCheck++;
            isCard = 1;
            if (countCheck == 5)
                return isCard;
        }
    }
    for (idx = 0; idx < 5; idx++) {
        if (uid[idx] != validUID_2[idx]) {
            countCheck = 0;
            break;
        }
        else {
            countCheck++;
            isCard = 2;
            if (countCheck == 5)
                return isCard;
        }
    }
    return 0;
}

#endif
