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

#pragma once

#include <sstream>
#include <string>
#include <vector>

namespace Clipped
{
    // Forward declaration:
    template <class T>
    class BasicStringStream;

    /**
     * @brief Class BasicString extendable string class for different char types.
     */
    template <class T>
    class BasicString : public std::basic_string<T, std::char_traits<T>, std::allocator<T>>
    {
    public:
        BasicString();

        /**
         * @brief BasicString constructs a BasicString object from an const ptr initializer.
         * @param (in) s string to initialize this String objects with.
         */
        BasicString(const T* s);

        /**
         * @brief BasicString constructs a BasicString object from an const ptr initializer.
         * @param s ptr to character to init from.
         * @param n elements are taken from s
         */
        BasicString(const T* s, size_t n);

        /**
         * @brief BasicString constructs a BasicString object from an vector.
         * @param s vector to initialize with.
         */
        BasicString(const std::vector<T>& s);

        /**
         * @brief BasicString Copy constructor.
         * @param s to copy.
         */
        BasicString(const BasicString<T>& s);

        /**
         * @brief BasicString Copy constructor.
         * @param s to copy.
         */
        BasicString(const std::basic_string<T>& s);

        /**
         * @brief BasicString creates a string representing the float value.
         * @param value number to initialize from.
         */
        BasicString(const float& value);

        /**
         * @brief BasicString creates a string representing the float value with given precision.
         * @param value the float to display.
         * @param precision number of digits behind the comma.
         */
        BasicString(const float& value, size_t precision);

        /**
         * @brief BasicString creates a string representing the double value with given precision.
         * @param value the double to display.
         * @param precision number of digits behind the comma.
         */
        BasicString(const double& value, size_t precision);

        /**
         * @brief BasicString creates a string representing the int value.
         * @param value number to initialize from.
         */
        BasicString(const int& value);

        /**
         * @brief BasicString creates a string representing the unsigned long long value.
         * @param value number to initialize from.
         */
        BasicString(const unsigned long long& value);

        /**
         * @brief toUpper uppers a string.
         * @return an upper case representation of this string.
         */
        BasicString toUpper() const;

        /**
         * @brief toLower lowers a string.
         * @return  a lower case representation of this string.
         */
        BasicString toLower() const;

        /**
         * @brief toString converts this string to an ascii string.
         * @return a String aka BasicString<char>.
         */
        BasicString<char> toString() const;

        /**
         * @brief toWString converts this string to an wide string.
         * @return a WString aka BasicString<wchar_t>.
         */
        BasicString<wchar_t> toWString() const;

        /**
         * @brief toVector converts this string to an char vector.
         * @return a vector containing chars.
         */
        std::vector<char> toVector() const;

        /**
         * @brief toHexString creates a hexadecimal representation of the string.
         * @param uppercase if true, Hex letters are displayed uppercase.
         * @param delimiter to be put between the 8bit numbers.
         * @return the hex representation as string.
         */
        BasicString<T> toHexString(bool uppercase, const BasicString<T> delimiter = " ") const;

        /**
         * @brief fromAsci sets this string from an ascii input.
         * @return a reference of this string.
         */
        static BasicString<T> fromAsci(const char* str);

        /**
         * @brief toInt casts this string to an integer.
         * @param pos position index to start interpreting at.
         * @param base for the conversion.
         * @return the resulting integer.
         */
        int toInt(size_t* pos = nullptr, int base = 10) const;

        /**
         * @brief toDouble casts this string to a double value.
         * @return a double.
         */
        double toDouble() const;

        /**
         * @brief toLong casts this string to a long.
         * @param pos position index to start interpreting at.
         * @param base for the conversion.
         * @return the resulting long.
         */
        long toLong(size_t* pos = nullptr, int base = 10) const;

        /**
         * @brief isNumber checks, if the current string is a valid number.
         * @return true if contained chars are numerics, false otherwise.
         */
        bool isNumber() const;

        /**
         * @brief equals checks, if the right hand side string has an identical content.
         * @param rhs string to compare with.
         * @return true, if identical contents, false otherwise.
         */
        bool equals(const BasicString<T>& rhs) const;

        /** @copydoc BasicString<T>::equals */
        bool operator==(const BasicString<T>& rhs) const;

        /** @copydoc BasicString<T>::equals */
        bool operator==(const T* rhs) const;

