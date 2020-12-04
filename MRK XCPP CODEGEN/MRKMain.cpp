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

#include <Windows.h>
#include <filesystem>
#include <string>
#include <vector>

#include "MRKCommon.h"
#include "Concat.hpp"
#include "MRKLog.h"
#include "MRKAlloc.hpp"
#include "MRKXCPPBackend.h"
#include "MRKXCPP.h"
#include "MRKCodeWriter.h"

namespace MRK {
	struct MRKGenDataMethod {
		mrks string Name;
		mrku32 ParamCount;
		bool Static;
		int Occurance;
	};

	struct MRKGenDataField {
		mrks string Name;
	};

	struct MRKGenDataClass {
		mrks string Namespace;
		mrks string Name;
		mrks vector<MRKGenDataMethod> Methods;
		mrks vector<MRKGenDataField> Fields;
	};

	struct MRKGenDataAssembly {
		mrks string Name;
		mrks vector<MRKGenDataClass> Classes;
	};

#define GEN_CLASS(nms, name) MRKGenDataClass { #nms, #name, {
#define GEN_CLASS_END } }

#define GEN_METHOD(name, argc, sttic) MRKGenDataMethod { #name, argc, sttic, 1 }
#define GEN_METHOD_EX(name, argc, sttic, occ) MRKGenDataMethod { #name, argc, sttic, occ }

#define GEN_FIELD(name) MRKGenDataField { #name }

#define GEN_MORE }, {

#ifdef MRK_COMPILE_CS

	mrks vector<MRKGenDataAssembly> ms_GenAssemblies = {
		MRKGenDataAssembly{
			"System",
			{
				MRKGenDataClass {
					"Microsoft.CSharp",
					"CSharpCodeProvider",
					{
					}
				},
				MRKGenDataClass {
					"System.CodeDom.Compiler",
					"CodeDomProvider",
					{
						MRKGenDataMethod {
							"CompileAssemblyFromSource",
							2
						}
					}
				},
				MRKGenDataClass {
					"System.CodeDom.Compiler",
					"CompilerParameters",
					{
						MRKGenDataMethod {
							"set_GenerateExecutable",
							1
						},
						MRKGenDataMethod {
							"set_GenerateInMemory",
							1
						}
					}
				},
				MRKGenDataClass {
					"System.CodeDom.Compiler",
					"CompilerResults",
					{
						MRKGenDataMethod {
							"get_CompiledAssembly",
							0
						}
					}
				}
			}
		},
		MRKGenDataAssembly {
			"mscorlib",
			{
				MRKGenDataClass {
					"System.Reflection",
					"Assembly",
					{
						MRKGenDataMethod {
							"GetType",
							1
						}
					}
				}
			}
		}
	};

#endif

