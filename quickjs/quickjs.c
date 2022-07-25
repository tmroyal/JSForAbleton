/**
	@file
	printr - just like print

	@ingroup	examples
*/

#include "ext.h"
#include "ext_obex.h"
#include "ext_time.h"
#include "ext_itm.h"

////////////////////////// object struct
typedef struct _printr
{
	t_object	ob;
    t_object    *time_obj;
	void		*out;
} t_printr;

/// next we see if we can create a self generating
/// stream of ticks
/// - with an attr
/// - without an attr

///////////////////////// function prototypes
//// standard set
void *printr_new(t_symbol *s, long argc, t_atom *argv);
void printr_assist(t_printr *x, void *b, long m, long a, char *s);
void printr_free(t_printr *x);
void printr_tick(t_printr *x);
void printr_anything(t_printr *x, t_symbol *s, long ac, t_atom *av);

//////////////////////// global class pointer variable
void *printr_class;

void ext_main(void *r)
{
	t_class *c;

    c = class_new("printr", (method)printr_new, (method)printr_free,
                  sizeof(t_printr), 0L, 0);

	class_addmethod(c, (method)printr_anything,		"anything",	A_GIMME, 0);
	class_addmethod(c, (method)printr_assist,			"assist",		A_CANT, 0);
    
    class_time_addattr(c, "delaytime", "Delay Time", TIME_FLAGS_TICKSONLY | TIME_FLAGS_USECLOCK | TIME_FLAGS_TRANSPORT);
    
	class_register(CLASS_BOX, c);
	printr_class = c;
}


void printr_assist(t_printr *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_INLET) { //inlet
        sprintf(s, "Does nothing");
	}
}

void printr_anything(t_printr *x, t_symbol *s, long ac, t_atom *av)
{
    post("anything");
}

void printr_tick(t_printr *x){
    // TODO: post transport
    double n1, n2;
    double ticks;
    
    time_getphase(x->time_obj, &n1, &n2, &ticks);
    
    post("ticks: %f", ticks);
    time_schedule(x->time_obj, NULL);
}

void *printr_new(t_symbol *s, long argc, t_atom *argv)
{
	t_printr *x = NULL;
    t_atom a;
    
    x = (t_printr *)object_alloc(printr_class);
    
    x->time_obj = (t_object *) time_new((t_object*)x, NULL, (method)printr_tick, TIME_FLAGS_TICKSONLY | TIME_FLAGS_USECLOCK);
    
    atom_setsym(&a, gensym("1n"));
    time_setvalue(x->time_obj, NULL, 1, &a);
    
    time_schedule(x->time_obj, NULL);
    
	return (x);
}

void printr_free(t_printr *x){
    freeobject(x->time_obj);
}
