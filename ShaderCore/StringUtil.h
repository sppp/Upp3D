#ifndef __SHADERCORE_STRINGUTIL_H
#define __SHADERCORE_STRINGUTIL_H

NAMESPACE_GLSL_BEGIN

template <typename T>
inline String Flatten(const T& t)
{
	return t.Definition();
}

template <>
inline String Flatten<bool>(const bool& t)
{
	StringStream stream;
	stream << t;
	return stream.GetResult();
}

template <>
inline String Flatten<int>(const int& t)
{
	StringStream stream;
	stream << t;
	return stream.GetResult();
}

template <>
inline String Flatten<unsigned int>(const unsigned int& t)
{
	StringStream stream;
	stream << t;
	return stream.GetResult();
}

template <>
inline String Flatten<unsigned long>(const unsigned long& t)
{
	StringStream stream;
	stream << t;
	return stream.GetResult();
}

template <>
inline String Flatten<float>(const float& t)
{
	StringStream stream;
	stream << Sppp::DblStr(t);
	return stream.GetResult();
}

template <>
inline String Flatten<double>(const double& t)
{
	StringStream stream;
	stream << Sppp::DblStr(t);
	return stream.GetResult();
}

void xs(String& result, const char* s);

template<typename T, typename... Args>
inline void xs(String& result, const char* s, T value, Args... args)
{
	while (*s)
	{
		if (*s == '%')
		{
			if (*(s + 1) == '%')
			{
				++s;
			}
			else
			{
				StringStream stream;
				stream << Flatten(value);
				result += stream.GetResult();
				xs(result, s + 1, args...); // call even when *s == 0 to detect extra arguments
				return;
			}
		}
		result += *s++;
	}
	throw std::logic_error("extra arguments provided to printf");
}

template<typename... Args>
inline void xs(String& result, const char* s, String value, Args... args)
{
	while (*s)
	{
		if (*s == '%')
		{
			if (*(s + 1) == '%')
			{
				++s;
			}
			else
			{
				StringStream stream;
				stream << value;
				result += stream.GetResult();
				xs(result, s + 1, args...); // call even when *s == 0 to detect extra arguments
				return;
			}
		}
		result += *s++;
	}
	throw std::logic_error("extra arguments provided to printf");
}

template<typename... Args>
inline void xs(String& result, const char* s, const char* value, Args... args)
{
	while (*s)
	{
		if (*s == '%')
		{
			if (*(s + 1) == '%')
			{
				++s;
			}
			else
			{
				StringStream stream;
				stream << value;
				result += stream.GetResult();
				xs(result, s + 1, args...); // call even when *s == 0 to detect extra arguments
				return;
			}
		}
		result += *s++;
	}
	throw std::logic_error("extra arguments provided to printf");
}

template<typename... Args>
inline String xs(const char* s, Args... args)
{
	String buf;
	xs(buf, s, args...);
	return buf;
}

inline void xs(String& result, const char* s)
{
	while (*s)
	{
		if (*s == '%')
		{
			if (*(s + 1) == '%')
			{
				++s;
			}
			else
			{
				throw std::runtime_error("invalid format string: missing arguments");
			}
		}

		result += *s++;
	}
}

NAMESPACE_GLSL_END

#endif