	mrks vector<MRKGenDataAssembly> ms_GenAssemblies = {
		MRKGenDataAssembly{
			"UnityEngine.CoreModule",
			{
				GEN_CLASS(UnityEngine, Object)
				GEN_METHOD(FindObjectOfType, 1, true),
				GEN_METHOD(DontDestroyOnLoad, 1, true),
				GEN_METHOD(Destroy, 1, true),
				GEN_METHOD(Instantiate, 1, true),
				GEN_METHOD(set_hideFlags, 1, false)
				GEN_CLASS_END,

				GEN_CLASS(UnityEngine, GameObject)
				GEN_METHOD(AddComponent, 1, false),
				GEN_METHOD(get_transform, 0, false)
				GEN_CLASS_END,

				GEN_CLASS(UnityEngine, Transform)
				GEN_METHOD(set_parent, 1, false),
				GEN_METHOD(SetAsLastSibling, 0, false),
				GEN_METHOD(get_position, 0, false),
				GEN_METHOD(get_forward, 0, false),
				GEN_METHOD(get_up, 0, false),
				GEN_METHOD(Internal_LookAt, 2, false)
				GEN_CLASS_END,

				GEN_CLASS(UnityEngine, RectTransform)
				GEN_METHOD(set_anchorMin, 1, false),
				GEN_METHOD(set_anchorMax, 1, false),
				GEN_METHOD(set_anchoredPosition, 1, false),
				GEN_METHOD(SetSizeWithCurrentAnchors, 2, false),
				GEN_METHOD(set_pivot, 1, false)
				GEN_CLASS_END,

				GEN_CLASS(UnityEngine, Resources)
				GEN_METHOD(FindObjectsOfTypeAll, 1, true)
				GEN_CLASS_END,

				GEN_CLASS(UnityEngine, LineRenderer)
				GEN_METHOD(set_positionCount, 1, false),
				GEN_METHOD(SetPosition, 2, false),
				GEN_METHOD(set_startWidth, 1, false),
				GEN_METHOD(set_endWidth, 1, false),
				GEN_METHOD(SetColors, 2, false),

				GEN_CLASS_END,

				GEN_CLASS(UnityEngine, Component)
				GEN_METHOD(get_transform, 0, false)
				GEN_CLASS_END,

				GEN_CLASS(UnityEngine, Camera)
				GEN_METHOD(get_main, 0, true),
				GEN_METHOD(WorldToScreenPoint, 1, false)
				GEN_CLASS_END,

				GEN_CLASS(UnityEngine, Material)
				GEN_METHOD(CreateWithShader, 2, true),
				GEN_METHOD(SetPass, 1, false),
				GEN_METHOD(get_shader, 0, false)
				GEN_CLASS_END,

				GEN_CLASS(UnityEngine, Shader)
				GEN_METHOD(Find, 1, true)
				GEN_CLASS_END,

				GEN_CLASS(UnityEngine, Renderer)
				GEN_METHOD(set_material, 1, false)
				GEN_CLASS_END,

				GEN_CLASS(UnityEngine, Texture2D)
				GEN_METHOD(Apply, 0, false),
				GEN_METHOD(SetPixel, 3, false),
				GEN_METHOD(LoadRawTextureDataImpl, 2, false),
				GEN_METHOD(GetPixel, 2, false)
				GEN_CLASS_END,

				GEN_CLASS(UnityEngine, Screen)
				GEN_METHOD(get_width, 0, true),
				GEN_METHOD(get_height, 0, true)
				GEN_CLASS_END,

				GEN_CLASS(UnityEngine, Time)
				GEN_METHOD(get_deltaTime, 0, true),
				GEN_METHOD(get_unscaledDeltaTime, 0, true)
				GEN_CLASS_END,

				GEN_CLASS(UnityEngine, Input)
				GEN_METHOD(GetMouseButton, 1, true),
				GEN_METHOD(get_mousePosition, 0, true),
				GEN_METHOD(GetKeyDownInt, 1, true)
				GEN_CLASS_END,

				GEN_CLASS(UnityEngine, GL)
				GEN_METHOD(Begin, 1, true),
				GEN_METHOD(Color, 1, true),
				GEN_METHOD(Vertex, 1, true),
				GEN_METHOD(End, 0, true)
				GEN_CLASS_END
			}
		},
		MRKGenDataAssembly {
			"UnityEngine.UIModule",
			{
				GEN_CLASS(UnityEngine, Canvas)
				GEN_METHOD(set_renderMode, 1, false),
				GEN_METHOD(set_sortingOrder, 1, false)
				GEN_CLASS_END
			}
		},
		MRKGenDataAssembly {
			"UnityEngine.UI",
			{
				GEN_CLASS(UnityEngine.UI, CanvasScaler)
				GEN_METHOD(set_uiScaleMode, 1, false),
				GEN_METHOD(set_referenceResolution, 1, false)
				GEN_CLASS_END,

				GEN_CLASS(UnityEngine.UI, Graphic)
				GEN_METHOD(set_color, 1, false)
				GEN_CLASS_END,

				GEN_CLASS(UnityEngine.UI, Text)
				GEN_METHOD(set_text, 1, false),
				GEN_METHOD(set_fontSize, 1, false),
				GEN_METHOD(set_fontStyle, 1, false),
				GEN_METHOD(set_font, 1, false),
				GEN_METHOD(set_supportRichText, 1, false)
				GEN_CLASS_END
			}
		},

		MRKGenDataAssembly {
			"UnityEngine.TextRenderingModule",
			{
				GEN_CLASS(UnityEngine, Font)
				GEN_METHOD(CreateDynamicFontFromOSFont, 2, true)
				GEN_CLASS_END
			}
		},

		MRKGenDataAssembly {
			"Assembly-CSharp",
			{
				GEN_CLASS(EFT, GameWorld)
				GEN_MORE
				GEN_FIELD(RegisteredPlayers),
				GEN_FIELD(LootItems)
				GEN_CLASS_END,

				GEN_CLASS(EFT, Player)
				GEN_METHOD(get_Id, 0, false),
				GEN_METHOD(get_PointOfView, 0, false),
				GEN_METHOD(get_Profile, 0, false)
				GEN_CLASS_END,

				GEN_CLASS(EFT, Profile)
				GEN_MORE
				GEN_FIELD(Info)
				GEN_CLASS_END
			}
		},

		MRKGenDataAssembly {
			"mscorlib",
			{
				GEN_CLASS(System.Collections.Generic, List`1)
				GEN_MORE
				GEN_FIELD(_items)
				GEN_CLASS_END,

				GEN_CLASS(System.Collections.Generic, Dictionary`2)
				GEN_METHOD(get_Item, 1, false)
				GEN_CLASS_END,

				GEN_CLASS(System, Type)
				GEN_METHOD_EX(GetMethod, 2, false, 2)
				GEN_CLASS_END,

				GEN_CLASS(System.Reflection, MethodBase)
				GEN_METHOD(get_MethodHandle, 0, false)
				GEN_CLASS_END,

				GEN_CLASS(System.Reflection, MonoMethod)
				GEN_METHOD(get_MethodHandle, 0, false)
				GEN_CLASS_END,

				GEN_CLASS(System, RuntimeMethodHandle)
				GEN_METHOD(GetFunctionPointer, 0, false)
				GEN_CLASS_END
			}
		},

		MRKGenDataAssembly{
			"UnityEngine.AssetBundleModule",
			{
				GEN_CLASS(UnityEngine, AssetBundle)
				GEN_METHOD(LoadFromFile, 1, true),
				GEN_METHOD(LoadAsset, 2, false)
				GEN_CLASS_END
			}
		},

		MRKGenDataAssembly{
			"UnityEngine.IMGUIModule",
			{
				GEN_CLASS(UnityEngine, GUI)
				GEN_METHOD_EX(Box, 2, true, 1),
				GEN_METHOD_EX(Label, 2, true, 1),
				GEN_METHOD(DrawTexture, 2, true),
				GEN_METHOD(get_skin, 0, true)
				GEN_CLASS_END,

				GEN_CLASS(UnityEngine, GUISkin)
				GEN_METHOD(get_label, 0, false)
				GEN_CLASS_END,

				GEN_CLASS(UnityEngine, GUIStyle)
				GEN_METHOD(CalcSize, 1, false),
				GEN_METHOD(set_alignment, 1, false),
				GEN_METHOD(get_alignment, 0, false)
				GEN_CLASS_END,

				GEN_CLASS(UnityEngine, GUIContent)
				GEN_METHOD(Temp, 1, true)
				GEN_CLASS_END,

				GEN_CLASS(UnityEngine, GUIUtility)
				GEN_METHOD(ScreenToGUIPoint, 1, true)
				GEN_CLASS_END,

				GEN_CLASS(UnityEngine, Event)
				GEN_METHOD(get_current, 0, true),
				GEN_METHOD(get_type, 0, false)
				GEN_CLASS_END
			}
		}
	};

