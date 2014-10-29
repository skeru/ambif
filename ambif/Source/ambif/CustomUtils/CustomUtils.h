// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#pragma once

#include "ambif.h"
#include "unordered_map"
#include "map"
#include "stdexcept"
#include "functional"

/*namespace _foo_utils{
	struct foo1 {
		size_t operator()(const FString& p) const {
			try
			{
				return (p.GetCharArray().GetTypedData()) ? std::hash<std::wstring>()(std::wstring((wchar_t*)(p.GetCharArray().GetTypedData()))) : 0;
			}
			catch (std::exception e)
			{
				return 0;
			}
			//return (p.Len() > 0) ? (size_t)(p[0] + p[p.Len()-1]) : 0;
		}
	};

	struct foo2{
		bool operator()(const FString& p1, const FString& p2) const {
			return p1.Equals(p2);
		}
	};
}*/

/**
 * Implicit operator definitions. Implicit call in hashmap templates
 */
namespace std
{
	template<> struct less < FString >
	{
		const inline bool operator()(const FString& lhs, const FString& rhs)
		{
			return lhs.Compare(rhs) == -1;
		};
	};

		template<> struct hash < FString >
		{
			const size_t operator()(const FString& p)
			{
				try
				{
					return (p.GetCharArray().GetTypedData()) ? std::hash<std::wstring>()(std::wstring((wchar_t*)(p.GetCharArray().GetTypedData()))) : 0;
				}
				catch (std::exception e)
				{
					return 0;
				}
			};
		};

			template<> struct equal_to < FString >
			{
				const inline bool operator()(const FString& p1, const FString& p2)
				{
					return p1.Equals(p2);
				};
			};

}

/**
 *
 */
namespace Utils
{
	template<class datatype> using FHashMap =
		std::unordered_map <
		FString,
		datatype>/*,
				 _foo_utils::foo1,
				 _foo_utils::foo2 >*/;

	template<typename type1, typename type2> using hashmap = std::unordered_map < type1, type2 >;

	template <class datatype> using FMap = std::map < FString, datatype >;

	template<typename type1, typename type2> using map = std::map < type1, type2 >;

	/** Remove quotes and white spaces
	*/
	inline static FString CustomUnquote(const FString input)
	{
		FString foo;
		foo = input;
		foo = foo.Trim().TrimTrailing();
		if (foo.Len() > 2)
		{
			if ((foo[0] == foo[foo.Len() - 1] || foo[0] == foo[foo.Len() - 1]) && (foo[0] == '\'' || foo[0] == '\"'))
			{
				foo = foo.Mid(1, foo.Len() - 2);
			}
		}
		return foo;
	}

	/** safe conversion from FString to float
	* @return false if conversion fails, otherwise true. */
	static bool FStrToF(FString string, float&output)
	{
		if (string.IsNumeric())
		{
			try
			{
				output = FCString::Atof(*string);
				return true;
			}
			catch (std::exception e){}
		}
		//DebugUtils::LogString("ConversionError: Cannot convert String " + string + " to Float");
		return false;
	}

	/** safe conversion from FString to unsigned
	* @return false if conversion fails, otherwise true. */
	static bool FStrToU(FString string, unsigned&output)
	{
		if (string.IsNumeric())
		{
			try
			{
				output = FCString::Atoi(*string);
				return true;
			}
			catch (std::exception e){}
		}
		//DebugUtils::LogString("ConversionError: Cannot convert String " + string + " to Unsigned");
		return false;
	}
};