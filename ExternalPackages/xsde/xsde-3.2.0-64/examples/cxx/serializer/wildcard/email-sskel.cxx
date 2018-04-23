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

#include "email-sskel.hxx"

#include <assert.h>

namespace email
{
  // binary_sskel
  //

  void binary_sskel::
  _reset ()
  {
    typedef ::xml_schema::base64_binary_sskel base;
    base::_reset ();

    if (this->name_serializer_)
      this->name_serializer_->_reset ();

    if (this->mime_serializer_)
      this->mime_serializer_->_reset ();
  }

  void binary_sskel::
  pre (const ::xml_schema::buffer* x)
  {
    assert (this->base64_binary_impl_);
    this->base64_binary_impl_->pre (x);
  }

  // envelope_sskel
  //

  bool envelope_sskel::
  any_attribute_next ()
  {
    return this->envelope_impl_ ? this->envelope_impl_->any_attribute_next () : false;
  }

  void envelope_sskel::
  any_attribute (::std::string& ns, ::std::string& n)
  {
    if (this->envelope_impl_)
      this->envelope_impl_->any_attribute (ns, n);
  }

  void envelope_sskel::
  serialize_any_attribute ()
  {
    if (this->envelope_impl_)
      this->envelope_impl_->serialize_any_attribute ();
  }

  void envelope_sskel::
  _reset ()
  {
    if (this->resetting_)
      return;

    typedef ::xsde::cxx::serializer::validating::complex_content base;
    base::_reset ();

    this->resetting_ = true;

    if (this->to_serializer_)
      this->to_serializer_->_reset ();

    if (this->from_serializer_)
      this->from_serializer_->_reset ();

    if (this->subject_serializer_)
      this->subject_serializer_->_reset ();

    this->resetting_ = false;
  }
}

namespace email
{
  // Element validation and serialization for envelope_sskel.
  //
  void envelope_sskel::
  _serialize_content ()
  {
    ::xsde::cxx::serializer::context& ctx = this->_context ();

    // to
    //
    {
      const ::std::string& r = this->to ();

      if (this->to_serializer_)
      {
        this->to_serializer_->pre (r);
        this->_start_element ("to");
        this->to_serializer_->_pre_impl (ctx);

        if (ctx.error_type ())
          return;

        this->to_serializer_->_serialize_attributes ();

        if (ctx.error_type ())
          return;

        this->to_serializer_->_serialize_content ();

        if (ctx.error_type ())
          return;

        this->to_serializer_->_post_impl ();

        if (ctx.error_type ())
          return;

        this->_end_element ();
        this->to_serializer_->post ();
      }
      else
      {
        this->_schema_error (::xsde::cxx::schema_error::expected_element);
        return;
      }
    }

    // from
    //
    {
      const ::std::string& r = this->from ();

      if (this->from_serializer_)
      {
        this->from_serializer_->pre (r);
        this->_start_element ("from");
        this->from_serializer_->_pre_impl (ctx);

        if (ctx.error_type ())
          return;

        this->from_serializer_->_serialize_attributes ();

        if (ctx.error_type ())
          return;

        this->from_serializer_->_serialize_content ();

        if (ctx.error_type ())
          return;

        this->from_serializer_->_post_impl ();

        if (ctx.error_type ())
          return;

        this->_end_element ();
        this->from_serializer_->post ();
      }
      else
      {
        this->_schema_error (::xsde::cxx::schema_error::expected_element);
        return;
      }
    }

    // subject
    //
    {
      const ::std::string& r = this->subject ();

      if (this->subject_serializer_)
      {
        this->subject_serializer_->pre (r);
        this->_start_element ("subject");
        this->subject_serializer_->_pre_impl (ctx);

        if (ctx.error_type ())
          return;

        this->subject_serializer_->_serialize_attributes ();

        if (ctx.error_type ())
          return;

        this->subject_serializer_->_serialize_content ();

        if (ctx.error_type ())
          return;

        this->subject_serializer_->_post_impl ();

        if (ctx.error_type ())
          return;

        this->_end_element ();
        this->subject_serializer_->post ();
      }
      else
      {
        this->_schema_error (::xsde::cxx::schema_error::expected_element);
        return;
      }
    }

    {
      size_t i = 0;
      for (; this->any_next (); ++i)
      {
        ::std::string ns, name;
        this->any (ns, name);

        if (ns == "http://www.codesynthesis.com/email")
        {
          if (ns.empty ())
          {
            this->_start_element (name.c_str ());
          }
          else
          {
            this->_start_element (ns.c_str (), name.c_str ());
          }

          this->serialize_any ();

          if (ctx.error_type ())
            return;

          this->_end_element ();
        }
        else
        {
          this->_schema_error (::xsde::cxx::schema_error::unexpected_element);
          return;
        }
      }

      if (i < 1UL)
      {
        this->_schema_error (::xsde::cxx::schema_error::expected_element);
        return;
      }
    }
  }
}

namespace email
{
  // Attribute validation and serialization for binary_sskel.
  //
  void binary_sskel::
  _serialize_attributes ()
  {
    ::xsde::cxx::serializer::context& ctx = this->_context ();

    typedef ::xml_schema::base64_binary_sskel base;
    base::_serialize_attributes ();

    if (ctx.error_type ())
      return;

    // name
    //
    {
      const ::std::string& r = this->name ();

      if (this->name_serializer_)
      {
        this->name_serializer_->pre (r);
        this->_start_attribute ("name");
        this->name_serializer_->_pre_impl (ctx);

        if (ctx.error_type ())
          return;

        this->name_serializer_->_serialize_content ();

        if (ctx.error_type ())
          return;

        this->name_serializer_->_post_impl ();

        if (ctx.error_type ())
          return;

        this->_end_attribute ();
        this->name_serializer_->post ();
      }
      else
      {
        this->_schema_error (::xsde::cxx::schema_error::expected_attribute);
        return;
      }
    }

    // mime
    //
    {
      const ::std::string& r = this->mime ();

      if (this->mime_serializer_)
      {
        this->mime_serializer_->pre (r);
        this->_start_attribute ("mime");
        this->mime_serializer_->_pre_impl (ctx);

        if (ctx.error_type ())
          return;

        this->mime_serializer_->_serialize_content ();

        if (ctx.error_type ())
          return;

        this->mime_serializer_->_post_impl ();

        if (ctx.error_type ())
          return;

        this->_end_attribute ();
        this->mime_serializer_->post ();
      }
      else
      {
        this->_schema_error (::xsde::cxx::schema_error::expected_attribute);
        return;
      }
    }
  }

  // Attribute validation and serialization for envelope_sskel.
  //
  void envelope_sskel::
  _serialize_attributes ()
  {
    ::xsde::cxx::serializer::context& ctx = this->_context ();

    while (this->any_attribute_next ())
    {
      ::std::string ns, name;
      this->any_attribute (ns, name);

      if (ns == "http://www.codesynthesis.com/email")
      {
        if (ns.empty ())
        {
          this->_start_attribute (name.c_str ());
        }
        else
        {
          this->_start_attribute (ns.c_str (), name.c_str ());
        }

        this->serialize_any_attribute ();

        if (ctx.error_type ())
          return;

        this->_end_attribute ();
      }
      else
      {
        this->_schema_error (::xsde::cxx::schema_error::unexpected_attribute);
        return;
      }
    }
  }
}

// Begin epilogue.
//
//
// End epilogue.
