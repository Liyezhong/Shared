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

// Begin prologue.
//
//
// End prologue.

#include "library-pimpl.hxx"

#include <xsde/cxx/parser/validating/string-common.hxx>

namespace library
{
  // isbn_pimpl
  //

  isbn_pimpl::
  isbn_pimpl ()
  : isbn_pskel (&base_impl_)
  {
  }

  void isbn_pimpl::
  pre ()
  {
    this->base_impl_.pre ();
    this->isbn_pimpl_state_.isbn_ = ::library::isbn ();
  }

  ::library::isbn isbn_pimpl::
  post_isbn ()
  {
    static_cast< ::xml_schema::unsigned_int_base& > (this->isbn_pimpl_state_.isbn_) = 
    this->base_impl_.post_unsigned_int ();
    return this->isbn_pimpl_state_.isbn_;
  }

  // title_pimpl
  //

  title_pimpl::
  title_pimpl ()
  : title_pskel (&base_impl_)
  {
  }

  void title_pimpl::
  pre ()
  {
    this->base_impl_.pre ();
    this->title_pimpl_state_.title_ = ::library::title ();
  }

  void title_pimpl::
  lang (const ::std::string& x)
  {
    this->title_pimpl_state_.title_.lang (x);
  }

  ::library::title title_pimpl::
  post_title ()
  {
    static_cast< ::std::string& > (this->title_pimpl_state_.title_) = 
    this->base_impl_.post_string ();
    return this->title_pimpl_state_.title_;
  }

  // genre_pimpl
  //

  genre_pimpl::
  genre_pimpl ()
  : genre_pskel (0)
  {
  }

  void genre_pimpl::
  pre ()
  {
    this->genre_pimpl_state_.str_.clear ();
  }

  void genre_pimpl::
  _characters (const ::xsde::cxx::ro_string& s)
  {
    if (this->_facets ().whitespace_ == 2 &&
        this->genre_pimpl_state_.str_.size () == 0)
    {
      ::xsde::cxx::ro_string tmp (s.data (), s.size ());

      if (::xsde::cxx::trim_left (tmp) != 0)
      {
        this->genre_pimpl_state_.str_ += tmp;
      }
    }
    else
      this->genre_pimpl_state_.str_ += s;
  }

  void genre_pimpl::
  _post ()
  {
    ::xsde::cxx::parser::validating::string_common::validate_facets (
      this->genre_pimpl_state_.str_,
      this->_facets (),
      this->_context ());
  }

  ::library::genre genre_pimpl::
  post_genre ()
  {
    ::library::genre::value_type v =
    static_cast< ::library::genre::value_type > (0);
    const char* s = this->genre_pimpl_state_.str_.c_str ();

    if (strcmp (s, "romance") == 0)
      v = ::library::genre::romance;
    else if (strcmp (s, "fiction") == 0)
      v = ::library::genre::fiction;
    else if (strcmp (s, "horror") == 0)
      v = ::library::genre::horror;
    else if (strcmp (s, "history") == 0)
      v = ::library::genre::history;
    else if (strcmp (s, "philosophy") == 0)
      v = ::library::genre::philosophy;

    ::library::genre r (v);
    return r;
  }

  // author_pimpl
  //

  void author_pimpl::
  pre ()
  {
    this->author_pimpl_state_.author_ = ::library::author ();
  }

  void author_pimpl::
  name (const ::std::string& x)
  {
    this->author_pimpl_state_.author_.name (x);
  }

  void author_pimpl::
  born (const ::xml_schema::date& x)
  {
    this->author_pimpl_state_.author_.born (x);
  }

  void author_pimpl::
  died (const ::xml_schema::date& x)
  {
    this->author_pimpl_state_.author_.died (x);
  }

  ::library::author author_pimpl::
  post_author ()
  {
    return this->author_pimpl_state_.author_;
  }

  // book_pimpl
  //

