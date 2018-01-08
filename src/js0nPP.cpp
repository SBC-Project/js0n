#include "js0nPP.h"

#include <string.h> // one strncmp() is used to do key comparison, and a strlen(key) if no len passed in

namespace js0nPP
{
	js0nPP::js0nPP()
	{
		InitializeStructJumpTable();
		InitializeBareJumpTable();
		InitializeStringJumpTable();
		InitializeUTF8ContinueJumpTable();
		InitializeESCJumpTable();
	}

	const char* js0nPP::js0n(const char * pKey, size_t pKeyLength, const char * pJSON, size_t pJSONLenth, size_t * pValueLength)
	{
		Labels label;
		const char *val = 0;
		const char *cur, *end, *start;
		size_t index = 1;
		int depth = 0;
		int utf8_remain = 0;
		Labels *jmpTable = mStructJumpTable;

		if (!pJSON || pJSONLenth <= 0 || !pValueLength) return 0;
		*pValueLength = 0;

		// no key is array mode, klen provides requested index
		if (!pKey)
		{
			index = pKeyLength;
			pKeyLength = 0;
		}
		else {
			if (pKeyLength <= 0) pKeyLength = strlen(pKey); // convenience
		}

		for (start = cur = pJSON, end = cur + pJSONLenth; cur<end; cur++)
		{
			JUMP(jmpTable[(unsigned char)*cur]);
		l_loop:;
		}

		if (depth) *pValueLength = pJSONLenth; // incomplete
		return 0;

	l_bad:
		*pValueLength = cur - pJSON; // where error'd
		return 0;

	l_up:
		PUSH(0);
		++depth;
		goto l_loop;

	l_down:
		--depth;
		CAP(0);
		goto l_loop;

	l_qup:
		PUSH(1);
		jmpTable = mStringJumpTable;
		goto l_loop;

	l_qdown:
		CAP(-1);
		jmpTable = mStructJumpTable;
		goto l_loop;

	l_esc:
		jmpTable = mESCJumpTable;
		goto l_loop;

	l_unesc:
		jmpTable = mStringJumpTable;
		goto l_loop;

	l_bare:
		PUSH(0);
		jmpTable = mBareJumpTable;
		goto l_loop;

	l_unbare:
		CAP(-1);
		jmpTable = mStructJumpTable;
		JUMP(jmpTable[(unsigned char)*cur]);

	l_utf8_2:
		jmpTable = mUTF8ContinueJumpTable;
		utf8_remain = 1;
		goto l_loop;

	l_utf8_3:
		jmpTable = mUTF8ContinueJumpTable;
		utf8_remain = 2;
		goto l_loop;

	l_utf8_4:
		jmpTable = mUTF8ContinueJumpTable;
		utf8_remain = 3;
		goto l_loop;

	l_utf_continue:
		if (!--utf8_remain)
			jmpTable = mStringJumpTable;
		goto l_loop;

	jump_target:
		switch (label)
		{
		case l_bad:
			goto l_bad;
			break;
		case l_up:
			goto l_up;
			break;
		case l_down:
			goto l_down;
			break;
		case l_qup:
			goto l_qup;
			break;
		case l_qdown:
			goto l_qdown;
			break;
		case l_esc:
			goto l_esc;
			break;
		case l_unesc:
			goto l_unesc;
			break;
		case l_bare:
			goto l_bare;
			break;
		case l_unbare:
			goto l_unbare;
			break;
		case l_loop:
			goto l_loop;
			break;
		case l_utf8_2:
			goto l_utf8_2;
			break;
		case l_utf8_3:
			goto l_utf8_3;
			break;
		case l_utf8_4:
			goto l_utf8_4;
			break;
		case l_utf_continue:
			goto l_utf_continue;
			break;
		}
	}

	void js0nPP::InitializeStructJumpTable()
	{
		for (int i = 0; i < 256; i++)
		{
			mStructJumpTable[i] = Labels::l_bad;
		}
		for (int i = 48; i <= 57; i++)
		{
			mStructJumpTable[i] = Labels::l_bare;
		}
		for (int i = 65; i <= 90; i++)
		{
			mStructJumpTable[i] = Labels::l_bare;
		}
		for(int i = 97; i <= 122; i++)
		{
			mStructJumpTable[i] = Labels::l_bare;
		}

		mStructJumpTable['\t'] =
			mStructJumpTable[' '] =
			mStructJumpTable['\r'] =
			mStructJumpTable['\n'] =
			mStructJumpTable[':'] =
			mStructJumpTable[','] = Labels::l_loop;

		mStructJumpTable['"'] = Labels::l_qup;

		mStructJumpTable['['] =
			mStructJumpTable['{'] = Labels::l_up;

		mStructJumpTable[']'] =
			mStructJumpTable['}'] = Labels::l_down;

		mStructJumpTable['-'] = Labels::l_bare;
	}

	void js0nPP::InitializeBareJumpTable()
	{
		for (int i = 0; i < 256; i++)
		{
			mBareJumpTable[i] = Labels::l_bad;
		}
		for (int i = 32; i <= 126; i++)
		{
			mBareJumpTable[i] = Labels::l_loop;
		}

		mBareJumpTable['\t'] =
			mBareJumpTable[' '] =
			mBareJumpTable['\r'] =
			mBareJumpTable['\n'] =
			mBareJumpTable[','] =
			mBareJumpTable[']'] =
			mBareJumpTable['}'] =
			mBareJumpTable[':'] = l_unbare;
	}

	void js0nPP::InitializeStringJumpTable()
	{
		for (int i = 0; i < 256; i++)
		{
			mStringJumpTable[i] = Labels::l_bad;
		}
		for (int i = 32; i <= 126; i++)
		{
			mStringJumpTable[i] = Labels::l_loop;
		}
		for (int i = 192; i <= 223; i++)
		{
			mStringJumpTable[i] = Labels::l_utf8_2;
		}
		for (int i = 224; i <= 239; i++)
		{
			mStringJumpTable[i] = Labels::l_utf8_3;
		}
		for (int i = 240; i <= 247; i++)
		{
			mStringJumpTable[i] = Labels::l_utf8_4;
		}

		mStringJumpTable['\\'] = Labels::l_esc;
		mStringJumpTable['"'] = Labels::l_qdown;
	}

	void js0nPP::InitializeUTF8ContinueJumpTable()
	{
		for(int i=0;i<256;i++)
		{
			mUTF8ContinueJumpTable[i] = Labels::l_bad;

			if (i >= 128 && i <= 191)
			{
				mUTF8ContinueJumpTable[i] = Labels::l_utf_continue;
			}
		}
	}

	void js0nPP::InitializeESCJumpTable()
	{
		for (int i = 0; i < 256; i++)
		{
			switch (i)
			{
			case '"':
			case '\\':
			case '/':
			case 'b':
			case 'f':
			case 'n':
			case 'r':
			case 't':
			case 'u':
				mESCJumpTable[i] = Labels::l_unesc;
				break;
			default:
				mESCJumpTable[i] = Labels::l_bad;
				break;
			}
		}
	}
}