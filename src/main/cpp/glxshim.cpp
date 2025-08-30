#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>


extern "C" {
    typedef void (*__eglMustCastToProperFunctionPointerType)(void);

    typedef __eglMustCastToProperFunctionPointerType (*eglGetProcAddress_ptr_t)(const char *);
}

struct context_t {
    context_t() {
        char* eglName = getenv("POJAVEXEC_EGL");
        dl_handle = dlopen(eglName, RTLD_LOCAL|RTLD_LAZY);
        eglGetProcAddress =
                (eglGetProcAddress_ptr_t)dlsym(dl_handle, "eglGetProcAddress");
        if (eglGetProcAddress == nullptr) {
            printf("GLXShim: context init failed: %s\n", dlerror());
        }
    }

    ~context_t() {
        dlclose(dl_handle);
    }
    void* dl_handle = nullptr;
    eglGetProcAddress_ptr_t eglGetProcAddress = nullptr;
};

extern "C" {

__attribute__((visibility("default"))) void *glXGetProcAddress(const char *name) {
    static context_t ctx;
    void* pfunc = (void*)ctx.eglGetProcAddress(name);
    return pfunc;
}

}
