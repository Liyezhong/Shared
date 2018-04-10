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

#ifndef LIBRARY_PSKEL_HXX
#define LIBRARY_PSKEL_HXX

#include <xsde/cxx/version.hxx>

#if (XSDE_INT_VERSION != 3020000L)
#error XSD/e runtime version mismatch
#endif

#include <xsde/cxx/config.hxx>

#ifndef XSDE_ENCODING_UTF8
#error the generated code uses the UTF-8 encodingwhile the XSD/e runtime does not (reconfigure the runtime or change the --char-encoding value)
#endif

#ifndef XSDE_STL
#error the generated code uses STL while the XSD/e runtime does not (reconfigure the runtime or add --no-stl)
#endif

#ifndef XSDE_IOSTREAM
#error the generated code uses iostream while the XSD/e runtime does not (reconfigure the runtime or add --no-iostream)
#endif

#ifndef XSDE_EXCEPTIONS
#error the generated code uses exceptions while the XSD/e runtime does not (reconfigure the runtime or add --no-exceptions)
#endif

#ifndef XSDE_LONGLONG
#error the generated code uses long long while the XSD/e runtime does not (reconfigure the runtime or add --no-long-long)
#endif

#ifndef XSDE_PARSER_VALIDATION
#error the generated code uses validation while the XSD/e runtime does not (reconfigure the runtime or add --suppress-validation)
#endif

#ifdef XSDE_POLYMORPHIC
#error the generated code expects XSD/e runtime without polymorphism support (reconfigure the runtime or add --generate-polymorphic/--runtime-polymorphic)
#endif

#ifndef XSDE_REUSE_STYLE_TIEIN
#error the generated code uses the tiein reuse style while the XSD/e runtime does not (reconfigure the runtime or add --reuse-style-mixin or --reuse-style-none)
#endif

#ifdef XSDE_CUSTOM_ALLOCATOR
#error the XSD/e runtime uses custom allocator while the generated code does not (reconfigure the runtime or add --custom-allocator)
#endif

#include <xsde/cxx/pre.hxx>

// Begin prologue.
//

#include <xsde/cxx/hybrid/any-type.hxx>
#include <xsde/cxx/hybrid/any-type-pskel.hxx>
#include <xsde/cxx/hybrid/any-type-pimpl.hxx>

namespace xml_schema
{
  using ::xsde::cxx::hybrid::any_type;

  using ::xsde::cxx::hybrid::any_type_pskel;
  using ::xsde::cxx::hybrid::any_type_pimpl;
}


//
// End prologue.

// Forward declarations
//
namespace library
{
  class isbn_pskel;
  class title_pskel;
  class genre_pskel;
  class author_pskel;
  class book_pskel;
  class catalog_pskel;
}


#include "xml-schema-pskel.hxx"

#include "library.hxx"

namespace library
{
  class isbn_pskel: public ::xml_schema::unsigned_int_pskel
  {
    public:
    // Parser callbacks. Override them in your implementation.
    //
    // virtual void
    // pre ();

    virtual ::library::isbn
    post_isbn () = 0;

    // Constructor.
    //
    isbn_pskel (::xml_schema::unsigned_int_pskel* tiein);

    // Implementation details.
    //
    virtual unsigned int
    post_unsigned_int ();

    protected:
    isbn_pskel* isbn_impl_;
    isbn_pskel (isbn_pskel*, void*);
  };

  class title_pskel: public ::xml_schema::string_pskel
  {
    public:
    // Parser callbacks. Override them in your implementation.
    //
    // virtual void
    // pre ();

    // Attributes.
    //
    virtual void
    lang (const ::std::string&);

    virtual ::library::title
    post_title () = 0;

    // Parser construction API.
    //
    void
    parsers (::xml_schema::string_pskel& /* lang */);

    // Individual attribute parsers.
    //
    void
    lang_parser (::xml_schema::string_pskel&);

    virtual void
    _reset ();

    // Constructor.
    //
    title_pskel (::xml_schema::string_pskel* tiein);

    // Implementation details.
    //
    virtual ::std::string
    post_string ();

