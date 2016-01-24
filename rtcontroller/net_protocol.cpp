#include "net_protocol.h"

int equal(char cha, char chb)
{
    if (cha >= 'A' && cha <= 'Z') cha += 'a'-'A';
    if (chb >= 'A' && chb <= 'Z') chb += 'a'-'A';
    return cha == chb;
}

char *strsub(char* source, const char *sub)
{
    char *temp, *find = source;
    const char* temq = sub;
    while (*find) {
        for (temp = find, temq = sub; *temq && equal(*temp, *temq); temp++, temq++);
        if (*temq == '\0') return find;
        find++;
    }
    return 0;
}


/*obtain the angle, axis
0 ok!Run instructions
1 check online ? if true, you can send instruction
2 undefined instructions {3 Error with input angel}
4 Busy
5 Finish
*/

int Analyze_order(char *instruct, int *axis, int *angle)
{
    int num = 0;
    char *temp;
    if ((temp = strsub(instruct, "GO")) != 0) {
        temp += 2;
        while (*temp != '\0'&& *temp == ' ') temp++;
        if (*temp == 'x'|| *temp == 'X') *axis=0;
           else if (*temp == 'y'|| *temp == 'Y') *axis=1;
            else if (*temp == 'z'|| *temp == 'Z') *axis=2;
             else return 2;
        temp += 1;
        while (*temp != '\0'&& *temp == ' ') temp++;
        while (*temp >= '0' && *temp <= '9') {
            num = num*10 + (*temp - '0');
            if (num > 36000) return 3;
            temp ++;
        }
        *angle = num;
        return 0;
    }
    if ((temp = strsub(instruct, "IM")) != 0) {
        temp += 2;
        while (*temp != '\0'&& *temp == ' ') temp++;
        if ((*temp == 'O' || *temp == 'o') &&
           (*(temp + 1) == 'N' || *(temp + 1) == 'n'))
            return 1;
    }
    if ((temp = strsub(instruct, "finished")) != 0) {
        return 5;
    }
    return 2;
}
