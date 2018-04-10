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

// position_sskel
//

inline
void position_sskel::
lat_serializer (::xml_schema::float_sskel& lat)
{
  this->lat_serializer_ = &lat;
}

inline
void position_sskel::
lon_serializer (::xml_schema::float_sskel& lon)
{
  this->lon_serializer_ = &lon;
}

inline
void position_sskel::
serializers (::xml_schema::float_sskel& lat,
             ::xml_schema::float_sskel& lon)
{
  this->lat_serializer_ = &lat;
  this->lon_serializer_ = &lon;
}

inline
position_sskel::
position_sskel ()
: position_impl_ (0),
  lat_serializer_ (0),
  lon_serializer_ (0)
{
}

inline
position_sskel::
position_sskel (position_sskel* impl, void*)
: ::xsde::cxx::serializer::validating::complex_content (impl, 0),
  position_impl_ (impl),
  lat_serializer_ (0),
  lon_serializer_ (0)
{
}

// object_sskel
//

inline
void object_sskel::
id_serializer (::xml_schema::unsigned_int_sskel& id)
{
  this->id_serializer_ = &id;
}

inline
void object_sskel::
position_serializer (::position_sskel& s)
{
  this->position_serializer_ = &s;
}

inline
void object_sskel::
serializers (::xml_schema::unsigned_int_sskel& id,
             ::position_sskel& position)
{
  this->id_serializer_ = &id;
  this->position_serializer_ = &position;
}

inline
object_sskel::
object_sskel ()
: object_impl_ (0),
  id_serializer_ (0),
  position_serializer_ (0)
{
}

inline
object_sskel::
object_sskel (object_sskel* impl, void*)
: ::xsde::cxx::serializer::validating::complex_content (impl, 0),
  object_impl_ (impl),
  id_serializer_ (0),
  position_serializer_ (0)
{
}

// Begin epilogue.
//
//
// End epilogue.