    protected:
    title_pskel* title_impl_;
    title_pskel (title_pskel*, void*);

    protected:
    virtual bool
    _attribute_impl_phase_one (const ::xsde::cxx::ro_string&,
                               const ::xsde::cxx::ro_string&,
                               const ::xsde::cxx::ro_string&);


    protected:
    ::xml_schema::string_pskel* lang_parser_;
  };

  class genre_pskel: public ::xml_schema::string_pskel
  {
    public:
    // Parser callbacks. Override them in your implementation.
    //
    // virtual void
    // pre ();

    virtual ::library::genre
    post_genre () = 0;

    // Constructor.
    //
    genre_pskel (::xml_schema::string_pskel* tiein);

    // Implementation details.
    //
    virtual ::std::string
    post_string ();

    protected:
    genre_pskel* genre_impl_;
    genre_pskel (genre_pskel*, void*);

    protected:
    static const char* const _xsde_genre_pskel_enums_[5UL];
  };

  class author_pskel: public ::xsde::cxx::parser::validating::complex_content
  {
    public:
    // Parser callbacks. Override them in your implementation.
    //
    // virtual void
    // pre ();

    // Elements.
    //
    virtual void
    name (const ::std::string&);

    virtual void
    born (const ::xml_schema::date&);

    virtual void
    died (const ::xml_schema::date&);

    virtual ::library::author
    post_author () = 0;

    // Parser construction API.
    //
    void
    parsers (::xml_schema::string_pskel& /* name */,
             ::xml_schema::date_pskel& /* born */,
             ::xml_schema::date_pskel& /* died */);

    // Individual element parsers.
    //
    void
    name_parser (::xml_schema::string_pskel&);

    void
    born_parser (::xml_schema::date_pskel&);

    void
    died_parser (::xml_schema::date_pskel&);

    virtual void
    _reset ();

    // Constructor.
    //
    author_pskel ();

    // Implementation details.
    //
    protected:
    author_pskel* author_impl_;
    author_pskel (author_pskel*, void*);

    protected:
    virtual bool
    _start_element_impl (const ::xsde::cxx::ro_string&,
                         const ::xsde::cxx::ro_string&);

    virtual bool
    _end_element_impl (const ::xsde::cxx::ro_string&,
                       const ::xsde::cxx::ro_string&);

    protected:
    ::xml_schema::string_pskel* name_parser_;
    ::xml_schema::date_pskel* born_parser_;
    ::xml_schema::date_pskel* died_parser_;

    public:
    struct v_state_descr_
    {
      void (::library::author_pskel::*func) (
        unsigned long&,
        unsigned long&,
        const ::xsde::cxx::ro_string&,
        const ::xsde::cxx::ro_string&,
        bool);
      unsigned long state;
      unsigned long count;
    };

    struct v_state_
    {
      v_state_descr_ data[2UL];
      unsigned long size;
    };

    protected:
    v_state_ v_state_first_;
    ::xsde::cxx::stack v_state_stack_;

    virtual void
    _pre_e_validate ();

    virtual void
    _post_e_validate ();

    void
    sequence_0 (unsigned long&,
                unsigned long&,
                const ::xsde::cxx::ro_string&,
                const ::xsde::cxx::ro_string&,
                bool);
  };

  class book_pskel: public ::xsde::cxx::parser::validating::complex_content
  {
    public:
    // Parser callbacks. Override them in your implementation.
    //
    // virtual void
    // pre ();

    // Attributes.
    //
    virtual void
    available (bool);

    // Elements.
    //
    virtual void
    isbn (const ::library::isbn&);

    virtual void
    title (const ::library::title&);

    virtual void
    genre (const ::library::genre&);

    virtual void
    author (const ::library::author&);

    virtual ::library::book*
    post_book () = 0;

    // Parser construction API.
    //
    void
    parsers (::xml_schema::boolean_pskel& /* available */,
             ::library::isbn_pskel& /* isbn */,
             ::library::title_pskel& /* title */,
             ::library::genre_pskel& /* genre */,
             ::library::author_pskel& /* author */);

