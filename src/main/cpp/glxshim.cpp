#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include <EGL/egl.h>

eglFuncPointer (*sys_eglGetProcAddress)(const char* proc);

extern "C" {
    typedef void (*__eglMustCastToProperFunctionPointerType)(void);

    typedef __eglMustCastToProperFunctionPointerType (*eglGetProcAddress_ptr_t)(const char *);
}

struct context_t {
    context_t() {
        char* eglName = getenv("POJAVEXEC_EGL");
        if (!eglName)
            eglName = getenv("LIBEGL_NAME");
        if (!eglName)
            eglName = getenv("LIBGL_EGL");
        if (eglName == nullptr) {
            printf("GLXShim: context init failed: EGL lib envvar not found!\n");
            return;
        }
        dl_handle = dlopen(eglName, RTLD_LOCAL|RTLD_LAZY);
        sys_eglGetProcAddress =
                (eglGetProcAddress_ptr_t)dlsym(dl_handle, "eglGetProcAddress");
        if (sys_eglGetProcAddress == nullptr) {
            printf("GLXShim: context init failed: %s\n", dlerror());
        }
    }

    ~context_t() {
        dlclose(dl_handle);
    }
    void* dl_handle = nullptr;
    eglGetProcAddress_ptr_t sys_eglGetProcAddress = nullptr;
};

extern "C" {

__attribute__((visibility("default"))) void* glXGetProcAddress(const char *name) {
    static context_t ctx;
    void* pfunc = (void*)ctx.sys_eglGetProcAddress(name);
    if(!func){
        fprintf(stderr, "GLXShim: Unknown function %s!\n", proc);
        return nullptr;
    }
    return pfunc;
}

__attribute__((visibility("default"))) void* glXGetProcAddressARB(const char *name) {
    return glXGetProcAddress(name);
}

__attribute__((visibility("default"))) void* eglGetProcAddress(const char *name) {
     return glXGetProcAddress(name);
}
