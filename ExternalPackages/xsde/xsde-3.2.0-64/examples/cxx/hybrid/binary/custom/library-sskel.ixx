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

namespace library
{
  // isbn_sskel
  //

  inline
  isbn_sskel::
  isbn_sskel (::xml_schema::unsigned_int_sskel* tiein)
  : ::xml_schema::unsigned_int_sskel (tiein, 0),
    isbn_impl_ (0)
  {
  }

  inline
  isbn_sskel::
  isbn_sskel (isbn_sskel* impl, void*)
  : ::xml_schema::unsigned_int_sskel (impl, 0),
    isbn_impl_ (impl)
  {
  }

  // title_sskel
  //

  inline
  void title_sskel::
  lang_serializer (::xml_schema::string_sskel& lang)
  {
    this->lang_serializer_ = &lang;
  }

  inline
  void title_sskel::
  serializers (::xml_schema::string_sskel& lang)
  {
    this->lang_serializer_ = &lang;
  }

  inline
  title_sskel::
  title_sskel (::xml_schema::string_sskel* tiein)
  : ::xml_schema::string_sskel (tiein, 0),
    title_impl_ (0),
    lang_serializer_ (0)
  {
  }

  inline
  title_sskel::
  title_sskel (title_sskel* impl, void*)
  : ::xml_schema::string_sskel (impl, 0),
    title_impl_ (impl),
    lang_serializer_ (0)
  {
  }

  // genre_sskel
  //

  inline
  genre_sskel::
  genre_sskel (::xml_schema::string_sskel* tiein)
  : ::xml_schema::string_sskel (tiein, 0),
    genre_impl_ (0)
  {
    this->_enumeration_facet (_xsde_genre_sskel_enums_, 5UL);
  }

  inline
  genre_sskel::
  genre_sskel (genre_sskel* impl, void*)
  : ::xml_schema::string_sskel (impl, 0),
    genre_impl_ (impl)
  {
    this->_enumeration_facet (_xsde_genre_sskel_enums_, 5UL);
  }

  // author_sskel
  //

  inline
  void author_sskel::
  name_serializer (::xml_schema::string_sskel& s)
  {
    this->name_serializer_ = &s;
  }

  inline
  void author_sskel::
  born_serializer (::xml_schema::date_sskel& s)
  {
    this->born_serializer_ = &s;
  }

  inline
  void author_sskel::
  died_serializer (::xml_schema::date_sskel& s)
  {
    this->died_serializer_ = &s;
  }

  inline
  void author_sskel::
  serializers (::xml_schema::string_sskel& name,
               ::xml_schema::date_sskel& born,
               ::xml_schema::date_sskel& died)
  {
    this->name_serializer_ = &name;
    this->born_serializer_ = &born;
    this->died_serializer_ = &died;
  }

  inline
  author_sskel::
  author_sskel ()
  : author_impl_ (0),
    name_serializer_ (0),
    born_serializer_ (0),
    died_serializer_ (0)
  {
  }

  inline
  author_sskel::
  author_sskel (author_sskel* impl, void*)
  : ::xsde::cxx::serializer::validating::complex_content (impl, 0),
    author_impl_ (impl),
    name_serializer_ (0),
    born_serializer_ (0),
    died_serializer_ (0)
  {
  }

  // book_sskel
  //

  inline
  void book_sskel::
  available_serializer (::xml_schema::boolean_sskel& available)
  {
    this->available_serializer_ = &available;
  }

  inline
  void book_sskel::
  isbn_serializer (::library::isbn_sskel& s)
  {
    this->isbn_serializer_ = &s;
  }

  inline
  void book_sskel::
  title_serializer (::library::title_sskel& s)
  {
    this->title_serializer_ = &s;
  }

  inline
  void book_sskel::
  genre_serializer (::library::genre_sskel& s)
  {
    this->genre_serializer_ = &s;
  }

  inline
  void book_sskel::
  author_serializer (::library::author_sskel& s)
  {
    this->author_serializer_ = &s;
  }

  inline
  void book_sskel::
  serializers (::xml_schema::boolean_sskel& available,
               ::library::isbn_sskel& isbn,
               ::library::title_sskel& title,
               ::library::genre_sskel& genre,
               ::library::author_sskel& author)
  {
    this->available_serializer_ = &available;
    this->isbn_serializer_ = &isbn;
    this->title_serializer_ = &title;
    this->genre_serializer_ = &genre;
    this->author_serializer_ = &author;
  }

  inline
  book_sskel::
  book_sskel ()
  : book_impl_ (0),
    available_serializer_ (0),
    isbn_serializer_ (0),
    title_serializer_ (0),
    genre_serializer_ (0),
    author_serializer_ (0)
  {
  }

  inline
  book_sskel::
  book_sskel (book_sskel* impl, void*)
  : ::xsde::cxx::serializer::validating::complex_content (impl, 0),
    book_impl_ (impl),
    available_serializer_ (0),
    isbn_serializer_ (0),
    title_serializer_ (0),
    genre_serializer_ (0),
    author_serializer_ (0)
  {
  }

  // catalog_sskel
  //

  inline
  void catalog_sskel::
  book_serializer (::library::book_sskel& s)
  {
    this->book_serializer_ = &s;
  }

  inline
  void catalog_sskel::
  serializers (::library::book_sskel& book)
  {
    this->book_serializer_ = &book;
  }

  inline
  catalog_sskel::
  catalog_sskel ()
  : catalog_impl_ (0),
    book_serializer_ (0)
  {
  }

  inline
  catalog_sskel::
  catalog_sskel (catalog_sskel* impl, void*)
  : ::xsde::cxx::serializer::validating::complex_content (impl, 0),
    catalog_impl_ (impl),
    book_serializer_ (0)
  {
  }
}

// Begin epilogue.
//
//
// End epilogue.
