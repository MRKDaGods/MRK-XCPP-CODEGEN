/*
 * Copyright (c) 2020, Mohamed Ammar <mamar452@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "MRKCommon.h"

#ifdef MRK_XCPP_MONO

#include "MRKXCPPBackend.h"
#include "MRKAlloc.hpp"
#include "MRKLog.h"
#include "Concat.hpp"

#include <Windows.h>

namespace MRK {
	typedef void* (*DynFunction)(...);

    DynFunction mono_class_from_name;
    DynFunction mono_get_root_domain;
    DynFunction mono_thread_attach;
    DynFunction mono_image_open_from_data;
    DynFunction mono_assembly_name_new;
    DynFunction mono_assembly_loaded;
    DynFunction mono_assembly_get_image;
    DynFunction mono_class_get_method_from_name;
    DynFunction mono_runtime_invoke;
    DynFunction mono_domain_assembly_open;
    DynFunction mono_assembly_foreach;
    DynFunction mono_image_get_name;
    DynFunction mono_method_signature;
    DynFunction mono_signature_get_param_count;
    DynFunction mono_signature_get_params;
    DynFunction mono_type_get_class;
    DynFunction mono_class_get_name;
    DynFunction mono_class_get_namespace;
    DynFunction mono_class_get_image;
    DynFunction mono_type_get_name;
    DynFunction mono_class_get_field_from_name;
    DynFunction mono_class_get_methods;
    DynFunction mono_method_get_name;

    void* ms_RootDomain;
    const char* ms_CachedImageName;
    void* ms_CachedImage;

	void*** ms_IndexPointerMapping = new void** [MONO_FUNCTION_COUNT] {
            (void**)&mono_class_from_name,
            (void**)&mono_get_root_domain,
            (void**)&mono_thread_attach,
            (void**)&mono_image_open_from_data,
            (void**)&mono_assembly_name_new,
            (void**)&mono_assembly_loaded,
            (void**)&mono_assembly_get_image,
            (void**)&mono_class_get_method_from_name,
            (void**)&mono_runtime_invoke,
            (void**)&mono_domain_assembly_open,
            (void**)&mono_assembly_foreach,
            (void**)&mono_image_get_name,
            (void**)&mono_method_signature,
            (void**)&mono_signature_get_param_count,
            (void**)&mono_signature_get_params,
            (void**)&mono_type_get_class,
            (void**)&mono_class_get_name,
            (void**)&mono_class_get_namespace,
            (void**)&mono_class_get_image,
            (void**)&mono_type_get_name,
            (void**)&mono_class_get_field_from_name,
            (void**)&mono_class_get_methods,
            (void**)&mono_method_get_name
	};

    const char** ms_FunctionSyms = new const char* [MONO_FUNCTION_COUNT] {
            "mono_class_from_name",
            "mono_get_root_domain",
            "mono_thread_attach",
            "mono_image_open_from_data",
            "mono_assembly_name_new",
            "mono_assembly_loaded",
            "mono_assembly_get_image",
            "mono_class_get_method_from_name",
            "mono_runtime_invoke",
            "mono_domain_assembly_open",
            "mono_assembly_foreach",
            "mono_image_get_name",
            "mono_method_signature",
            "mono_signature_get_param_count",
            "mono_signature_get_params",
            "mono_type_get_class",
            "mono_class_get_name",
            "mono_class_get_namespace",
            "mono_class_get_image",
            "mono_type_get_name",
            "mono_class_get_field_from_name",
            "mono_class_get_methods",
            "mono_method_get_name"
    };

	bool MRKXCPPBackendInit(const char* moduleName) {
        HMODULE lib = GetModuleHandleA(moduleName);
        if (!lib)
            return false;

        for (mrku32 idx = 0; idx < MONO_FUNCTION_COUNT; idx++) {
            *ms_IndexPointerMapping[idx] = (void*)GetProcAddress(lib, ms_FunctionSyms[idx]);
        }

        return true;
	}

    void*** MRKXCPPBackendGetPointers(mrku32* sz) {
        if (sz)
            *sz = MONO_FUNCTION_COUNT;

        return ms_IndexPointerMapping;
    }

    void MRKMonoThreadAttach() {
        if (!ms_RootDomain)
            ms_RootDomain = mono_get_root_domain();

        mono_thread_attach(ms_RootDomain);
    }

    void MRKMonoAssemblyIterator(void* assembly, void*)
    {
        if (ms_CachedImage)
            return;

        void* img = mono_assembly_get_image(assembly);
        const char* name = (const char*)mono_image_get_name(img);

        if (!strcmp(name, ms_CachedImageName)) {
            ms_CachedImage = img;
        }
    }

    void MRKCopyString(char** dest, const char* src) {
        if (!dest || !src)
            return;

        mrku32 nLen = strlen(src);
        *dest = MRKAllocNewArr<char>(nLen + 1);
        strcpy(*dest, src);
        (*dest)[nLen] = '\0';
    }

    MRKXCPPImage* MRKXCPPBackendGetImage(const char* name) {
        MRKMonoThreadAttach();

        ms_CachedImageName = name;
        ms_CachedImage = 0;
        mono_assembly_foreach(MRKMonoAssemblyIterator, 0);

        if (!ms_CachedImage)
            return 0;

        MRKXCPPImage* image = MRKAllocNew<MRKXCPPImage>();
        image->Ptr = ms_CachedImage;

        MRKCopyString(&image->Name, name);

        return image;
    }

    MRKXCPPClass* MRKXCPPBackendGetClass(MRKXCPPImage* image, const char* namespaze, const char* name) {
        MRKMonoThreadAttach();

        if (!image)
            return 0;

        void* clazz = mono_class_from_name(image->Ptr, namespaze, name);
        if (!clazz)
            return 0;

        MRKXCPPClass* mclass = MRKAllocNew<MRKXCPPClass>();
        mclass->Ptr = clazz;
        mclass->Image = image;

        MRKCopyString(&mclass->Namespace, namespaze);
        MRKCopyString(&mclass->Name, name);

        return mclass;
    }

    MRKXCPPMethod* MRKXCPPBackendGetMethod(MRKXCPPClass* clazz, const char* name, int argc, int occ) {
        MRKMonoThreadAttach();

        if (!clazz)
            return 0;

        void* method = 0;
        void* __sig = 0;
        if (occ == 1)
            method = mono_class_get_method_from_name(clazz->Ptr, name, argc);
        else {
            void* iter = 0;
            int _occ = 0;
            while (method = mono_class_get_methods(clazz->Ptr, &iter)) {
                void* __sig = mono_method_signature(method);
                if (!strcmp((const char*)mono_method_get_name(method), name) && 
                    (mrku32)mono_signature_get_param_count(__sig) == argc) {
                    _occ++;

                    if (_occ == occ)
                        break;
                }
            }
        }

        if (!method)
            return 0;

        MRKXCPPMethod* mmethod = MRKAllocNew<MRKXCPPMethod>();
        mmethod->Ptr = method;
        mmethod->Class = clazz;
        mmethod->Occurance = occ;

        MRKCopyString(&mmethod->Name, name);

        void* sig = mono_method_signature(method);
        mmethod->ParamCount = (mrku32)mono_signature_get_param_count(sig);
        mmethod->Params = MRKAllocNewArr<char*>(mmethod->ParamCount);

        void* typeiter = 0;
        void* currentType = 0;
        mrku32 idx = 0;
        while (currentType = mono_signature_get_params(sig, &typeiter)) {
            MRKCopyString(&mmethod->Params[idx++], (char*)mono_type_get_name(currentType));
        }

        return mmethod;
    }

    MRKXCPPField* MRKXCPPBackendGetField(MRKXCPPClass* clazz, const char* name) {
        MRKMonoThreadAttach();

        if (!clazz)
            return 0;

        void* field = mono_class_get_field_from_name(clazz->Ptr, name);
        if (!field)
            return 0;

        MRKXCPPField* mfield = MRKAllocNew<MRKXCPPField>();
        mfield->Ptr = field;
        mfield->Class = clazz;

        MRKCopyString(&mfield->Name, name);

        return mfield;
    }
}

#endif