	void Init() {
		char __path[MAX_PATH];
		mrks string spath(__path, GetModuleFileNameA(0, __path, MAX_PATH));

		spath = concat(spath.substr(0, spath.find_last_of('\\')), "\\MRK CGEN Log.txt");
		MRKSetLogPath(spath);

		MRKLog("MRK XCPP CODEGEN - v1\n", true, false);

#ifdef MRK_XCPP_MONO
		MRKLog("Initializing Mono backend");
#else
		MRKLog("Initializing XCPP backend");
#endif

		if (!MRKXCPPBackendInit(MONO_MODULE_NAME)) {
			MRKLog("Unable to initialize XCPP backend");
			return;
		}

		MRKLog("Successfully initialized XCPP backend");

		mrku32 ptrsSz;
		void*** ptrs = MRKXCPPBackendGetPointers(&ptrsSz);

		MRKLog("XCPP backend pointers:");

		for (mrku32 idx = 0; idx < ptrsSz; idx++) {
			MRKLog(concat('\t', *(ptrs[idx])));
		}

		spath = concat(spath.substr(0, spath.find_last_of('\\')), "\\MRK CGEN");
		if (!mrksfs is_directory(spath))
			mrksfs create_directory(spath);

		MRKCodeWriter codeWriter(spath);

		for (MRKGenDataAssembly& assembly : ms_GenAssemblies) {
			MRKLog("XXX");
			MRKXCPPImage* _image = MRKXCPPGetImage(assembly.Name.c_str());

			for (MRKGenDataClass& clazz : assembly.Classes) {
				MRKXCPPClass* _class = MRKXCPPGetClass(_image, clazz.Namespace.c_str(), clazz.Name.c_str());
				if (!_class) {
					MRKLog(concat("CLASS NULL -> ", clazz.Name));
					continue;
				}

				codeWriter.OpenClass(_class);

				for (MRKGenDataMethod& method : clazz.Methods) {
					MRKXCPPMethod* _method = MRKXCPPGetMethod(_class, method.Name.c_str(), method.ParamCount, method.Occurance);
					if (!_method) {
						MRKLog(concat("METHOD NULL -> ", method.Name));
						continue;
					}

					codeWriter.WriteMethod(_method, method.Static);
				}

				for (MRKGenDataField& field : clazz.Fields) {
					MRKLog("XXMM");
					MRKXCPPField* _field = MRKXCPPGetField(_class, field.Name.c_str());
					if (!_field) {
						MRKLog(concat("FIELD NULL -> ", field.Name));
						continue;
					}

					codeWriter.WriteField(_field);
				}

				codeWriter.CloseClass();
			}
		}

		codeWriter.CloseWriter();
	}
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
	if (fdwReason == DLL_PROCESS_ATTACH)
		MRK::Init();

	return TRUE;
}

int main() {
	MRK::Init();

	return 0;
}