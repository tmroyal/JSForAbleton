/**
	@file
	quickjs - just like print

	@ingroup	examples
*/

#include "ext.h"
#include "ext_obex.h"
#include "ext_time.h"
#include "ext_itm.h"

#include "quickjs.h"

#include "qjs_interp.h"

////////////////////////// object struct
typedef struct _quickjs
{
	t_object	ob;
    qjs_interp   *qjs;
    void		*out;
} t_quickjs;

/// next we see if we can create a self generating
/// stream of ticks
/// - with an attr
/// - without an attr

///////////////////////// function prototypes
//// standard set
void *quickjs_new(t_symbol *s, long argc, t_atom *argv);
void quickjs_assist(t_quickjs *x, void *b, long m, long a, char *s);
void quickjs_free(t_quickjs *x);
void quickjs_anything(t_quickjs *x, t_symbol *s, long ac, t_atom *av);

//////////////////////// global class pointer variable
void *quickjs_class;

void ext_main(void *r)
{
	t_class *c;
    
    c = class_new("quickjs", (method)quickjs_new, (method)quickjs_free,
                  sizeof(t_quickjs), 0L, 0);

	class_addmethod(c, (method)quickjs_anything,		"anything",	A_GIMME, 0);
	class_addmethod(c, (method)quickjs_assist,			"assist",		A_CANT, 0);
    
	class_register(CLASS_BOX, c);
    quickjs_class = c;
}


void quickjs_assist(t_quickjs *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_INLET) { //inlet
        sprintf(s, "Does nothing");
	}
}

void quickjs_anything(t_quickjs *x, t_symbol *s, long ac, t_atom *av)
{
    post("anything");
}

void *quickjs_new(t_symbol *s, long argc, t_atom *argv)
{
	t_quickjs *x = NULL;
    t_atom a;
    
    x = (t_quickjs *)object_alloc(quickjs_class);
    
    create_interp(x->qjs);;
    set_glob_obj((t_object*)x);
    
	return (x);
}

void quickjs_free(t_quickjs *x){
    destroy_interp(x->qjs);
}
