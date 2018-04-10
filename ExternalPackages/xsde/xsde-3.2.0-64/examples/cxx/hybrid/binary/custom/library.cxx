// Copyright (c) 2005-2011 Code Synthesis Tools CC
//
// This program was generated by CodeSynthesis XSD/e, an XML Schema
// to C++ data binding compiler for embedded systems.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
//
//

#include <xsde/cxx/pre.hxx>

// Begin prologue.
//
//
// End prologue.

#include "library.hxx"

#include <stdlib.h>
#include <new>

namespace library
{
  // isbn
  //

  isbn::
  isbn ()
  {
  }

  // title
  //

  title::
  title ()
  {
    this->lang_present_ = false;
  }

  title::
  ~title ()
  {
  }

  title::
  title (const title& x)
  : ::std::string (x)
  {
    XSDE_UNUSED (x);
    this->lang_present_ = x.lang_present_;
    if (this->lang_present_)
      this->lang_ = x.lang_;
  }

  title& title::
  operator= (const title& x)
  {
    XSDE_UNUSED (x);
    static_cast< ::std::string& > (*this) = x;
    this->lang_present_ = x.lang_present_;
    if (this->lang_present_)
      this->lang_ = x.lang_;
    return *this;
  }

  // genre
  //

  static const char* _xsde_genre_enumerators_[] = 
  {
    "romance",
    "fiction",
    "horror",
    "history",
    "philosophy"
  };

  const char* genre::
  string () const
  {
    return _xsde_genre_enumerators_[value_];
  }

  // author
  //

  author::
  author ()
  {
    this->died_present_ = false;
  }

  author::
  ~author ()
  {
  }

  author::
  author (const author& x)
  {
    XSDE_UNUSED (x);
    this->name_ = x.name_;
    this->born_ = x.born_;
    this->died_present_ = x.died_present_;
    if (this->died_present_)
      this->died_ = x.died_;
  }

  author& author::
  operator= (const author& x)
  {
    XSDE_UNUSED (x);
    this->name_ = x.name_;
    this->born_ = x.born_;
    this->died_present_ = x.died_present_;
    if (this->died_present_)
      this->died_ = x.died_;
    return *this;
  }

  // book
  //

  book::
  book ()
  {
    this->available_ = available_default_value ();
  }

  book::
  ~book ()
  {
  }

  bool book::
  available_default_value ()
  {
    return true;
  }

  // catalog
  //

  catalog::
  catalog ()
  {
  }

  catalog::
  ~catalog ()
  {
  }
}

namespace library
{
  void
  operator<< (orawstream& s,
              const isbn& x)
  {
    XSDE_UNUSED (s);
    XSDE_UNUSED (x);

    unsigned int b = x;
    s << b;
  }

  void
  operator<< (orawstream& s,
              const title& x)
  {
    XSDE_UNUSED (s);
    XSDE_UNUSED (x);

    const ::std::string& b = x;
    s << b;
    s << x.lang_present ();

    if (x.lang_present ())
    {
      s << x.lang ();
    }
  }

  void
  operator<< (orawstream& s,
              const genre& x)
  {
    unsigned int i = x;
    s << i;
  }


  void
  operator<< (orawstream& s,
              const author& x)
  {
    XSDE_UNUSED (s);
    XSDE_UNUSED (x);

    s << x.name ();
    s << x.born ();
    s << x.died_present ();

    if (x.died_present ())
    {
      s << x.died ();
    }
  }

  void
  operator<< (orawstream& s,
              const book& x)
  {
    XSDE_UNUSED (s);
    XSDE_UNUSED (x);

    s << x.available_default ();

    if (!x.available_default ())
    {
      s << x.available ();
    }

    s << x.isbn ();
    s << x.title ();
    s << x.genre ();
    s << x.author ();
  }

  void
  operator<< (orawstream& s,
              const catalog& x)
  {
    XSDE_UNUSED (s);
    XSDE_UNUSED (x);

    s << x.book ();
  }
}

namespace library
{
  void
  operator>> (irawstream& s,
              isbn& x)
  {
    XSDE_UNUSED (s);
    XSDE_UNUSED (x);

    unsigned int& b = x;
    s >> b;
  }

  void
  operator>> (irawstream& s,
              title& x)
  {
    XSDE_UNUSED (s);
    XSDE_UNUSED (x);

    ::std::string& b = x;
    s >> b;
    {
      bool p;
      s >> p;

      if (p)
      {
        x.lang_present (true);
        s >> x.lang ();
      }
      else
        x.lang_present (false);
    }
  }

  void
  operator>> (irawstream& s,
              genre& x)
  {
    unsigned int i;
    s >> i;
    x = static_cast< genre::value_type > (i);
  }


  void
  operator>> (irawstream& s,
              author& x)
  {
    XSDE_UNUSED (s);
    XSDE_UNUSED (x);

    {
      s >> x.name ();
    }

    {
      s >> x.born ();
    }

    {
      bool p;
      s >> p;

      if (p)
      {
        x.died_present (true);
        s >> x.died ();
      }
      else
        x.died_present (false);
    }
  }

  void
  operator>> (irawstream& s,
              book& x)
  {
    XSDE_UNUSED (s);
    XSDE_UNUSED (x);

    {
      bool d;
      s >> d;

      if (!d)
      {
        s >> x.available ();
      }
      else
        x.available_default (true);
    }

    {
      s >> x.isbn ();
    }

    {
      s >> x.title ();
    }

    {
      s >> x.genre ();
    }

    s >> x.author ();
  }

  void
  operator>> (irawstream& s,
              catalog& x)
  {
    XSDE_UNUSED (s);
    XSDE_UNUSED (x);

    s >> x.book ();
  }
}

// Begin epilogue.
//
//
// End epilogue.

#include <xsde/cxx/post.hxx>