    // Individual attribute parsers.
    //
    void
    available_parser (::xml_schema::boolean_pskel&);

    // Individual element parsers.
    //
    void
    isbn_parser (::library::isbn_pskel&);

    void
    title_parser (::library::title_pskel&);

    void
    genre_parser (::library::genre_pskel&);

    void
    author_parser (::library::author_pskel&);

    virtual void
    _reset ();

    // Constructor.
    //
    book_pskel ();

    // Implementation details.
    //
    protected:
    book_pskel* book_impl_;
    book_pskel (book_pskel*, void*);

    protected:
    virtual bool
    _start_element_impl (const ::xsde::cxx::ro_string&,
                         const ::xsde::cxx::ro_string&);

    virtual bool
    _end_element_impl (const ::xsde::cxx::ro_string&,
                       const ::xsde::cxx::ro_string&);

    protected:
    virtual bool
    _attribute_impl_phase_one (const ::xsde::cxx::ro_string&,
                               const ::xsde::cxx::ro_string&,
                               const ::xsde::cxx::ro_string&);


    protected:
    ::xml_schema::boolean_pskel* available_parser_;
    ::library::isbn_pskel* isbn_parser_;
    ::library::title_pskel* title_parser_;
    ::library::genre_pskel* genre_parser_;
    ::library::author_pskel* author_parser_;

    public:
    struct v_state_descr_
    {
      void (::library::book_pskel::*func) (
        unsigned long&,
        unsigned long&,
        const ::xsde::cxx::ro_string&,
        const ::xsde::cxx::ro_string&,
        bool);
      unsigned long state;
      unsigned long count;
    };

    struct v_state_
    {
      v_state_descr_ data[2UL];
      unsigned long size;
    };

    protected:
    v_state_ v_state_first_;
    ::xsde::cxx::stack v_state_stack_;

    virtual void
    _pre_e_validate ();

    virtual void
    _post_e_validate ();

    void
    sequence_0 (unsigned long&,
                unsigned long&,
                const ::xsde::cxx::ro_string&,
                const ::xsde::cxx::ro_string&,
                bool);
  };

  class catalog_pskel: public ::xsde::cxx::parser::validating::complex_content
  {
    public:
    // Parser callbacks. Override them in your implementation.
    //
    // virtual void
    // pre ();

    // Elements.
    //
    virtual void
    book (::library::book*);

    virtual ::library::catalog*
    post_catalog () = 0;

    // Parser construction API.
    //
    void
    parsers (::library::book_pskel& /* book */);

    // Individual element parsers.
    //
    void
    book_parser (::library::book_pskel&);

    virtual void
    _reset ();

    // Constructor.
    //
    catalog_pskel ();

    // Implementation details.
    //
    protected:
    catalog_pskel* catalog_impl_;
    catalog_pskel (catalog_pskel*, void*);

    protected:
    virtual bool
    _start_element_impl (const ::xsde::cxx::ro_string&,
                         const ::xsde::cxx::ro_string&);

    virtual bool
    _end_element_impl (const ::xsde::cxx::ro_string&,
                       const ::xsde::cxx::ro_string&);

    protected:
    ::library::book_pskel* book_parser_;

    public:
    struct v_state_descr_
    {
      void (::library::catalog_pskel::*func) (
        unsigned long&,
        unsigned long&,
        const ::xsde::cxx::ro_string&,
        const ::xsde::cxx::ro_string&,
        bool);
      unsigned long state;
      unsigned long count;
    };

    struct v_state_
    {
      v_state_descr_ data[2UL];
      unsigned long size;
    };

    protected:
    v_state_ v_state_first_;
    ::xsde::cxx::stack v_state_stack_;

    virtual void
    _pre_e_validate ();

    virtual void
    _post_e_validate ();

    void
    sequence_0 (unsigned long&,
                unsigned long&,
                const ::xsde::cxx::ro_string&,
                const ::xsde::cxx::ro_string&,
                bool);
  };
}

#include "library-pskel.ixx"

// Begin epilogue.
//
//
// End epilogue.

#include <xsde/cxx/post.hxx>

#endif // LIBRARY_PSKEL_HXX
