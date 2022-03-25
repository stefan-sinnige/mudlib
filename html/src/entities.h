#ifndef _MUDLIB_HTML_ENTITIES_H_
#define _MUDLIB_HTML_ENTITIES_H_

struct entity_t
{
    const char* named;
    const char* utf8;
};

extern const struct entity_t entities[];
extern int compressed[];
extern int shifts[];
extern int checks[];

#endif /*  _MUDLIB_HTML_ENTITIES_H_ */
