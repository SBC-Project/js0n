#pragma once
/*The MIT License(MIT)
Original work Copyright(c) 2010 - 2016 Jeremie Miller
Modified work Copyright 2018 Gabriel Borrelli

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files(the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdlib.h>

namespace js0nPP
{
	class js0nPP
	{
	public:
		js0nPP();

		/** Parses JSON code
		* @param pKey string to match or null
		* @param pKeyLength key length (or 0), or if null key then len is the array offset value
		* @param pJSON json object or array
		* @param pJSONLenth length of json
		* @param pValueLength where to store return value length
		* @return returns pointer to value and sets len to value length, or 0 if not found any parse error will set vlen to the position of the error
		*/
		const char* js0n(const char* pKey, size_t pKeyLength, const char* pJSON, size_t pJSONLenth, size_t* pValueLength);

	private:
		// Only at depth 1, track start pointers to match key/value
#define PUSH(i) if(depth == 1) { if(!index) { val = cur+i; }else{ if(pKeyLength && index == 1) start = cur+i; else index--; } }
		// Determine if key matches or value is complete
#define CAP(i) if(depth == 1) { if(val && !index) {*pValueLength = (size_t)((cur+i+1) - val); return val;}; if(pKeyLength && start) {index = (pKeyLength == (size_t)(cur-start) && strncmp(pKey,start,pKeyLength)==0) ? 0 : 2; start = 0;} }
		// Jump to the assigned lable
#define JUMP(x) do{ label=x; goto jump_target; }while(0)

		void InitializeStructJumpTable();
		void InitializeBareJumpTable();
		void InitializeStringJumpTable();
		void InitializeUTF8ContinueJumpTable();
		void InitializeESCJumpTable();

		enum Labels { l_bad, l_up, l_down, l_qup, l_qdown, l_esc, l_unesc, l_bare, l_unbare, l_loop, l_utf8_2, l_utf8_3, l_utf8_4, l_utf_continue };

		Labels mStructJumpTable[256];
		Labels mBareJumpTable[256];
		Labels mStringJumpTable[256];
		Labels mUTF8ContinueJumpTable[256];
		Labels mESCJumpTable[256];
	};
}
