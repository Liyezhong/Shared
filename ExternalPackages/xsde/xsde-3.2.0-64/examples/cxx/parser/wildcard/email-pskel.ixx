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

namespace email
{
  // binary_pskel
  //

  inline
  void binary_pskel::
  name_parser (::xml_schema::string_pskel& p)
  {
    this->name_parser_ = &p;
  }

  inline
  void binary_pskel::
  mime_parser (::xml_schema::string_pskel& p)
  {
    this->mime_parser_ = &p;
  }

  inline
  void binary_pskel::
  parsers (::xml_schema::string_pskel& name,
           ::xml_schema::string_pskel& mime)
  {
    this->name_parser_ = &name;
    this->mime_parser_ = &mime;
  }

  inline
  binary_pskel::
  binary_pskel (::xml_schema::base64_binary_pskel* tiein)
  : ::xml_schema::base64_binary_pskel (tiein, 0),
    binary_impl_ (0),
    name_parser_ (0),
    mime_parser_ (0),
    v_state_attr_stack_ (sizeof (v_state_attr_), &v_state_attr_first_)
  {
  }

  inline
  binary_pskel::
  binary_pskel (binary_pskel* impl, void*)
  : ::xml_schema::base64_binary_pskel (impl, 0),
    binary_impl_ (impl),
    name_parser_ (0),
    mime_parser_ (0),
    v_state_attr_stack_ (sizeof (v_state_attr_), &v_state_attr_first_)
  {
  }

  // envelope_pskel
  //

  inline
  void envelope_pskel::
  to_parser (::xml_schema::string_pskel& p)
  {
    this->to_parser_ = &p;
  }

  inline
  void envelope_pskel::
  from_parser (::xml_schema::string_pskel& p)
  {
    this->from_parser_ = &p;
  }

  inline
  void envelope_pskel::
  subject_parser (::xml_schema::string_pskel& p)
  {
    this->subject_parser_ = &p;
  }

  inline
  void envelope_pskel::
  parsers (::xml_schema::string_pskel& to,
           ::xml_schema::string_pskel& from,
           ::xml_schema::string_pskel& subject)
  {
    this->to_parser_ = &to;
    this->from_parser_ = &from;
    this->subject_parser_ = &subject;
  }

  inline
  envelope_pskel::
  envelope_pskel ()
  : envelope_impl_ (0),
    to_parser_ (0),
    from_parser_ (0),
    subject_parser_ (0),
    v_state_stack_ (sizeof (v_state_), &v_state_first_)
  {
  }

  inline
  envelope_pskel::
  envelope_pskel (envelope_pskel* impl, void*)
  : ::xsde::cxx::parser::validating::complex_content (impl, 0),
    envelope_impl_ (impl),
    to_parser_ (0),
    from_parser_ (0),
    subject_parser_ (0),
    v_state_stack_ (sizeof (v_state_), &v_state_first_)
  {
  }
}

// Begin epilogue.
//
//
// End epilogue.
