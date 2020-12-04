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

#pragma once

#include <fstream>
#include <string>
#include <map>
#include <vector>

#include "MRKCommon.h"
#include "MRKXCPPStructs.h"

namespace MRK {
	struct MRKCodeStream {
		mrks ofstream Stream;
		mrku32 IndentCount;
		bool IsDirty;
		mrks string ClassPath;
		MRKXCPPClass* Class;
	};

	class MRKCodeWriter {
	private:
		mrks map<mrks string, MRKCodeStream> m_OpenedStreams;
		MRKCodeStream* m_CurrentStream;
		mrks string m_ParentDir;
		mrks ofstream m_InitializeStream;
		mrks vector<mrks string> m_CodeClassPaths;
		mrks vector<mrks string> m_RegParams;
		mrks vector<mrks string> m_Images;

		void WriteLine(mrks string line);
		mrks string Replicate(char c, mrku32 times);
		mrks string Replicate(mrks string c, mrku32 times);
		void Increment();
		void Decrement();
		mrks string Replace(mrks string orig, mrks string from, mrks string to);
		void RegParam(mrks string& param);

	public:
		MRKCodeWriter(mrks string dir);
		void OpenClass(MRKXCPPClass* clazz);
		void WriteMethod(MRKXCPPMethod* method, bool sttic);
		void WriteField(MRKXCPPField* field);
		void CloseClass();
		void CloseWriter();
	};
}