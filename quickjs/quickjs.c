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
    t_object    *time_obj;
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
void quickjs_tick(t_quickjs *x);
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
    
    class_time_addattr(c, "delaytime", "Delay Time", TIME_FLAGS_TICKSONLY | TIME_FLAGS_USECLOCK | TIME_FLAGS_TRANSPORT);
    
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

void quickjs_tick(t_quickjs *x){
    // TODO: post transport
    double n1, n2;
    double ticks;
    
    time_getphase(x->time_obj, &n1, &n2, &ticks);
    
    post("ticks: %f", ticks);
    time_schedule(x->time_obj, NULL);
}

void *quickjs_new(t_symbol *s, long argc, t_atom *argv)
{
	t_quickjs *x = NULL;
    t_atom a;
    
    x = (t_quickjs *)object_alloc(quickjs_class);
    
    x->time_obj = (t_object *) time_new((t_object*)x, NULL, (method)quickjs_tick, TIME_FLAGS_TICKSONLY | TIME_FLAGS_USECLOCK);
    
    atom_setsym(&a, gensym("1n"));
    time_setvalue(x->time_obj, NULL, 1, &a);
    
    time_schedule(x->time_obj, NULL);
    
	return (x);
}

void quickjs_free(t_quickjs *x){
    freeobject(x->time_obj);
}
