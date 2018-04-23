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

#ifndef EMAIL_PIMPL_HXX
#define EMAIL_PIMPL_HXX

#include <xsde/cxx/pre.hxx>

// Begin prologue.
//
//
// End prologue.

#ifndef XSDE_OMIT_PAGGR
#  define XSDE_OMIT_PAGGR
#  define EMAIL_PIMPL_HXX_CLEAR_OMIT_PAGGR
#endif

#include "email-pskel.hxx"

#include <xsde/cxx/stack.hxx>

namespace email
{
  class binary_pimpl: public binary_pskel
  {
    public:
    binary_pimpl (bool = false);

    ~binary_pimpl ();

    virtual void
    _reset ();

    virtual void
    pre ();

    // Attributes.
    //
    virtual void
    name (const ::std::string&);

    virtual void
    mime (const ::std::string&);

    virtual ::email::binary*
    post_binary ();

    public:
    void
    pre_impl (::email::binary*);

    public:
    ::xml_schema::base64_binary_pimpl base_impl_;

    public:
    struct binary_pimpl_state
    {
      ::email::binary* binary_;
    };

    binary_pimpl_state binary_pimpl_state_;
    bool binary_pimpl_base_;
  };

  class envelope_base_pimpl: public envelope_pskel
  {
    public:
    envelope_base_pimpl (bool = false);

    ~envelope_base_pimpl ();

    virtual void
    _reset ();

    virtual void
    pre ();

    // Elements.
    //
    virtual void
    to (const ::std::string&);

    virtual void
    from (const ::std::string&);

    virtual void
    subject (const ::std::string&);

    virtual ::email::envelope*
    post_envelope ();

    public:
    void
    pre_impl (::email::envelope*);

    public:
    struct envelope_base_pimpl_state
    {
      ::email::envelope* envelope_;
    };

    envelope_base_pimpl_state envelope_base_pimpl_state_;
    bool envelope_base_pimpl_base_;
  };
}

#include "envelope-pimpl.hxx"

namespace email
{
}

#ifdef EMAIL_PIMPL_HXX_CLEAR_OMIT_PAGGR
#  undef XSDE_OMIT_PAGGR
#endif

#ifndef XSDE_OMIT_PAGGR

namespace email
{
  // Parser aggregate for the text element.
  //
  class text_paggr
  {
    public:
    text_paggr ();

    void
    pre ()
    {
      this->string_p_.pre ();
    }

    ::std::string
    post ()
    {
      return this->string_p_.post_string ();
    }

    ::xml_schema::string_pimpl&
    root_parser ()
    {
      return this->string_p_;
    }

    static const char*
    root_name ();

    static const char*
    root_namespace ();

    void
    reset ()
    {
      this->string_p_._reset ();
    }

    public:
    ::xml_schema::string_pimpl string_p_;
  };

  // Parser aggregate for the binary element.
  //
  class binary_paggr
  {
    public:
    binary_paggr ();

    void
    pre ()
    {
      this->binary_p_.pre ();
    }

    ::email::binary*
    post ()
    {
      return this->binary_p_.post_binary ();
    }

    ::email::binary_pimpl&
    root_parser ()
    {
      return this->binary_p_;
    }

    static const char*
    root_name ();

    static const char*
    root_namespace ();

    void
    reset ()
    {
      this->binary_p_._reset ();
    }

    public:
    ::xml_schema::string_pimpl string_p_;
    ::email::binary_pimpl binary_p_;
  };

  // Parser aggregate for the message element.
  //
  class message_paggr
  {
    public:
    message_paggr ();

    void
    pre ()
    {
      this->envelope_p_.pre ();
    }

    ::email::envelope*
    post ()
    {
      return this->envelope_p_.post_envelope ();
    }

    ::email::envelope_pimpl&
    root_parser ()
    {
      return this->envelope_p_;
    }

    static const char*
    root_name ();

    static const char*
    root_namespace ();

    void
    reset ()
    {
      this->envelope_p_._reset ();
    }

    public:
    ::xml_schema::string_pimpl string_p_;
    ::email::envelope_pimpl envelope_p_;
  };
}

#endif // XSDE_OMIT_PAGGR

// Begin epilogue.
//
//
// End epilogue.

#include <xsde/cxx/post.hxx>

#endif // EMAIL_PIMPL_HXX