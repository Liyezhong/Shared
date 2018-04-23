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

// gender_pskel
//

inline
gender_pskel::
gender_pskel (::xml_schema::string_pskel* tiein)
: ::xml_schema::string_pskel (tiein, 0),
  gender_impl_ (0)
{
  this->_enumeration_facet (_xsde_gender_pskel_enums_, 2UL);
}

inline
gender_pskel::
gender_pskel (gender_pskel* impl, void*)
: ::xml_schema::string_pskel (impl, 0),
  gender_impl_ (impl)
{
  this->_enumeration_facet (_xsde_gender_pskel_enums_, 2UL);
}

// person_pskel
//

inline
void person_pskel::
first_name_parser (::xml_schema::string_pskel& p)
{
  this->first_name_parser_ = &p;
}

inline
void person_pskel::
last_name_parser (::xml_schema::string_pskel& p)
{
  this->last_name_parser_ = &p;
}

inline
void person_pskel::
gender_parser (::gender_pskel& p)
{
  this->gender_parser_ = &p;
}

inline
void person_pskel::
age_parser (::xml_schema::unsigned_short_pskel& p)
{
  this->age_parser_ = &p;
}

inline
void person_pskel::
parsers (::xml_schema::string_pskel& first_name,
         ::xml_schema::string_pskel& last_name,
         ::gender_pskel& gender,
         ::xml_schema::unsigned_short_pskel& age)
{
  this->first_name_parser_ = &first_name;
  this->last_name_parser_ = &last_name;
  this->gender_parser_ = &gender;
  this->age_parser_ = &age;
}

inline
person_pskel::
person_pskel ()
: person_impl_ (0),
  first_name_parser_ (0),
  last_name_parser_ (0),
  gender_parser_ (0),
  age_parser_ (0),
  v_state_stack_ (sizeof (v_state_), &v_state_first_)
{
}

inline
person_pskel::
person_pskel (person_pskel* impl, void*)
: ::xsde::cxx::parser::validating::complex_content (impl, 0),
  person_impl_ (impl),
  first_name_parser_ (0),
  last_name_parser_ (0),
  gender_parser_ (0),
  age_parser_ (0),
  v_state_stack_ (sizeof (v_state_), &v_state_first_)
{
}

// people_pskel
//

inline
void people_pskel::
person_parser (::person_pskel& p)
{
  this->person_parser_ = &p;
}

inline
void people_pskel::
parsers (::person_pskel& person)
{
  this->person_parser_ = &person;
}

inline
people_pskel::
people_pskel ()
: people_impl_ (0),
  person_parser_ (0),
  v_state_stack_ (sizeof (v_state_), &v_state_first_)
{
}

inline
people_pskel::
people_pskel (people_pskel* impl, void*)
: ::xsde::cxx::parser::validating::complex_content (impl, 0),
  people_impl_ (impl),
  person_parser_ (0),
  v_state_stack_ (sizeof (v_state_), &v_state_first_)
{
}

// Begin epilogue.
//
//
// End epilogue.
