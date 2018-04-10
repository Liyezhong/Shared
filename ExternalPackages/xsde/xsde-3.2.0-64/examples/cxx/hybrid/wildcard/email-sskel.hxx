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

#ifndef EMAIL_SSKEL_HXX
#define EMAIL_SSKEL_HXX

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

#ifndef XSDE_SERIALIZER_VALIDATION
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
#include <xsde/cxx/hybrid/any-type-sskel.hxx>
#include <xsde/cxx/hybrid/any-type-simpl.hxx>

namespace xml_schema
{
  using ::xsde::cxx::hybrid::any_type;

  using ::xsde::cxx::hybrid::any_type_sskel;
  using ::xsde::cxx::hybrid::any_type_simpl;
}


//
// End prologue.

// Forward declarations
//
namespace email
{
  class binary_sskel;
  class envelope_sskel;
}


#include <string>

#include <xsde/cxx/serializer/xml-schema.hxx>

#include <xsde/cxx/serializer/exceptions.hxx>

#include <xsde/cxx/serializer/validating/serializer.hxx>
#include <xsde/cxx/serializer/validating/xml-schema-sskel.hxx>
#include <xsde/cxx/serializer/validating/xml-schema-simpl.hxx>

#include <xsde/cxx/serializer/genx/document.hxx>

#include "email.hxx"

namespace xml_schema
{
  // Built-in XML Schema types mapping.
  //
  using ::xsde::cxx::string_sequence;
  using ::xsde::cxx::qname;
  using ::xsde::cxx::buffer;
  using ::xsde::cxx::time_zone;
  using ::xsde::cxx::gday;
  using ::xsde::cxx::gmonth;
  using ::xsde::cxx::gyear;
  using ::xsde::cxx::gmonth_day;
  using ::xsde::cxx::gyear_month;
  using ::xsde::cxx::date;
  using ::xsde::cxx::time;
  using ::xsde::cxx::date_time;
  using ::xsde::cxx::duration;

  // Base serializer skeletons.
  //
  using ::xsde::cxx::serializer::serializer_base;
  typedef ::xsde::cxx::serializer::validating::empty_content serializer_empty_content;
  typedef ::xsde::cxx::serializer::validating::simple_content serializer_simple_content;
  typedef ::xsde::cxx::serializer::validating::complex_content serializer_complex_content;

  // Serializer skeletons and implementations for the
  // XML Schema built-in types.
  //
  using ::xsde::cxx::serializer::validating::any_simple_type_sskel;
  using ::xsde::cxx::serializer::validating::any_simple_type_simpl;

  using ::xsde::cxx::serializer::validating::byte_sskel;
  using ::xsde::cxx::serializer::validating::byte_simpl;

  using ::xsde::cxx::serializer::validating::unsigned_byte_sskel;
  using ::xsde::cxx::serializer::validating::unsigned_byte_simpl;

  using ::xsde::cxx::serializer::validating::short_sskel;
  using ::xsde::cxx::serializer::validating::short_simpl;

  using ::xsde::cxx::serializer::validating::unsigned_short_sskel;
  using ::xsde::cxx::serializer::validating::unsigned_short_simpl;

  using ::xsde::cxx::serializer::validating::int_sskel;
  using ::xsde::cxx::serializer::validating::int_simpl;

  using ::xsde::cxx::serializer::validating::unsigned_int_sskel;
  using ::xsde::cxx::serializer::validating::unsigned_int_simpl;

  using ::xsde::cxx::serializer::validating::long_sskel;
  using ::xsde::cxx::serializer::validating::long_simpl;

  using ::xsde::cxx::serializer::validating::unsigned_long_sskel;
  using ::xsde::cxx::serializer::validating::unsigned_long_simpl;

  using ::xsde::cxx::serializer::validating::integer_sskel;
  using ::xsde::cxx::serializer::validating::integer_simpl;

  using ::xsde::cxx::serializer::validating::non_positive_integer_sskel;
  using ::xsde::cxx::serializer::validating::non_positive_integer_simpl;

