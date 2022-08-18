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
    char* filename;
    char **code;
    long code_size;
    bool code_loaded;
    t_object *editor;
} t_quickjs;

/// next we see if we can create a self generating
/// stream of ticks
/// - with an attr
/// - without an attr

typedef enum
{
    DO_NOTHING = (1u << 0),
    INTERPRET = (1u << 1),
    SET_EDITOR_TEXT = (1u << 2)
} loadAction;

///////////////////////// function prototypes
//// standard set
void *quickjs_new(t_symbol *s, long argc, t_atom *argv);
void quickjs_assist(t_quickjs *x, void *b, long m, long a, char *s);
void quickjs_free(t_quickjs *x);
void quickjs_dblclick(t_quickjs *x);
void quickjs_edclose(t_quickjs *x, char **ht, long size);
long quickjs_edsave(t_quickjs *x, char **ht, long size);
void quickjs_compile(t_quickjs *x, t_symbol *s);
void quickjs_wclose(t_quickjs *x);
void quickjs_newfile(t_quickjs *x);
void quickjs_opendefault(t_quickjs *x);

short verify_fileexists(t_quickjs *x, t_symbol* filename_ss);
short load_file(t_quickjs *x, t_symbol* filename_s, short path);
void read_file(t_quickjs *x, t_symbol* filename_s, t_atom onload);
void set_editor_text(t_quickjs* x);


//////////////////////// global class pointer variable
void *quickjs_class;

void ext_main(void *r)
{
	t_class *c;
    
    c = class_new("quickjs", (method)quickjs_new, (method)quickjs_free,
                  sizeof(t_quickjs), 0L, A_GIMME, 0);

	class_addmethod(c, (method)quickjs_assist, "assist", A_CANT, 0);
    class_addmethod(c, (method)quickjs_dblclick, "dblclick", A_CANT, 0);
    class_addmethod(c, (method)quickjs_dblclick, "open", A_CANT, 0);
    class_addmethod(c, (method)quickjs_edsave, "wclose", A_CANT, 0);
    class_addmethod(c, (method)quickjs_compile, "compile", A_DEFSYM, 0);
    class_addmethod(c, (method)quickjs_edclose, "edclose", A_CANT, 0);
    class_addmethod(c, (method)quickjs_edsave, "edsave", A_CANT, 0);
    class_addmethod(c, (method)quickjs_opendefault, "editor", 0);
    
	class_register(CLASS_BOX, c);
    quickjs_class = c;
}

void quickjs_dblclick(t_quickjs *x){
    if (!x->editor){
        x->editor = object_new(CLASS_NOBOX, gensym("jed") ,(t_object*) x, 0);
        object_attr_setsym(x->editor, gensym("title"), gensym("QuickJS (ES2020)"));
        if (x->filename != NULL){
            t_atom onload;
            t_atom filename;
            
            atom_setlong(&onload, SET_EDITOR_TEXT);
            defer((t_object*)x, (method)read_file, gensym(x->filename), 1, &onload);
        }
    } else {
        object_attr_setchar(x->editor, gensym("visible"), 1);
    }
}

void quickjs_opendefault(t_quickjs *x){
    t_fourcc filetype = "TEXT";
    t_fourcc outtype;
    char filename[MAX_PATH_CHARS];
    char fullpath[MAX_PATH_CHARS];
    char conformed_path[MAX_PATH_CHARS];
    char command[MAX_PATH_CHARS];
    short path;
    strncpy_zero(filename, x->filename, strlen(sym->s_name)+1);
    
    if (path_topathname(path_getdefault(), filename, fullpath)){
        object_error((t_object*)x, "%s not found", filename);
        return;
    } else {
        post(fullpath);
    }
    
    if (path_nameconform(fullpath, conformed_path, PATH_STYLE_NATIVE_PLAT, PATH_TYPE_TILDE)){
        object_error((t_object*)x, "%s not conform", filename);
        return;
    } else {
        sprintf(command, "open %s", conformed_path);
        system(command);
    }
    
}

void quickjs_wclose(t_quickjs *x){
    if (x->editor){
        object_method(x->editor, gensym("wclose"));
    }
}

void quickjs_edclose(t_quickjs *x, char **ht, long size)
{
    x->editor = NULL;
}

