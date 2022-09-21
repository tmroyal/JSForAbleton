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

#include "qjs_struct.h"
#include "qjs_interp.h"

////////////////////////// object struct

/// next we see if we can create a self generating
/// stream of ticks
/// - with an attr
/// - without an attr


///////////////////////// function prototypes
//// standard set
void *quickjs_new(t_symbol *s, long argc, t_atom *argv);
void quickjs_assist(t_quickjs *x, void *b, long m, long a, char *s);
void quickjs_free(t_quickjs *x);
void quickjs_interpret(t_quickjs *x, t_symbol *s);
void quickjs_opendefault(t_quickjs *x);
void quickjs_watch(t_quickjs *x, t_symbol* cmd, long argc, t_atom* argv);
void quickjs_filechanged(t_quickjs* x, char *filename, short path);


/// Inlet messages
void quickjs_bang(t_quickjs* x);
void quickjs_bang_cb(t_quickjs *x, t_symbol *s, long argc, t_atom *argv);


// file funcs
void read_file(t_quickjs *x, t_symbol* filename_s);
short set_path(t_quickjs *x, t_symbol* filename_s, short *outpath);
void set_fqn(t_quickjs *x); // fully qualified name
short load_file(t_quickjs *x, t_symbol* filename_s, short path);
void set_watcher(t_quickjs *x, int status);
void filechange(t_quickjs *x, t_symbol* s, short c, t_atom *v);

// time funcs
void quickjs_tick(t_quickjs * x);


//////////////////////// global class pointer variable
void *quickjs_class;

void ext_main(void *r)
{
	t_class *c;
    
    c = class_new("quickjs", (method)quickjs_new, (method)quickjs_free,
                  sizeof(t_quickjs), 0L, A_GIMME, 0);

    class_addmethod(c, (method)quickjs_bang, "bang", 0);
    
    class_addmethod(c, (method)quickjs_assist, "assist", A_CANT, 0);
    class_addmethod(c, (method)quickjs_interpret, "interp", A_DEFSYM, 0);
    class_addmethod(c, (method)quickjs_opendefault, "open", 0);
    class_addmethod(c, (method)quickjs_opendefault, "dblclick", 0);
    class_addmethod(c, (method)quickjs_watch, "watch", A_GIMME, 0);
    class_addmethod(c, (method)quickjs_filechanged, "filechanged", A_CANT, 0);
    
	class_register(CLASS_BOX, c);
    quickjs_class = c;
}

void quickjs_bang_cb(t_quickjs *x, t_symbol *s, long argc, t_atom *argv){
    JSValue res = interp_handle_bang((qjs_interp*)x->qjs);
    
}

void quickjs_bang(t_quickjs *x){
    schedule(x, (method)quickjs_bang_cb, 0, NULL, 0, NULL);
}



void quickjs_opendefault(t_quickjs *x){
#ifdef MAC_VERSION
    char format[] = "open \"%s\"";
#else
    char format[] = "start \"%s\"";
#endif
    char cmnd[MAX_PATH_CHARS+20];
    
    if (strlen(x->fqn)){
        sprintf(cmnd, format, x->fqn);
        system(cmnd);
    } else {
        object_post((t_object*)x, "Could not get path");
    }
}


void quickjs_interpret(t_quickjs *x, t_symbol* s){
    int hasFileArg = strlen(s->s_name);
    
    // TODO: interpret should reload existing file, not simply reinterpret code
    if (!hasFileArg){
        if (x->filename){
            defer((t_object*)x, (method)read_file, gensym(x->filename), 0, NULL);
        } else {
            object_warn((t_object*)x, "Cannot interpret: no code");
        }
    } else {
        defer((t_object*)x, (method)read_file, s, 0, NULL);
    }
    
}

void quickjs_filechanged(t_quickjs* x, char *filename, short path){
    defer((t_object*)x, (method)filechange, gensym("NULL"), 0, NULL);
}

void filechange(t_quickjs *x, t_symbol* s, short c, t_atom *v){
    
    if (load_file(x, gensym(x->filename), x->path)){
        x->code_size = strlen(*(x->code));
        interp_code(x, (qjs_interp*)x->qjs, *(x->code), x->code_size);
    } else {
        object_error((t_object*)x, "Error reloading file");
    }
}

