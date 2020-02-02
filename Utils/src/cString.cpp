/*
** Clipped -- a Multipurpose C++ Library.
**
** Copyright (C) 2019-2020 Christian Löpke. All rights reserved.
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
** [ MIT license: http://www.opensource.org/licenses/mit-license.php ]
*/

#include "cString.h"
#include <algorithm>
#include <cctype>   //std::isdigit
#include <codecvt>  //Converting ascii, utf16, utf32
#include <locale>   //Converting ...
#include "cLogger.h"

//Note ::std used because of g++ c++11 compiler bug. Template implementation has to be wrapped into
// a namespace Clipped { block to get around this bug. Therefore std:: would lead to Clipped::std::,
// which is wrong. Therefore all std namespace functions are prefixed with :: here.

namespace Clipped
{
    template <class T>
    BasicString<T>::BasicString() : ::std::basic_string<T>()
    {
    }

    template <class T>
    BasicString<T>::BasicString(const T* s)
    {
        if (s) *this = ::std::basic_string<T>(s);
    }

    template <class T>
    BasicString<T>::BasicString(const T* s, size_t n) : ::std::basic_string<T>(s, n)
    {
    }

    template <class T>
    BasicString<T>::BasicString(const ::std::vector<T>& s) : ::std::basic_string<T>(s.data(), s.size())
    {
    }

    template <class T>
    BasicString<T>::BasicString(const BasicString<T>& s) : ::std::basic_string<T>(s)
    {
    }

    template <class T>
    BasicString<T>::BasicString(const ::std::basic_string<T>& s) : ::std::basic_string<T>(s)
    {
    }

    template <class T>
    BasicString<T>::BasicString(const float& value)
    {
        *this = fromAsci(::std::to_string(value).c_str());
    }

    template <class T>
    BasicString<T>::BasicString(const float& value, size_t precision)
    {
        BasicStringStream<T> stream;
        stream << ::std::fixed << ::std::setprecision(precision) << value;
        *this = stream.str();
    }

    template <class T>
    BasicString<T>::BasicString(const double& value, size_t precision)
    {
        BasicStringStream<T> stream;
        stream << ::std::fixed << ::std::setprecision(precision) << value;
        *this = stream.str();
    }

    template <class T>
    BasicString<T>::BasicString(const int& value)
    {
        *this = fromAsci(::std::to_string(value).c_str());
    }

    template <class T>
    BasicString<T>::BasicString(const unsigned long long& value)
    {
        *this = fromAsci(::std::to_string(value).c_str());
    }

    template <class T>
    BasicString<T> BasicString<T>::toLower() const
    {
        BasicString<T> tmp = *this;
        transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
        return tmp;
    }

    template <class T>
    BasicString<T> BasicString<T>::toUpper() const
    {
        BasicString<T> tmp = *this;
        transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
        return tmp;
    }

    template <>
    BasicString<char> BasicString<char>::toString() const
    {
        return *this;  // Copy
    }

    template <>
    BasicString<char> BasicString<wchar_t>::toString() const
    {
        setlocale(LC_CTYPE, "");
        return ::std::string(this->begin(), this->end()).c_str();
    }

    template <>
    BasicString<char> BasicString<char16_t>::toString() const
    {
        // Workaround for VS - unresolved external symbols of codecvt.
        // They forgot std::locale::id for char16_t and char32_t, but not for int32_t
#if (_MSC_VER >= 1900 /* VS 2015*/) && (_MSC_VER <= 1916 /* VS 2017 */)
        ::std::wstring_convert<::std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
        auto p = reinterpret_cast<const int16_t*>(this->data());
        return BasicString<char>(convert.to_bytes(p, p + this->size()));
#else
        ::std::wstring_convert<::std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
        return BasicString<char>(convert.to_bytes(this->c_str()));
#endif
    }

    template <>
    BasicString<char> BasicString<char32_t>::toString() const
    {
        // Workaround for VS - unresolved external symbols of codecvt.
        // They forgot std::locale::id for char16_t and char32_t, but not for int32_t
#if (_MSC_VER >= 1900 /* VS 2015*/) && (_MSC_VER <= 1916 /* VS 2017 */)
        ::std::wstring_convert<::std::codecvt_utf8_utf16<int32_t>, int32_t> convert;
        auto p = reinterpret_cast<const int32_t*>(this->data());
        return BasicString<char>(convert.to_bytes(p, p + this->size()));
#else
        ::std::wstring_convert<::std::codecvt_utf8<char32_t>, char32_t> convert;
        return BasicString<char>(convert.to_bytes(this->c_str()));
#endif
    }

    template <>
    BasicString<wchar_t> BasicString<char>::toWString() const
    {
        ::std::wstring_convert<::std::codecvt_utf8_utf16<wchar_t>> converter;
        return ::std::wstring(converter.from_bytes(this->c_str()));
    }

    template <>
    BasicString<wchar_t> BasicString<wchar_t>::toWString() const
    {
        return *this;
    }

