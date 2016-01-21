#ifndef NET_PROTOCOL_H
#define NET_PROTOCOL_H

#include <iostream>
#include <mtig/mtig_host.h>
#include <string.h>

//find the first appearance
int equal(char , char );
char *strsub(char*, char *);

//send the response to the target addr
char *Response();

//obtain the axis & angle
/*for return values :
  0 ok!Run instructions
  1 check online ? if true, you can send instruction
  2 undefined instructions
  3 unable to
  4 Finish
*/
int Analyze_order(char * , int * , int * );

//calculate the offset from the mtig & the constrction
int Calculate_offset(int, int);

#endif // NET_PROTOCOL_H
