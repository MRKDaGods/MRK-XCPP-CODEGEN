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

#include "MRKXCPP.h"
#include "MRKXCPPBackend.h"

#include <vector>

namespace MRK {
    mrks vector<MRKXCPPImage*> ms_Images;
    mrks vector<MRKXCPPClass*> ms_Classes;
    mrks vector<MRKXCPPMethod*> ms_Methods;
    mrks vector<MRKXCPPField*> ms_Fields;

    MRKXCPPImage* MRK::MRKXCPPGetImage(const char* name) {
        for (MRKXCPPImage* image : ms_Images) {
            if (!strcmp(image->Name, name))
                return image;
        }

        MRKXCPPImage* image = MRKXCPPBackendGetImage(name);
        if (!image)
            return 0;

        ms_Images.push_back(image);
        return image;
    }

    MRKXCPPClass* MRKXCPPGetClass(MRKXCPPImage* image, const char* namespaze, const char* name) {
        for (MRKXCPPClass* clazz : ms_Classes) {
            if (clazz->Image == image && !strcmp(namespaze, clazz->Namespace) 
                && !strcmp(name, clazz->Name))
                return clazz;
        }

        MRKXCPPClass* clazz = MRKXCPPBackendGetClass(image, namespaze, name);
        if (!clazz)
            return 0;

        ms_Classes.push_back(clazz);
        return clazz;
    }

    MRKXCPPMethod* MRKXCPPGetMethod(MRKXCPPClass* clazz, const char* methodName, int argc, int occ) {
        for (MRKXCPPMethod* method : ms_Methods) {
            if (method->Class == clazz && !strcmp(method->Name, methodName) 
                && method->ParamCount == argc && method->Occurance == occ)
                return method;
        }

        MRKXCPPMethod* method = MRKXCPPBackendGetMethod(clazz, methodName, argc, occ);
        if (!method)
            return 0;

        ms_Methods.push_back(method);
        return method;
    }

    MRKXCPPField* MRKXCPPGetField(MRKXCPPClass* clazz, const char* fieldName) {
        for (MRKXCPPField* field : ms_Fields) {
            if (field->Class == clazz && !strcmp(field->Name, fieldName))
                return field;
        }

        MRKXCPPField* field = MRKXCPPBackendGetField(clazz, fieldName);
        if (!field)
            return 0;

        ms_Fields.push_back(field);
        return field;
    }
}