        /**
         * @brief contains checks, if a string is contained in this string.
         * @param search the content to search for in this string.
         * @param ignoreCase flag specifying, if the case shall be ignored.
         * @return true, if the search string is contained in this string.
         */
        bool contains(const BasicString<T>& search, bool ignoreCase = true) const;

        /**
         * @brief endsWith checks if the string ends with given text.
         * @param value to be checked.
         * @param ignoreCase true if the case should be ignored.
         * @return true if the string ends with the given value.
         */
        bool endsWith(const BasicString<T>& value, bool ignoreCase = false) const;

        /**
         * @brief replace replace all occurences of search with replace.
         * @param search to replace
         * @param replace to replace with
         * @return a reference to this string.
         */
        BasicString<T>& replace(const BasicString<T>& search, const BasicString<T>& replace);

        /**
         * @brief split splits a and stores the elements in the given vector.
         * @param delim character delimeter to split at.
         * @param elems out elements found.
         * @return reference of the given vector.
         */
        std::vector<BasicString<T>>& split(const T delim, std::vector<BasicString<T>>& elems) const;

        /**
         * @brief split splits a string with a given char delimiter.
         * @param delim to split at.
         * @return a vector containing the resulting parts.
         */
        std::vector<BasicString<T>> split(const T delim) const;

        /**
         * @brief split splits a string with a given delimiter string.
         * @param delim string to split with.
         * @return a vector containing the resulting parts.
         */
        std::vector<BasicString<T>> split(const BasicString<T>& delim) const;

        /**
         * @brief trim trimms the given char from both sides of the string.
         * @param trimChar char that gets trimmed.
         * @return the trimmed string copy.
         */
        BasicString<T> trim(const BasicString<T>& trimChar) const;

        /**
         * @brief trimRight trims the given char from the right side of the string.
         * @param trimChar char that gets trimmed.
         * @return the trimmed string copy.
         */
        BasicString<T> trimRight() const;

        /**
         * @brief trimLeft trims whitespace charactetrs from the left side of the string.
         * @return a trimmed string copy.
         */
        BasicString<T> trimLeft() const;

        /**
         * @brief trim trims whitespace characters from both sides of the string.
         *   Call trimLeft or trimRight if you want to trim only one side.
         * @return a trimmed string copy.
         */
        BasicString<T> trim() const;

        /**
         * @brief fill fills up the string with fillChar.
         *  Note: If fillChar length is greather 1, the string possibly will be shorter than length.
         * @param fillChar string to fillup with.
         * @param length to fillup the string to.
         * @return the filled up string.
         */
        BasicString<T> fill(const BasicString<T>& fillChar, size_t length) const;

    };  // class BasicString

    // Using directives for common string types:
    using String = BasicString<char>;         // 8-Bit ASCII String
    using WString = BasicString<wchar_t>;     // 8-Bit Wide char String
    using U16String = BasicString<char16_t>;  // 16-Bit Unicode String.
    using U32String = BasicString<char32_t>;  // 32-Bit Unicode String.

    template <class T>
    class BasicStringStream
        : public std::basic_stringstream<T, std::char_traits<T>, std::allocator<T>>
    {
    public:
        BasicStringStream();
        BasicStringStream(const BasicString<T>& str);
    };  // class BasicStringStream

    using StringStream = BasicStringStream<char>;
    using WStringStream = BasicStringStream<wchar_t>;
    using U16StringStream = BasicStringStream<char16_t>;
    using U32StringStream = BasicStringStream<char32_t>;

    //Tell the compiler what template instanciations are compiled (fixes -Wundefined-func-template)
    extern template class BasicString<char>;
    extern template class BasicStringStream<char>;
#ifdef CLIPPED_BUILD_WIDE
    extern template class BasicString<wchar_t>;
    extern template class BasicStringStream<wchar_t>;
#endif
#ifdef CLIPPED_BUILD_U16
    extern template class BasicString<char16_t>;
    extern template class BasicStringStream<char16_t>;
#endif
#ifdef CLIPPED_BUILD_U32
    extern template class BasicString<char32_t>;
    extern template class BasicStringStream<char32_t>;
#endif

}  // namespace Clipped

namespace std
{
    template <class T>
    /**
     * @brief The hash<Clipped::BasicString<T> > struct
     *        Injection of a hash function required to use this BasicStrings in containers like
     *        unordered maps or pairs.
     */
    struct hash<Clipped::BasicString<T>>
    {
        size_t operator()(const Clipped::BasicString<T>& s) const
        {
            std::hash<std::basic_string<T>> hash_fn;
            return hash_fn(s);
        }
    };
}  // namespace std
