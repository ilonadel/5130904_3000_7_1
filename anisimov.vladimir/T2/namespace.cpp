#include "namespace.h"
#include <cmath>

namespace anisimov
{
  std::string binaryNull(unsigned long long ref)
  {
    std::stringstream ss;
    ss << "0b" << std::setfill('0') << std::setw(2) << std::to_string(ref);
    std::string out = ss.str();
    size_t i = out.find('b');
    if (out[i + 1] == '0' and out[i + 2] == '0')
    {
      out.erase(i + 2, 1);
    }
    return out;
  }

  std::istream& operator>>(std::istream& in, DelimiterIO&& dest)
  {
    std::istream::sentry sentry(in);
    if (!sentry)
    {
      return in;
    }
    char ch = '0';
    in >> ch;
    if (in && (ch != dest.exp))
    {
      in.setstate(std::ios::failbit);
    }
    return in;
  }

  std::istream& operator>>(std::istream& in, ULongLiteralIO&& dest)
  {
    std::istream::sentry sentry(in);
    if (!sentry)
    {
      return in;
    }
    return in >> dest.ref;
  }

  std::istream& operator>>(std::istream& in, ULongBinaryLiteralIO&& dest)
  {
    std::istream::sentry sentry(in);
    if (!sentry)
    {
      return in;
    }
    return in >> dest.ref;
  }

  std::istream& operator>>(std::istream& in, StringIO&& dest)
  {
    std::istream::sentry sentry(in);
    if (!sentry)
    {
      return in;
    }
    return std::getline(in >> DelimiterIO{ '"' }, dest.ref, '"');
  }

  std::istream& operator>>(std::istream& in, DataStruct& dest)
  {
    std::istream::sentry sentry(in);
    if (!sentry)
    {
      return in;
    }

    DataStruct input;
    {
      using sep = DelimiterIO;
      using ull = ULongLiteralIO;
      using str = StringIO;
      in >> sep{ '(' };
      bool flag1 = false;
      bool flag2 = false;
      bool flag3 = false;
      while (true)
      {
        if (flag1 && flag2 && flag3)
        {
          break;
        }
        std::string key;
        char ch;
        in >> ch;
        if (!in)
        {
          break;
        }

        if (ch == ':' && (in >> key))
        {
          if (key == "key1")
          {
            in >> ull{ input.key1 } >> sep{ 'u' } >> sep{ 'l' } >> sep{ 'l' };
            flag1 = true;
          }
          else if (key == "key2")
          {
            unsigned long long realPart, imagPart;
            in >> sep{ '(' } >> ull{ realPart } >> sep{ ',' } >> ull{ imagPart } >> sep{ ')' };
            input.key2 = std::complex<double>(realPart, imagPart);
            flag2 = true;
          }
          else if (key == "key3")
          {
            in >> str{ input.key3 };
            flag3 = true;
          }
        }
      }
      in >> sep{ ':' } >> sep{ ')' };
    }
    if (in)
    {
      dest = input;
    }
    return in;
  }

  std::ostream& operator<<(std::ostream& out, const DataStruct& src)
  {
    std::ostream::sentry sentry(out);
    if (!sentry)
    {
      return out;
    }
    iofmtguard fmtguard(out);
    out << "(";
    out << ":key1 " << src.key1 << "ull";
    out << ":key2 (" << src.key2.real() << "," << src.key2.imag() << ")";
    out << ":key3 " << "\"" << src.key3 << "\"";
    out << ":)";
    return out;
  }

  bool compareDataStruct(const DataStruct& a, const DataStruct& b)
  {
    if (a.key1 != b.key1)
    {
      return a.key1 < b.key1;
    }
    else
    {
      
      return std::abs(a.key2) < std::abs(b.key2);
    }
  }

  iofmtguard::iofmtguard(std::basic_ios<char>& s) :
    s_(s),
    fill_(s.fill()),
    precision_(s.precision()),
    fmt_(s.flags())
  {}

  iofmtguard::~iofmtguard()
  {
    s_.fill(fill_);
    s_.precision(precision_);
    s_.flags(fmt_);
  }
}
