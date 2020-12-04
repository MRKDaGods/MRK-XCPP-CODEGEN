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

#include <vector>
#include <malloc.h>
#include <algorithm>

#include "MRKCommon.h"

namespace MRK {
	static mrks vector<void*> mrk_stored_ptrs;

	template<typename T>
	inline T* MRKAllocNew() {
		T* ptr = (T*)malloc(sizeof(T));
		mrk_stored_ptrs.push_back((void*)ptr);

		return ptr;
	}

	template<typename T>
	inline T* MRKAllocNew(void* oldPtr) {
		for (void* ptrs : mrk_stored_ptrs) {
			if (((mrku32ptr)ptrs) == ((mrku32ptr)oldPtr))
				return (T*)ptrs;
		}

		return MRKAllocNew<T>();
	}

	template<typename T>
	inline T* MRKAllocNewArr(mrku32 sz) {
		T* ptr = (T*)malloc(sizeof(T) * sz);
		mrk_stored_ptrs.push_back((void*)ptr);

		return ptr;
	}

	template<typename T>
	inline void MRKAllocFree(T* ptr) {
		if (!(MRK_VEC_CONTAIN(mrk_stored_ptrs, ptr)))
			return;

		mrk_stored_ptrs.erase(mrks remove(mrk_stored_ptrs.begin(), mrk_stored_ptrs.end(), 
			(void*)ptr), mrk_stored_ptrs.end());
		free(ptr);
	}

	inline void MRKAllocFreeAll() {
		for (mrku32 idx = mrk_stored_ptrs.size() - 1; idx > -1; idx--)
			MRKAllocFree<void>(mrk_stored_ptrs[idx]);
	}
}