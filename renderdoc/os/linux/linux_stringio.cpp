/******************************************************************************
 * The MIT License (MIT)
 * 
 * Copyright (c) 2014 Crytek
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/


#include "os/os_specific.h"
#include "api/app/renderdoc_app.h"

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include <iconv.h>

#include "common/string_utils.h"
#include "common/threading.h"
using std::string;

namespace Keyboard
{
	void Init()
	{
	}
	
	Display *CurrentXDisplay = NULL;

	void AddInputWindow(void *wnd)
	{
		// TODO check against this drawable & parent window being focused in GetKeyState
	}

	void RemoveInputWindow(void *wnd)
	{
	}

	bool GetKeyState(int key)
	{
		KeySym ks = 0;
		
		if(CurrentXDisplay == NULL) return false;

		if(key >= eKey_A && key <= eKey_Z) ks = key;
		if(key >= eKey_0 && key <= eKey_9) ks = key;
		
		switch(key)
		{
			case eKey_Divide:    ks = XK_KP_Divide; break;
			case eKey_Multiply:  ks = XK_KP_Multiply; break;
			case eKey_Subtract:  ks = XK_KP_Subtract; break;
			case eKey_Plus:      ks = XK_KP_Add; break;
			case eKey_F1:        ks = XK_F1; break;
			case eKey_F2:        ks = XK_F2; break;
			case eKey_F3:        ks = XK_F3; break;
			case eKey_F4:        ks = XK_F4; break;
			case eKey_F5:        ks = XK_F5; break;
			case eKey_F6:        ks = XK_F6; break;
			case eKey_F7:        ks = XK_F7; break;
			case eKey_F8:        ks = XK_F8; break;
			case eKey_F9:        ks = XK_F9; break;
			case eKey_F10:       ks = XK_F10; break;
			case eKey_F11:       ks = XK_F11; break;
			case eKey_F12:       ks = XK_F12; break;
			case eKey_Home:      ks = XK_Home; break;
			case eKey_End:       ks = XK_End; break;
			case eKey_Insert:    ks = XK_Insert; break;
			case eKey_Delete:    ks = XK_Delete; break;
			case eKey_PageUp:    ks = XK_Prior; break;
			case eKey_PageDn:    ks = XK_Next; break;
			case eKey_Backspace: ks = XK_BackSpace; break;
			case eKey_Tab:       ks = XK_Tab; break;
			case eKey_PrtScrn:   ks = XK_Print; break;
			case eKey_Pause:     ks = XK_Pause; break;
			default:
				break;
		}

		if(ks == 0)
			return false;
		
		KeyCode kc = XKeysymToKeycode(CurrentXDisplay, ks);
		
		char keyState[32];
		XQueryKeymap(CurrentXDisplay, keyState);
		
		int byteIdx = (kc/8);
		int bitMask = 1 << (kc%8);
		
		uint8_t keyByte = (uint8_t)keyState[byteIdx];
		
		return (keyByte & bitMask) != 0;
	}
}

namespace FileIO
{
	string GetAppFolderFilename(string filename)
	{
		passwd *pw = getpwuid(getuid());
		const char *homedir = pw->pw_dir;

		string ret = string(homedir) + "/.renderdoc/";

		mkdir(ret.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

		return ret + filename;
	}

	void GetExecutableFilename(string &selfName)
	{
		char path[512] = {0};
		readlink("/proc/self/exe", path, 511);

		selfName = string(path);
	}

	void GetDefaultFiles(const char *logBaseName, string &capture_filename, string &logging_filename, string &target)
	{
		char path[2048] = {0};
		readlink("/proc/self/exe", path, 511);
		const char *mod = strrchr(path, '/');
		if(mod == NULL)
			mod = "unknown";
		else
			mod++;

		target = string(mod);

		time_t t = time(NULL);
		tm now = *localtime(&t);

		char temp_folder[2048] = { "/tmp" };

		char *temp_override = getenv("RENDERDOC_TEMP");
		if(temp_override && temp_override[0] == '/')
		{
			strncpy(temp_folder, temp_override, sizeof(temp_folder)-1);
			size_t len = strlen(temp_folder);
			while(temp_folder[len-1] == '/') temp_folder[--len] = 0;
		}

		char temp_filename[2048] = {0};

		snprintf(temp_filename, sizeof(temp_filename)-1, "/tmp/%s_%04d.%02d.%02d_%02d.%02d.rdc", mod, 1900+now.tm_year, now.tm_mon+1, now.tm_mday, now.tm_hour, now.tm_min);

		capture_filename = string(temp_filename);

		snprintf(temp_filename, sizeof(temp_filename)-1, "/tmp/%s_%04d.%02d.%02d_%02d.%02d.%02d.log", logBaseName, 1900+now.tm_year, now.tm_mon+1, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec);

		logging_filename = string(temp_filename);
	}
	
	uint64_t GetModifiedTimestamp(const char *filename)
	{
		struct ::stat st;
		int res = stat(filename, &st);

		if(res == 0)
		{
			return (uint64_t)st.st_mtim.tv_sec;
		}
		
		return 0;
	}

	void Copy(const char *from, const char *to, bool allowOverwrite)
	{
		if(from[0] == 0 || to[0] == 0)
			return;

		FILE *ff = ::fopen(from, "r");

		if(!ff)
		{
			RDCERR("Can't open source file for copy '%s'", from);
			return;
		}

		FILE *tf = ::fopen(to, "r");

		if(tf && !allowOverwrite)
		{
			RDCERR("Destination file for non-overwriting copy '%s' already exists", from);
			::fclose(ff);
			::fclose(tf);
			return;
		}

		if(tf)
			::fclose(tf);

		tf = ::fopen(to, "w");

		if(!tf)
		{
			::fclose(ff);
			RDCERR("Can't open destination file for copy '%s'", to);
		}

		char buffer[BUFSIZ];

		while(!feof(ff))
		{
			size_t nread = ::fread(buffer, 1, BUFSIZ, ff);
			::fwrite(buffer, 1, nread, tf);
		}

		::fclose(ff);
		::fclose(tf);
	}

	void Delete(const char *path)
	{
		unlink(path);
	}

	FILE *fopen(const char *filename, const char *mode)
	{
		return ::fopen(filename, mode);
	}

	size_t fread(void *buf, size_t elementSize, size_t count, FILE *f) { return ::fread(buf, elementSize, count, f); }
	size_t fwrite(const void *buf, size_t elementSize, size_t count, FILE *f) { return ::fwrite(buf, elementSize, count, f); }

	uint64_t ftell64(FILE *f) { return (uint64_t)::ftell(f); }
	void fseek64(FILE *f, uint64_t offset, int origin) { ::fseek(f, (long)offset, origin); }

	int fclose(FILE *f) { return ::fclose(f); }
};

namespace StringFormat
{
	void sntimef(char *str, size_t bufSize, const char *format)
	{
		time_t tim;
		time(&tim);

		tm *tmv = localtime(&tim);

		strftime(str, bufSize, format, tmv);
	}

	string Fmt(const char *format, ...)
	{
		va_list args;
		va_start(args, format);

		va_list args2;
		va_copy(args2, args);

		int size = StringFormat::vsnprintf(NULL, 0, format, args2);

		char *buf = new char[size+1];
		StringFormat::vsnprintf(buf, size+1, format, args);
		buf[size] = 0;

		va_end(args);
		va_end(args2);

		string ret = buf;

		delete[] buf;
		
		return ret;
	}

	// cache iconv_t descriptor to save on iconv_open/iconv_close each time
	iconv_t iconvWide2UTF8 = (iconv_t)-1;

	// iconv is not thread safe when sharing an iconv_t descriptor
	// I don't expect much contention but if it happens we could TryLock
	// before creating a temporary iconv_t, or hold two iconv_ts, or something.
	Threading::CriticalSection lockWide2UTF8;

	string Wide2UTF8(const std::wstring &s)
	{
		// include room for null terminator, assuming unicode input (not ucs)
		// utf-8 characters can be max 4 bytes.
		size_t len = (s.length()+1)*4;

		vector<char> charBuffer;

		if(charBuffer.size() < len)
			charBuffer.resize(len);

		size_t ret;

		{
			SCOPED_LOCK(lockWide2UTF8);

			if(iconvWide2UTF8 == (iconv_t)-1)
				iconvWide2UTF8 = iconv_open("UTF-8", "WCHAR_T");

			if(iconvWide2UTF8 == (iconv_t)-1)
			{
				RDCERR("Couldn't open iconv for WCHAR_T to UTF-8: %d", errno);
				return "";
			}

			char *inbuf = (char *)s.c_str();
			size_t insize = (s.length()+1)*sizeof(wchar_t); // include null terminator
			char *outbuf = &charBuffer[0];
			size_t outsize = len;

			ret = iconv(iconvWide2UTF8, &inbuf, &insize, &outbuf, &outsize);
		}

		if(ret == (size_t)-1)
		{
#if !defined(_RELEASE)
			RDCWARN("Failed to convert wstring");
#endif
			return "";
		}

		// convert to string from null-terminated string - utf-8 never contains
		// 0 bytes before the null terminator, and this way we don't care if
		// charBuffer is larger than the string
		return string(&charBuffer[0]);
	}
};

namespace OSUtility
{
	void WriteOutput(int channel, const char *str)
	{
		if(channel == OSUtility::Output_StdOut)
			fprintf(stdout, "%s", str);
		else if(channel == OSUtility::Output_StdErr)
			fprintf(stderr, "%s", str);
	}
};