void quickjs_watch(t_quickjs *x, t_symbol* cmd, long argc, t_atom* argv){
    if (argc > 1){
        object_error((t_object*)x, "Too many args for watch. Watch only accepts, at more, one arg.");
        return;
    } else if (argc == 1){
        if (atom_gettype(argv) != A_LONG){
            object_error((t_object*)x, "Watch only allows int args");
            return;
        }
        set_watcher(x, atom_getlong(argv));
    } else {
        set_watcher(x, 1);
    }
}

void set_watcher(t_quickjs *x, int status){
    if (status && x->filewatcher == NULL){
        x->filewatcher = filewatcher_new((t_object*)x, x->path, x->filename);
        filewatcher_start(x->filewatcher);
    } else if (!status && x->filewatcher != NULL){
        filewatcher_stop(x->filewatcher);
        object_free(x->filewatcher);
        x->filewatcher = NULL;
    }
}


void quickjs_assist(t_quickjs *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET) { //inlet
        sprintf(s, "Does nothing");
	}
}


short set_path(t_quickjs *x, t_symbol* filename_s, short* outpath){
    char filename[MAX_PATH_CHARS];
    short not_success, op;
    t_fourcc outtype;
    strncpy_zero(filename, filename_s->s_name, strlen(filename_s->s_name)+1);
    
    not_success = locatefile_extended(filename, &op, &outtype, NULL, 0);
    if (not_success){
        return 0;
    } else {
        (*outpath) = op;
        return 1;
    }
}

void set_fqn(t_quickjs *x){
    char abssyspath[MAX_PATH_CHARS];
    
    path_toabsolutesystempath(x->path, x->filename, abssyspath);
    path_nameconform(abssyspath, x->fqn, PATH_STYLE_NATIVE, PATH_TYPE_PATH);
}

short load_file(t_quickjs *x, t_symbol* filename_s, short path){
    t_filehandle fh;
    
    if (path_opensysfile(filename_s->s_name, x->path, &fh, READ_PERM)){
        object_error((t_object*)x, "error opening %s", filename_s->s_name);
        return;
    }
    
    x->code = sysmem_newhandle(0);
    sysfile_readtextfile(fh, x->code, 0, TEXT_NULL_TERMINATE);
    sysfile_close(fh);
    return 1;
}

void read_file(t_quickjs *x, t_symbol* filename_s){
    short success, path;
    int loadInd;
    char fqn[MAX_PATH_CHARS];
    
    // TODO: consider placing this in the file load func
    if (x->code_loaded){
        sysmem_freehandle(x->code);
        x->code_loaded = false;
        x->code_size = 0;
        x->path = 0;
        x->filename[0] = '\0';
        x->fqn[0] = '\0';
    }
    
    success = set_path(x, filename_s, &path);
    
    if (success && load_file(x, filename_s, &path)){
        strncpy_zero(x->filename, filename_s->s_name, strlen(filename_s->s_name)+1);
        x->code_loaded = true;
        x->code_size = strlen(*(x->code));
        x->path = path;
        
        set_fqn(x); // side effect sets fqn
        
        interp_code(x, (qjs_interp*)x->qjs, *(x->code), x->code_size);
    }
}


void *quickjs_new(t_symbol *s, long argc, t_atom *argv)
{
	t_quickjs *x = NULL;
    
    x = (t_quickjs *)object_alloc(quickjs_class);
    
    x->code_loaded = false;
    x->qjs = (struct qjs_interp*) create_interp();
    x->outlet = outlet_new((t_object*)x, NULL);
    x->proxy = proxy_new((t_object*)x,  1, &(x->inlet_num));

    
    if (argc > 0 && argv[0].a_type == A_SYM){
        defer((t_object*)x, (method)read_file, atom_getsym(&argv[0]), 0, NULL);
    }
    
	return (x);
}


void quickjs_free(t_quickjs *x){
    freeobject(x->proxy);
    
    destroy_interp((qjs_interp*)x->qjs);
    if (x->code_loaded){
        set_watcher(x, 0);
        sysmem_freehandle(x->code);
    }
}
