//
// Created by root on 14.06.16.
//

#ifndef VOXWEBSERVER_LIN64_VOXSTRING_H
#define VOXWEBSERVER_LIN64_VOXSTRING_H

#include <algorithm>

class voxstring
{
public:
	static vector<char *> split(char *s, const char *delim, unsigned int lim = UINT32_MAX);

	static vector<string> split(string s, const char *delim, unsigned int lim = UINT32_MAX);

	static string decode_web(char *str);

	static string decode_web(string str);

	static string xss_filter(string str);

	static string xss_filter_char(char *str);

	static wstring to_wstring(std::string &s);

	static string random_hash(int length);

	static bool icompare(const string &str1, const string &str2);

	static string trim(string str);

	static string trim_start_end(string &str);

};

inline string voxstring::trim(string str)
{
	remove_if(str.begin(), str.end(), ::isspace);
	return str;
}

inline string voxstring::trim_start_end(string &str)
{
	size_t first = str.find_first_not_of(' ');
	size_t last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
}

inline bool voxstring::icompare(const string &str1, const string &str2)
{
	if (str1.size() != str2.size())
	{
		return false;
	}
	for (string::const_iterator c1 = str1.begin(), c2 = str2.begin(); c1 != str1.end(); ++c1, ++c2)
	{
		if (tolower(*c1) != tolower(*c2))
		{
			return false;
		}
	}
	return true;
}

inline string voxstring::random_hash(int length)
{
	char *res = new char[length + 1];

	for (int i = 0; i < length; i++)
	{
		res[i] = 65 + rand() % 26;
	}
	res[length] = 0;
	string ret = string(res);
	delete[] res;

	return ret;
}


inline wstring voxstring::to_wstring(std::string &s)
{
	std::wstring wsTmp(s.begin(), s.end());
	return wsTmp;
}

inline vector<char *> voxstring::split(char *s, const char *delim, unsigned int lim)
{
	if (!s || !delim)
		return vector<char *>();

	//int ln = strlen(s_r);
	//char* s = new char[ln+1];
	//s[ln] = 0x00;
	//memcpy(s, s_r, ln);
	int size_dest = min((unsigned int) strlen(s), lim);
	size_t size_val = strlen(delim);
	vector<char *> res;
	size_t ok = size_val;
	for (int i = 0; i <= size_dest; i++)
	{
		if (ok == size_val)
		{
			res.push_back(&s[i]);
			ok = 0;
			if ((i - size_val) >= 0)
			{
				for (size_t g = i - size_val; g < i; g++)
				{
					s[g] = 0x00;
				}
			}
		}

		if (s[i] == delim[ok])
		{
			ok++;
		} else
			ok = 0;
	}
	return res;
}

inline vector<string> voxstring::split(string s, const char *delim, unsigned int lim)
{
	if (s.empty() || !delim)
		return vector<string>();

	int size_dest = s.length();
	int size_val = strlen(delim);
	vector<char *> reser;
	int ok = size_val;
	char *sata = new char[size_dest + 1];
	memcpy(sata, s.c_str(), size_dest + 1);
	for (int i = 0; i <= size_dest; i++)
	{
		if (ok == size_val)
		{
			reser.push_back(&sata[i]);
			ok = 0;
			if ((i - size_val) >= 0)
			{
				for (int g = i - size_val; g < i; g++)
				{
					sata[g] = 0x00;
				}
			}
		}

		if (sata[i] == delim[ok])
		{
			ok++;
		} else
			ok = 0;
	}
	vector<string> res;
	for (char *eh : reser)
	{
		res.push_back(string(eh));
	}
	delete[] sata;
	return res;
}

inline string voxstring::decode_web(string str)
{
	if (str.empty())
		return string();

	decode_web(str.c_str());
}

inline string voxstring::decode_web(char *str)
{
	if (!str)
		return string();

	int sln = strlen(str);

	if (!sln)
		return string();

	int i = 0;
	char *res = new char[sln + 1]();
	int resi = 0;

	char cur;
	char tmp[3];
	int tbt = 0;
	while (str[i] != 0)
	{
		cur = str[i];
		if (cur != '%')
		{
			if (tbt == 0)
			{
				if (cur != '+')
					res[resi++] = cur;
				else
					res[resi++] = ' ';
			} else
			{
				if (isxdigit(cur))
				{
					if (tbt-- == 1)
					{
						tmp[0] = str[i - 1];
						tmp[1] = str[i];
						res[resi++] = strtol(tmp, 0, 16);
					}
				} else
				{
					tbt = 0;
				}
			}
		} else
		{
			tbt = 2;
		}
		i++;
	}
	string rt = string(res);
	delete[] res;
	return rt;
}

inline string voxstring::xss_filter(string str)
{
	//	if (voxerr::report(str.empty(), voxerr_type::too_short, __FUNCTION__))
	//		return string();
	char *cgf = new char[str.size() + 1];
	strcpy(cgf, str.c_str());
	string r = xss_filter_char(cgf);
	delete[] cgf;
	return r;
}

inline string voxstring::xss_filter_char(char *str)
{
	if (!str)
		return string();

	int ln = 1;
	int i = 0;
	while (str[i])
	{
		switch (str[i])
		{
			case '&':
				ln += 5;
				break;
			case '<':
				ln += 4;
				break;
			case '>':
				ln += 4;
				break;
			case '"':
				ln += 6;
				break;
			case '\'':
				ln += 6;
				break;
			case '/':
				ln += 6;
				break;
			default:
				ln++;
		}
		i++;
	}

	i = 0;
	char *arr = new char[ln]();
	ln = 0;
	while (str[i])
	{
		switch (str[i])
		{
			case '&':
				strcpy(arr + ln, "&amp;");
				ln += 5;
				break;
			case '<':
				strcpy(arr + ln, "&lt;");
				ln += 4;
				break;
			case '>':
				strcpy(arr + ln, "&gt;");
				ln += 4;
				break;
			case '"':
				strcpy(arr + ln, "&quot;");
				ln += 6;
				break;
			case '\'':
				strcpy(arr + ln, "&#x27;");
				ln += 6;
				break;
			case '/':
				strcpy(arr + ln, "&#x2F;");
				ln += 6;
				break;
			default:
				arr[ln] = str[i];
				ln++;
		}
		i++;
	}
	string rs = string(arr);
	delete[] arr;
	return rs;
}

#endif //VOXWEBSERVER_LIN64_VOXSTRING_H