  using ::xsde::cxx::serializer::validating::non_negative_integer_sskel;
  using ::xsde::cxx::serializer::validating::non_negative_integer_simpl;

  using ::xsde::cxx::serializer::validating::positive_integer_sskel;
  using ::xsde::cxx::serializer::validating::positive_integer_simpl;

  using ::xsde::cxx::serializer::validating::negative_integer_sskel;
  using ::xsde::cxx::serializer::validating::negative_integer_simpl;

  using ::xsde::cxx::serializer::validating::boolean_sskel;
  using ::xsde::cxx::serializer::validating::boolean_simpl;

  using ::xsde::cxx::serializer::validating::float_sskel;
  using ::xsde::cxx::serializer::validating::float_simpl;

  using ::xsde::cxx::serializer::validating::double_sskel;
  using ::xsde::cxx::serializer::validating::double_simpl;

  using ::xsde::cxx::serializer::validating::decimal_sskel;
  using ::xsde::cxx::serializer::validating::decimal_simpl;

  using ::xsde::cxx::serializer::validating::string_sskel;
  using ::xsde::cxx::serializer::validating::string_simpl;

  using ::xsde::cxx::serializer::validating::normalized_string_sskel;
  using ::xsde::cxx::serializer::validating::normalized_string_simpl;

  using ::xsde::cxx::serializer::validating::token_sskel;
  using ::xsde::cxx::serializer::validating::token_simpl;

  using ::xsde::cxx::serializer::validating::name_sskel;
  using ::xsde::cxx::serializer::validating::name_simpl;

  using ::xsde::cxx::serializer::validating::nmtoken_sskel;
  using ::xsde::cxx::serializer::validating::nmtoken_simpl;

  using ::xsde::cxx::serializer::validating::nmtokens_sskel;
  using ::xsde::cxx::serializer::validating::nmtokens_simpl;

  using ::xsde::cxx::serializer::validating::ncname_sskel;
  using ::xsde::cxx::serializer::validating::ncname_simpl;

  using ::xsde::cxx::serializer::validating::language_sskel;
  using ::xsde::cxx::serializer::validating::language_simpl;

  using ::xsde::cxx::serializer::validating::id_sskel;
  using ::xsde::cxx::serializer::validating::id_simpl;

  using ::xsde::cxx::serializer::validating::idref_sskel;
  using ::xsde::cxx::serializer::validating::idref_simpl;

  using ::xsde::cxx::serializer::validating::idrefs_sskel;
  using ::xsde::cxx::serializer::validating::idrefs_simpl;

  using ::xsde::cxx::serializer::validating::uri_sskel;
  using ::xsde::cxx::serializer::validating::uri_simpl;

  using ::xsde::cxx::serializer::validating::qname_sskel;
  using ::xsde::cxx::serializer::validating::qname_simpl;

  using ::xsde::cxx::serializer::validating::base64_binary_sskel;
  using ::xsde::cxx::serializer::validating::base64_binary_simpl;

  using ::xsde::cxx::serializer::validating::hex_binary_sskel;
  using ::xsde::cxx::serializer::validating::hex_binary_simpl;

  using ::xsde::cxx::serializer::validating::date_sskel;
  using ::xsde::cxx::serializer::validating::date_simpl;

  using ::xsde::cxx::serializer::validating::date_time_sskel;
  using ::xsde::cxx::serializer::validating::date_time_simpl;

  using ::xsde::cxx::serializer::validating::duration_sskel;
  using ::xsde::cxx::serializer::validating::duration_simpl;

  using ::xsde::cxx::serializer::validating::gday_sskel;
  using ::xsde::cxx::serializer::validating::gday_simpl;

  using ::xsde::cxx::serializer::validating::gmonth_sskel;
  using ::xsde::cxx::serializer::validating::gmonth_simpl;

  using ::xsde::cxx::serializer::validating::gmonth_day_sskel;
  using ::xsde::cxx::serializer::validating::gmonth_day_simpl;