    template <>
    BasicString<wchar_t> BasicString<char16_t>::toWString() const
    {
        ::std::wstring_convert<::std::codecvt_utf16<wchar_t, 0x10ffff, ::std::little_endian>, wchar_t> conv;
        ::std::wstring ws = conv.from_bytes(reinterpret_cast<const char*>(&this->at(0)),
                                     reinterpret_cast<const char*>(&this->at(0) + this->size()));
        return ws;
    }

    template <>
    BasicString<wchar_t> BasicString<char32_t>::toWString() const
    {
        return this->toString().toWString();  // FIXME: Find a direct way of converting this.
    }

    template <class T>
    ::std::vector<char> BasicString<T>::toVector() const
    {
        ::std::vector<char> out;
        for(const auto& character : *this)
        {
            for(size_t i = sizeof(T); i > 0; i--)
            {
                out.push_back(static_cast<char>((character >> (i - 1))));
            }
        }
        return out;
    }

    template <class T>
    BasicString<T> BasicString<T>::toHexString(bool uppercase, const BasicString<T> delimiter) const
    {
        BasicStringStream<T> stream;
        if(uppercase)
            stream << std::uppercase;

        BasicString<T> fillchar = BasicString<T>::fromAsci("0");
        for(const T& c : *this)
        {
            stream << ::std::hex << ::std::setfill(fillchar.at(0)) << ::std::setw(2) << (unsigned int)c << delimiter;
        }
        BasicString<T> returned = stream.str();
        if(!returned.empty())
            returned = returned.substr(0, returned.length() - delimiter.length());
        return returned;
    }

    template <>
    BasicString<char> BasicString<char>::fromAsci(const char* str)
    {
        return String(str);
    }

    template <>
    BasicString<wchar_t> BasicString<wchar_t>::fromAsci(const char* str)
    {
        BasicString<char> conv = str;
        return conv.toWString();
    }

    template <class T>
    int BasicString<T>::toInt(size_t* pos, int base) const
    {
        return ::std::stoi(*this, pos, base);
    }

    template <class T>
    double BasicString<T>::toDouble() const
    {
        double casted;
        BasicStringStream<T> stream;
        stream << *this;
        stream >> casted;
        return casted;
    }

    template <class T>
    long BasicString<T>::toLong(size_t* pos, int base) const
    {
        return ::std::stol(*this, pos, base);
    }

    template <class T>
    bool BasicString<T>::isNumber() const
    {
        return !this->empty() && ::std::find_if(this->begin(), this->end(), [](T c) {
            return !::std::isdigit(static_cast<int>(c));
        }) == this->end();
    }

    template <class T>
    bool BasicString<T>::equals(const BasicString<T>& rhs) const
    {
        if (this->length() == rhs.length())
        {
            return this->find(rhs) == 0;
        }
        return false;
    }

    template <class T>
    bool BasicString<T>::operator==(const BasicString<T>& rhs) const
    {
        return this->equals(rhs);
    }

    template <class T>
    bool BasicString<T>::operator==(const T* rhs) const
    {
        return this->equals(BasicString<T>(rhs));
    }

    template <class T>
    bool BasicString<T>::contains(const BasicString<T>& search, bool ignoreCase) const
    {
        if (ignoreCase)
        {
            BasicString<T> str = this->toUpper();
            BasicString<T> searchStr = search.toUpper();

            return (str.find(searchStr) != BasicString::npos);
        }
        return (this->find(search) != BasicString::npos);
    }

    template <class T>
    bool BasicString<T>::endsWith(const BasicString<T>& value, bool ignoreCase) const
    {
        if(this->length() >= value.length())
        {
            BasicString<T> check = this->substr(this->length() - value.length());

            if(ignoreCase)
                return check.toUpper() == value.toUpper();
            else
                return check == value;
        }
        return false;
    }

    template <class T>
    BasicString<T>& BasicString<T>::replace(const BasicString<T>& search, const BasicString<T>& replace)
    {
        size_t pos = 0;
        while ((pos = this->find(search, pos)) != BasicString<T>::npos)  // While search is found..
        {
            this->::std::basic_string<T>::replace(pos, search.length(), replace);  // Replace it
            pos += replace.length();
        }
        return *this;
    }

    template <class T>
    ::std::vector<BasicString<T>>& BasicString<T>::split(const T delim,
                                                       ::std::vector<BasicString<T>>& elems) const
    {
        BasicStringStream<T> ss(*this);
        BasicString<T> item;
        while (::std::getline(ss, item, delim)) elems.push_back(item);

        return elems;
    }

    template <class T>
    ::std::vector<BasicString<T>> BasicString<T>::split(const T delim) const
    {
        ::std::vector<BasicString> vec;
        return split(delim, vec);
    }

    template <class T>
    ::std::vector<BasicString<T>> BasicString<T>::split(const BasicString<T>& delim) const
    {
        ::std::vector<BasicString<T>> parts;
        BasicString<T> tmp = *this;
        size_t index = tmp.find(delim);

        while (index != BasicString<T>::npos)
        {
            if (index != 0) parts.push_back(tmp.substr(0, index));  // Take part up to delim
            tmp = tmp.substr(index + delim.length());               // Cut off part + delim
            index = tmp.find(delim);
        }

        if (!tmp.empty()) parts.push_back(tmp);

        return parts;
    }

