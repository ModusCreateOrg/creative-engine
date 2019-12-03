#ifndef UTILS_H
#define UTILS_H

#include "../../src/BTypes.h"

// TODO: add these to CE proper
extern void HexDump(TUint8 *ptr, TInt length, TInt width=8);
extern void HexDump(TUint16 *ptr, TInt length, TInt width=8);
extern void HexDump(TUint32 *ptr, TInt length, TInt width=8);

extern char *skipbl(char *p);
extern char *parse_token(char *dst, char *src);
extern char *trim(char *p);
extern void symbol_name(char *dst, char *base);

// XML helpers

// parse xml attribute, e.g. foo="bar", into attr[] = foo, value = bar
extern void parse_attr(char *s, char *attr, char *value);
// parse xml value for given attribute - e.g. given attribute "foo" and xml contains foo="bar", then value is set to "bar"
extern TBool parse_value(char *s, const char *attribute, char *value);

#endif
