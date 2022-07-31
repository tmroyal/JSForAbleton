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
    t_object *editor;
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
void quickjs_dblclick(t_quickjs *x);
void quickjs_edclose(t_quickjs *x, char **ht, long size);


//////////////////////// global class pointer variable
void *quickjs_class;

void ext_main(void *r)
{
	t_class *c;
    
    c = class_new("quickjs", (method)quickjs_new, (method)quickjs_free,
                  sizeof(t_quickjs), 0L, 0);

	class_addmethod(c, (method)quickjs_anything,		"anything",	A_GIMME, 0);
	class_addmethod(c, (method)quickjs_assist,			"assist",		A_CANT, 0);
    class_addmethod(c, (method)quickjs_dblclick,            "dblclick",        A_CANT, 0);
    class_addmethod(c, (method)quickjs_edclose, "edclose", A_CANT, 0);
    
	class_register(CLASS_BOX, c);
    quickjs_class = c;
}

void quickjs_dblclick(t_quickjs *x){
    if (!x->editor){
        x->editor = object_new(CLASS_NOBOX, gensym("jed") ,(t_object*) x, 0);
        object_attr_setsym(x->editor, gensym("title"), gensym("crazytext"));
    } else {
        object_attr_setchar(x->editor, gensym("visible"), 1);
    }
}

void quickjs_edclose(t_quickjs *x, char **ht, long size)
{
    JSValue result = interp_code(x->qjs, *ht, size);
    post("done");
    // do something with the text
    x->editor = NULL;
}


void quickjs_assist(t_quickjs *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_INLET) { //inlet
        sprintf(s, "Does nothing");
	}
}

void quickjs_anything(t_quickjs *x, t_symbol *s, long ac, t_atom *av)
{
    JSValue result = interp_code(x->qjs, "console.log('hello world', 'im here')", -1);
    result = interp_code(x->qjs, "console.error('hello world')", -1);
    result = interp_code(x->qjs, "console.warn('hello world')", -1);
    
    //if (JS_IsError(x->qjs->ctx, result)){}
}

void *quickjs_new(t_symbol *s, long argc, t_atom *argv)
{
	t_quickjs *x = NULL;
    t_atom a;
    
    x = (t_quickjs *)object_alloc(quickjs_class);
    
    x->qjs = create_interp();
    x->editor = NULL;
    set_glob_obj((t_object*)x);
    setup_console(x->qjs);
    
	return (x);
}

void quickjs_free(t_quickjs *x){
    destroy_interp(x->qjs);
}