    template <class T>
    BasicString<T> BasicString<T>::trim(const BasicString<T>& trimChar) const
    {
        size_t endpos = this->find_last_not_of(trimChar);
        size_t firstpos = this->find_first_not_of(trimChar);
        if (BasicString<T>::npos != endpos)
        {
            return this->substr(firstpos, endpos + 1 - firstpos);
        }
        else if(endpos == BasicString<T>::npos && firstpos == BasicString<T>::npos)
        {
            return BasicString<T>(); //Return empty string.
        }
        return *this;
    }

    template <class T>
    BasicString<T> BasicString<T>::trimRight() const
    {
        BasicString<T> copy = *this;
        copy.erase(
                    ::std::find_if(copy.rbegin(), copy.rend(), [](int ch) { return !::std::isspace(ch); }).base(),
                    copy.end());
        return copy;
    }

    template <class T>
    BasicString<T> BasicString<T>::trimLeft() const
    {
        BasicString<T> copy = *this;
        copy.erase(copy.begin(),
                   ::std::find_if(copy.begin(), copy.end(), [](int ch) { return !::std::isspace(ch); }));
        return copy;
    }

    template <class T>
    BasicString<T> BasicString<T>::trim() const
    {
        BasicString<T> copy = this->trimRight();
        return copy.trimLeft();
    }

    template <class T>
    BasicString<T> BasicString<T>::fill(const BasicString<T>& fillChar, size_t length) const
    {
        BasicString<T> filled = *this;

        while (filled.length() < length && filled.length() + fillChar.length() <= length)
        {
            filled = filled.append(fillChar);
        }
        return filled;
    }

    template <class T>
    BasicStringStream<T>::BasicStringStream() : ::std::basic_stringstream<T>()
    {
    }

    template <class T>
    BasicStringStream<T>::BasicStringStream(const BasicString<T>& str) : ::std::basic_stringstream<T>(str)
    {
    }

    // Please compile template class for the following types:
    template class BasicString<char>;
    template class BasicStringStream<char>;

    template class BasicString<wchar_t>;
    template class BasicStringStream<wchar_t>;

#ifdef CLIPPED_BUILD_U16
    template class BasicString<char16_t>;
    template class BasicStringStream<char16_t>;
#endif
#ifdef CLIPPED_BUILD_U32
    template class BasicString<char32_t>;
    template class BasicStringStream<char32_t>;
#endif
}  // namespace Clipped

// Functions injected to std:
namespace std
{
    /**
     * @brief stoi string to int conversion function for BasicString Utf16.
     */
    int stoi(const Clipped::BasicString<char16_t>& str, size_t*& pos, int& base)
    {
        return ::std::stoi(str.toString().c_str(), pos, base);
    }

    /**
     * @brief stoi string to int conversion function for BasicString Utf32.
     */
    int stoi(const Clipped::BasicString<char32_t>& str, size_t*& pos, int& base)
    {
        return ::std::stoi(str.toString().c_str(), pos, base);
    }

    /**
     * @brief stol string to long conversion function for BasicString Utf16.
     */
    long stol(const Clipped::BasicString<char16_t>& str, size_t*& pos, int& base)
    {
        // Workaround for VS - unresolved external symbols of codecvt.
        // They forgot std::locale::id for char16_t and char32_t, but not for int32_t
#if (_MSC_VER >= 1900 /* VS 2015*/) && (_MSC_VER <= 1916 /* VS 2017 */)
        ::std::wstring_convert<::std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
        auto p = reinterpret_cast<const int16_t*>(str.data());
        return ::std::stol(convert.to_bytes(p, p + str.size()), pos, base);
#else
        ::std::wstring_convert<::std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
        return ::std::stol(convert.to_bytes(str.c_str()), pos, base);
#endif
    }

    /**
     * @brief stol string to long conversion function for BasicString Utf32.
     */
    long stol(const Clipped::BasicString<char32_t>& str, size_t*& pos, int& base)
    {
        // Workaround for VS - unresolved external symbols of codecvt.
        // They forgot std::locale::id for char16_t and char32_t, but not for int32_t
#if (_MSC_VER >= 1900 /* VS 2015*/) && (_MSC_VER <= 1916 /* VS 2017 */)
        (void)pos; //Silence unused warnings. They're used in non msvc implementations below.
        (void)base;
        ::std::wstring_convert<::std::codecvt_utf8_utf16<int32_t>, int32_t> convert;
        auto p = reinterpret_cast<const int32_t*>(str.data());
        return ::std::stol(convert.to_bytes(p, p + str.size()));
#else
        ::std::wstring_convert<::std::codecvt_utf8<char32_t>, char32_t> convert;
        return ::std::stol(convert.to_bytes(str.c_str()), pos, base);
#endif
    }
}  // namespace std
