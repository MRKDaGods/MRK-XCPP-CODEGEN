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

#include "MRKCodeWriter.h"
#include "Concat.hpp"

#include <filesystem>
#include <Shlobj.h>

namespace MRK {
	void MRKCodeWriter::WriteLine(mrks string line) {
		m_CurrentStream->Stream << Replicate('\t', m_CurrentStream->IndentCount) << line << '\n';
	}

	mrks string MRKCodeWriter::Replicate(char c, mrku32 times) {
		mrks string str;
		for (mrku32 i = 0; i < times; i++)
			str += c;

		return str;
	}

	mrks string MRKCodeWriter::Replicate(mrks string c, mrku32 times) {
		mrks string str;
		for (mrku32 i = 0; i < times; i++)
			str += c;

		return str;
	}

	void MRKCodeWriter::Increment() {
		m_CurrentStream->IndentCount++;
	}

	void MRKCodeWriter::Decrement() {
		if (m_CurrentStream->IndentCount > 0)
			m_CurrentStream->IndentCount--;
	}

	mrks string MRKCodeWriter::Replace(mrks string orig, mrks string from, mrks string to) {
		mrku32 sz = orig.size();
		for (mrku32 i = 0; i < sz; i++) {
			if (!orig.substr(i).find(from)) {
				orig = orig.replace(i, from.size(), to);
				sz += to.size() - from.size();
			}
		}

		return orig;
	}

	void MRKCodeWriter::RegParam(mrks string& param) {
		if (!(MRK_VEC_CONTAIN(m_RegParams, param)))
			m_RegParams.push_back(param);
	}

	MRKCodeWriter::MRKCodeWriter(mrks string dir) {
		m_CurrentStream = 0;
		m_ParentDir = dir;
		m_InitializeStream = mrks ofstream(concat(dir, "\\MRKXCPPInit.cpp"), mrks ios_base::out);
	}

	void MRKCodeWriter::OpenClass(MRKXCPPClass* clazz) {
		//find class path
		mrks string nms = "";
		if (clazz->Namespace && strlen(clazz->Namespace))
			nms = '\\' + Replace(mrks string(clazz->Namespace), ".", "\\");

		mrks string classDir = m_ParentDir + nms;
		mrks string classPath = concat(classDir, "\\", Replace(clazz->Name, "`", "_gctx"), ".hpp");

		auto x = m_OpenedStreams.find(classPath);
		if (x == m_OpenedStreams.end()) {
			/*mrku32 lastidx = 0;

			while (true) {
				mrks string realScan = classDir.substr(0, 
					classDir.substr(lastidx).find_first_of('\\') + lastidx);

				if (!mrksfs is_directory(realScan))
					mrksfs create_directory(realScan);

				lastidx += realScan.size();

				if (lastidx >= classDir.size())
					break;
			}*/


			if (!mrksfs is_directory(classDir))
				SHCreateDirectoryExA(0, classDir.c_str(), 0);
				//mrksfs create_directory(classDir);

			m_OpenedStreams.insert(mrks make_pair(classPath, MRKCodeStream{
				mrks ofstream(classPath, mrks ios_base::out),
				0,
				false,
				classPath,
				clazz
			}));

			m_CurrentStream = &m_OpenedStreams.at(classPath);
		}
		else
			m_CurrentStream = &x->second;

		if (!m_CurrentStream->IsDirty) {
			//write basic info to stream
			WriteLine("//GENERATED BY MRK XCPP CODEGEN");

			mrku32 nmsDepthC;

			if (!clazz->Namespace || !strlen(clazz->Namespace))
				nmsDepthC = 0;
			else {
				nmsDepthC = 1;
				for (mrku32 i = 0; i < strlen(clazz->Namespace); i++) {
					if (clazz->Namespace[i] == '.')
						nmsDepthC++;
				}
			}

			mrks string depth = Replicate("../", nmsDepthC);
			WriteLine(concat("\n#include \"", depth, "MRKXCPPGen.h\""));
			WriteLine(concat("#include \"", depth, "MRKXCPPTypes.h\"\n"));
			WriteLine(concat("namespace ", Replace(clazz->Namespace, ".", "::"), " {"));
			Increment();

			m_CurrentStream->IsDirty = true;
		}
		else {
			// 2!!!!!
			//m_CurrentStream->Stream
			//stream opened before, LATE FEATURE
			mrks _Xruntime_error(concat("Stream '", m_CurrentStream->ClassPath, "' has been opened before!").c_str());
		}

		WriteLine(concat("class ", Replace(clazz->Name, "`", "_gctx"), " {"));

		WriteLine("public:");
		Increment();

		WriteLine("static void* __class;");

		Decrement();

		WriteLine("public:");

		//write constructor
		Increment();
		WriteLine("static void* __new(void** args = 0, unsigned int argc = 0) {");
		Increment();

		WriteLine("__protect();");

		WriteLine("return MRKRuntimeInvokeCtor(__class, args, argc);");

		WriteLine("__end();");

		Decrement();
		WriteLine("}");
	}