long quickjs_edsave(t_quickjs *x, char **ht, long size){
    // TODO: defer me
    t_symbol filename;
    object_attr_getsym(&filename, gensym("filename"));
    JSValue result;
    
    size_t fn_len = strlen(filename.s_name);
    
    if (fn_len > 0){
        strncpy_zero(x->filename, filename.s_name, fn_len);
        strncpy_zero(*(x->code), *ht, size);
        x->code_size = size;
        x->code_loaded = true;
        result = interp_code(x->qjs, *(x->code), x->code_size);
    }
    
    return 0;
}

void quickjs_compile(t_quickjs *x, t_symbol* s){
    // TODO: defer me
    // TODO: no file loaded
    int hasFileArg = strlen(s->s_name);
    JSValue result;
    
    if (!hasFileArg){
        if (x->code_loaded){
            result = interp_code(x->qjs, *(x->code), x->code_size);
        } else {
            object_warn((t_object*)x, "Cannot compile: no code");
        }
    } else {
        unsigned int loadFlags = INTERPRET;
        t_atom onLoad;
        
        if (x->editor != NULL){ loadFlags &= SET_EDITOR_TEXT; }
        atom_setlong(&onLoad, loadFlags);
        
        defer((t_object*)x, (method)read_file, gensym(x->filename), 1, &onLoad);
    }
    
}

void quickjs_assist(t_quickjs *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET) { //inlet
        sprintf(s, "Does nothing");
	}
}


short verify_fileexists(t_quickjs *x, t_symbol* filename_s){
    t_fourcc filetype = "TEXT";
    t_fourcc outtype;
    char filename[MAX_PATH_CHARS];
    short path;
    strncpy_zero(filename, filename_s->s_name, strlen(filename_s->s_name)+1);
    
    if (locatefile_extended(filename, &path, &outtype, &filetype, 1)){
        object_error((t_object*)x, "%s not found", filename);
        return path;
    } else {
        return 0;
    }
}

short load_file(t_quickjs *x, t_symbol* filename_s, short path){
    t_filehandle fh;
    
    if (path_opensysfile(filename_s->s_name, path, &fh, READ_PERM)){
        object_error((t_object*)x, "error opening %s", filename_s->s_name);
        return;
    }
    
    x->code = sysmem_newhandle(0);
    sysfile_readtextfile(fh, x->code, 0, TEXT_NULL_TERMINATE);
    sysfile_close(fh);
    return 1;
}

void read_file(t_quickjs *x, t_symbol* filename_s, t_atom onload){
    short path;
    int loadInd;
    
    JSValue result;
    
    // is this the place to do this?
    if (x->code_loaded){
        sysmem_freehandle(x->code);
        x->code_loaded = false;
        x->code_size = 0;
    }
    
    path = verify_fileexists(x,filename_s);
    if (path && load_file(x, filename_s, path)){
        loadInd = (int)atom_getlong(&onload);
        x->filename = filename_s->s_name;
        x->code_loaded = true;
        x->code_size = sysmem_handlesize(x->code);
        
        if (loadInd & INTERPRET){ result = interp_code(x->qjs, *(x->code), x->code_size); }
        if (loadInd & SET_EDITOR_TEXT){ set_editor_text(x); }
    }
}

void set_editor_text(t_quickjs* x){
    if (x->editor != NULL){
        object_method(x->editor, gensym("settext"), *(x->code), gensym("utf-8"));
    }
}

void *quickjs_new(t_symbol *s, long argc, t_atom *argv)
{
	t_quickjs *x = NULL;
    t_atom onLoad;
    atom_setlong(&onLoad, INTERPRET);
    
    x = (t_quickjs *)object_alloc(quickjs_class);
    
    x->code_loaded = false;
    x->qjs = create_interp();
    x->editor = NULL;
    set_glob_obj((t_object*)x);
    setup_console(x->qjs);
    
    if (argc > 0 && argv[0].a_type == A_SYM){
        defer((t_object*)x, (method)read_file, atom_getsym(&argv[0]), 1, &onLoad);
    }
    
	return (x);
}

void quickjs_free(t_quickjs *x){
    destroy_interp(x->qjs);
    if (x->code_loaded){
        sysmem_freehandle(x->code);
        free(x->filename);
    }
}