  using ::xsde::cxx::serializer::validating::gyear_sskel;
  using ::xsde::cxx::serializer::validating::gyear_simpl;

  using ::xsde::cxx::serializer::validating::gyear_month_sskel;
  using ::xsde::cxx::serializer::validating::gyear_month_simpl;

  using ::xsde::cxx::serializer::validating::time_sskel;
  using ::xsde::cxx::serializer::validating::time_simpl;

  // Error codes.
  //
  typedef xsde::cxx::serializer::genx::xml_error serializer_xml_error;
  typedef xsde::cxx::schema_error serializer_schema_error;

  // Exceptions.
  //
  typedef xsde::cxx::serializer::exception serializer_exception;
  typedef xsde::cxx::serializer::xml serializer_xml;
  typedef xsde::cxx::serializer::schema serializer_schema;

  // Document serializer.
  //
  using xsde::cxx::serializer::genx::writer;
  using xsde::cxx::serializer::genx::document_simpl;

  // Serializer context.
  //
  typedef xsde::cxx::serializer::context serializer_context;
}

namespace email
{
  class binary_sskel: public ::xml_schema::base64_binary_sskel
  {
    public:
    // Serializer callbacks. Override them in your implementation.
    //

    virtual void
    pre (const ::email::binary&) = 0;

    // Attributes.
    //
    virtual ::std::string
    name () = 0;

    virtual ::std::string
    mime () = 0;

    // virtual void
    // post ();

    // Serializer construction API.
    //
    void
    serializers (::xml_schema::string_sskel& /* name */,
                 ::xml_schema::string_sskel& /* mime */);

    // Individual attribute serializers.
    //
    void
    name_serializer (::xml_schema::string_sskel&);

    void
    mime_serializer (::xml_schema::string_sskel&);

    virtual void
    _reset ();

    // Constructor.
    //
    binary_sskel (::xml_schema::base64_binary_sskel* tiein);

    // Implementation.
    //
    public:
    virtual void
    pre (const ::xml_schema::buffer*);

    virtual void
    _serialize_attributes ();

    protected:
    binary_sskel* binary_impl_;
    binary_sskel (binary_sskel*, void*);

    protected:
    ::xml_schema::string_sskel* name_serializer_;
    ::xml_schema::string_sskel* mime_serializer_;
  };

  class envelope_sskel: public ::xsde::cxx::serializer::validating::complex_content
  {
    public:
    // Serializer callbacks. Override them in your implementation.
    //

    virtual void
    pre (const ::email::envelope&) = 0;

    // Elements.
    //
    virtual ::std::string
    to () = 0;

    virtual ::std::string
    from () = 0;

    virtual ::std::string
    subject () = 0;

    virtual bool
    any_next () = 0;

    virtual void
    any (::std::string& ns, ::std::string& name) = 0;

    virtual void
    serialize_any () = 0;

    // virtual void
    // post ();

    // Serializer construction API.
    //
    void
    serializers (::xml_schema::string_sskel& /* to */,
                 ::xml_schema::string_sskel& /* from */,
                 ::xml_schema::string_sskel& /* subject */);

    // Individual element serializers.
    //
    void
    to_serializer (::xml_schema::string_sskel&);

    void
    from_serializer (::xml_schema::string_sskel&);

    void
    subject_serializer (::xml_schema::string_sskel&);

    virtual void
    _reset ();

    // Constructor.
    //
    envelope_sskel ();

    // Implementation.
    //
    public:
    virtual void
    _serialize_content ();

    protected:
    envelope_sskel* envelope_impl_;
    envelope_sskel (envelope_sskel*, void*);

    protected:
    ::xml_schema::string_sskel* to_serializer_;
    ::xml_schema::string_sskel* from_serializer_;
    ::xml_schema::string_sskel* subject_serializer_;
  };
}

#include "email-sskel.ixx"

// Begin epilogue.
//
//
// End epilogue.

#include <xsde/cxx/post.hxx>

#endif // EMAIL_SSKEL_HXX