	void MRKCodeWriter::WriteMethod(MRKXCPPMethod* method, bool sttic) {
		mrks string paramStr;
		mrks string invokeStr;
		for (mrku32 i = 0; i < method->ParamCount; i++) {
			mrks string param = Replace(Replace(method->Params[i], ".", "_"), "[]", "_ARRAY");
			RegParam(param);

			paramStr += concat(param, " arg", i, ", ");
			invokeStr += concat("arg", i);

			if (i < method->ParamCount - 1)
				invokeStr += ", ";
		}

		paramStr += "void* instance = 0";

		WriteLine(concat("static void* ", method->Name, "(", paramStr, ") {"));
		Increment();

		WriteLine("__protect();");

		WriteLine(concat("return MRKRuntimeInvoke(__class, E(\"", method->Name, "\"), N(" , method->ParamCount, 
			"), instance, new void*[", method->ParamCount, "] { ", invokeStr, " }, ", sttic ? "true" : "false", ", N(", method->Occurance, "));"));

		WriteLine("__end();");

		Decrement();
		WriteLine("}");
	}

	void MRKCodeWriter::WriteField(MRKXCPPField* field) {
		WriteLine(concat("static void* m", Replace(field->Name, "`", "_gctx"), "(void* instance = 0) {"));
		Increment();

		WriteLine("__protect();");

		WriteLine(concat("return MRKRuntimeGetFieldValue(__class, E(\"", field->Name, "\"), instance);"));

		WriteLine("__end();");

		Decrement();
		WriteLine("}");
	}

	void MRKCodeWriter::CloseClass() {
		Decrement();
		WriteLine("};");
		Decrement();
		WriteLine("}");

		//x::y::__class = 0;
		WriteLine(concat("inline void* ", Replace(m_CurrentStream->Class->Namespace, ".", "::"), 
			"::", Replace(m_CurrentStream->Class->Name, "`", "_gctx"), "::__class = 0;"));

		m_CodeClassPaths.push_back(Replace(m_CurrentStream->ClassPath.substr(m_ParentDir.size() + 1), "\\", "/"));
		m_Images.push_back(m_CurrentStream->Class->Image->Name);

		m_CurrentStream->Stream.close();
		m_OpenedStreams.erase(m_CurrentStream->ClassPath);

		m_CurrentStream = 0;
	}

	void MRKCodeWriter::CloseWriter() {
		m_InitializeStream << "//GENERATED BY MRK XCPP CODEGEN\n\n";

		m_InitializeStream << "#include \"MRKXCPPGen.h\"\n";

		for (mrks string& path : m_CodeClassPaths)
			m_InitializeStream << "#include \"" << path << "\"\n";

		m_InitializeStream << "\nnamespace MRK {\n\tvoid MRK_XCPP_INIT() {\n";

		for (mrks string& path : m_CodeClassPaths) {
			mrks string cpC = Replace(Replace(path, ".hpp", ""), "/", "::"); //x::y
			mrku32 spLast = cpC.find_last_of("::");
			mrks string monoNms = Replace(cpC.substr(0, spLast - 1), "::", ".");
			mrks string monoClass = Replace(cpC.substr(spLast + 1), "_gctx", "`");

			m_InitializeStream << "\t\t"
				<< cpC
				<< "::__class = MRKRuntimeGetClass(E(\""
				<< monoNms
				<< "\"), E(\""
				<< monoClass
				<< "\"), E(\""
				<< m_Images.front()
				<< "\"));"
				<< '\n';

			m_Images.erase(m_Images.begin());
		}

		m_InitializeStream << "\t}\n}";

		m_InitializeStream.close();

		m_InitializeStream = mrks ofstream(concat(m_ParentDir, "\\MRKXCPPInit.h"), mrks ios_base::out);
		m_InitializeStream << "//GENERATED BY MRK XCPP CODEGEN\n\n"
			<< "#pragma once\n\n"
			<< "namespace MRK {\n"
			<< "\t void MRK_XCPP_INIT();\n"
			<< "}";

		m_InitializeStream.close();

		m_InitializeStream = mrks ofstream(concat(m_ParentDir, "\\MRKXCPPTypes.h"), mrks ios_base::out);
		m_InitializeStream << "//GENERATED BY MRK XCPP CODEGEN\n\n"
			<< "#pragma once\n\n";

		for (mrks string& regParam : m_RegParams)
			m_InitializeStream << "typedef void* " << regParam << ";\n";

		m_InitializeStream.close();
	}
}