  book_pimpl::
  book_pimpl (bool b)
  {
    this->book_pimpl_base_ = b;
    this->book_pimpl_state_.book_ = 0;
  }

  book_pimpl::
  ~book_pimpl ()
  {
    if (!this->book_pimpl_base_ && this->book_pimpl_state_.book_)
      delete this->book_pimpl_state_.book_;
  }

  void book_pimpl::
  _reset ()
  {
    book_pskel::_reset ();

    if (!this->book_pimpl_base_ && this->book_pimpl_state_.book_)
    {
      delete this->book_pimpl_state_.book_;
      this->book_pimpl_state_.book_ = 0;
    }
  }

  void book_pimpl::
  pre_impl (::library::book* x)
  {
    this->book_pimpl_state_.book_ = x;
  }

  void book_pimpl::
  pre ()
  {
    ::library::book* x = new ::library::book;
    this->pre_impl (x);
  }

  void book_pimpl::
  available (bool x)
  {
    this->book_pimpl_state_.book_->available (x);
  }

  void book_pimpl::
  isbn (const ::library::isbn& x)
  {
    this->book_pimpl_state_.book_->isbn (x);
  }

  void book_pimpl::
  title (const ::library::title& x)
  {
    this->book_pimpl_state_.book_->title (x);
  }

  void book_pimpl::
  genre (const ::library::genre& x)
  {
    this->book_pimpl_state_.book_->genre (x);
  }

  void book_pimpl::
  author (const ::library::author& x)
  {
    this->book_pimpl_state_.book_->author ().push_back (x);
  }

  ::library::book* book_pimpl::
  post_book ()
  {
    ::library::book* r = this->book_pimpl_state_.book_;
    this->book_pimpl_state_.book_ = 0;
    return r;
  }

  // catalog_pimpl
  //

  catalog_pimpl::
  catalog_pimpl (bool b)
  {
    this->catalog_pimpl_base_ = b;
    this->catalog_pimpl_state_.catalog_ = 0;
  }

  catalog_pimpl::
  ~catalog_pimpl ()
  {
    if (!this->catalog_pimpl_base_ && this->catalog_pimpl_state_.catalog_)
      delete this->catalog_pimpl_state_.catalog_;
  }

  void catalog_pimpl::
  _reset ()
  {
    catalog_pskel::_reset ();

    if (!this->catalog_pimpl_base_ && this->catalog_pimpl_state_.catalog_)
    {
      delete this->catalog_pimpl_state_.catalog_;
      this->catalog_pimpl_state_.catalog_ = 0;
    }
  }

  void catalog_pimpl::
  pre_impl (::library::catalog* x)
  {
    this->catalog_pimpl_state_.catalog_ = x;
  }

  void catalog_pimpl::
  pre ()
  {
    ::library::catalog* x = new ::library::catalog;
    this->pre_impl (x);
  }

  void catalog_pimpl::
  book (::library::book* x)
  {
    this->catalog_pimpl_state_.catalog_->book ().push_back (x);
  }

  ::library::catalog* catalog_pimpl::
  post_catalog ()
  {
    ::library::catalog* r = this->catalog_pimpl_state_.catalog_;
    this->catalog_pimpl_state_.catalog_ = 0;
    return r;
  }
}

namespace library
{
  // catalog_paggr
  //

  catalog_paggr::
  catalog_paggr ()
  {
    this->title_p_.parsers (this->string_p_);

    this->author_p_.parsers (this->string_p_,
                             this->date_p_,
                             this->date_p_);

    this->book_p_.parsers (this->boolean_p_,
                           this->isbn_p_,
                           this->title_p_,
                           this->genre_p_,
                           this->author_p_);

    this->catalog_p_.parsers (this->book_p_);
  }

  const char* catalog_paggr::
  root_name ()
  {
    return "catalog";
  }

  const char* catalog_paggr::
  root_namespace ()
  {
    return "http://www.codesynthesis.com/library";
  }
}

// Begin epilogue.
//
//
// End epilogue.
