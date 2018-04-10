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

#ifndef PROTOCOL_PIMPL_HXX
#define PROTOCOL_PIMPL_HXX

#include <xsde/cxx/pre.hxx>

// Begin prologue.
//
//
// End prologue.

#ifndef XSDE_OMIT_PAGGR
#  define XSDE_OMIT_PAGGR
#  define PROTOCOL_PIMPL_HXX_CLEAR_OMIT_PAGGR
#endif

#include "protocol-pskel.hxx"

#include <xsde/cxx/stack.hxx>

namespace protocol
{
  class request_pimpl: public request_pskel
  {
    public:
    virtual void
    pre ();

    // Elements.
    //
    virtual void
    account (unsigned int);

    virtual ::protocol::request
    post_request ();

    public:
    struct request_pimpl_state
    {
      ::protocol::request request_;
    };

    request_pimpl_state request_pimpl_state_;
  };

  class balance_pimpl: public balance_pskel
  {
    public:
    balance_pimpl ();

    virtual void
    pre ();

    virtual ::protocol::balance
    post_balance ();

    public:
    ::protocol::request_pimpl base_impl_;

    public:
    struct balance_pimpl_state
    {
      ::protocol::balance balance_;
    };

    balance_pimpl_state balance_pimpl_state_;
  };

  class withdraw_pimpl: public withdraw_pskel
  {
    public:
    withdraw_pimpl ();

    virtual void
    pre ();

    // Elements.
    //
    virtual void
    amount (unsigned int);

    virtual ::protocol::withdraw
    post_withdraw ();

    public:
    ::protocol::request_pimpl base_impl_;

    public:
    struct withdraw_pimpl_state
    {
      ::protocol::withdraw withdraw_;
    };

    withdraw_pimpl_state withdraw_pimpl_state_;
  };

  class deposit_pimpl: public deposit_pskel
  {
    public:
    deposit_pimpl ();

    virtual void
    pre ();

    // Elements.
    //
    virtual void
    amount (unsigned int);

    virtual ::protocol::deposit
    post_deposit ();

    public:
    ::protocol::request_pimpl base_impl_;

    public:
    struct deposit_pimpl_state
    {
      ::protocol::deposit deposit_;
    };

    deposit_pimpl_state deposit_pimpl_state_;
  };
}

#ifdef PROTOCOL_PIMPL_HXX_CLEAR_OMIT_PAGGR
#  undef XSDE_OMIT_PAGGR
#endif

#ifndef XSDE_OMIT_PAGGR

namespace protocol
{
  // Parser aggregate for the balance element.
  //
  class balance_paggr
  {
    public:
    balance_paggr ();

    void
    pre ()
    {
      this->balance_p_.pre ();
    }

    ::protocol::balance
    post ()
    {
      return this->balance_p_.post_balance ();
    }

    ::protocol::balance_pimpl&
    root_parser ()
    {
      return this->balance_p_;
    }

    static const char*
    root_name ();

    static const char*
    root_namespace ();

    void
    reset ()
    {
      this->balance_p_._reset ();
    }

    public:
    ::xml_schema::unsigned_int_pimpl unsigned_int_p_;
    ::protocol::balance_pimpl balance_p_;
  };

  // Parser aggregate for the withdraw element.
  //
  class withdraw_paggr
  {
    public:
    withdraw_paggr ();

    void
    pre ()
    {
      this->withdraw_p_.pre ();
    }

    ::protocol::withdraw
    post ()
    {
      return this->withdraw_p_.post_withdraw ();
    }

    ::protocol::withdraw_pimpl&
    root_parser ()
    {
      return this->withdraw_p_;
    }

    static const char*
    root_name ();

    static const char*
    root_namespace ();

    void
    reset ()
    {
      this->withdraw_p_._reset ();
    }

    public:
    ::xml_schema::unsigned_int_pimpl unsigned_int_p_;
    ::protocol::withdraw_pimpl withdraw_p_;
  };

  // Parser aggregate for the deposit element.
  //
  class deposit_paggr
  {
    public:
    deposit_paggr ();

    void
    pre ()
    {
      this->deposit_p_.pre ();
    }

    ::protocol::deposit
    post ()
    {
      return this->deposit_p_.post_deposit ();
    }

    ::protocol::deposit_pimpl&
    root_parser ()
    {
      return this->deposit_p_;
    }

    static const char*
    root_name ();

    static const char*
    root_namespace ();

    void
    reset ()
    {
      this->deposit_p_._reset ();
    }

    public:
    ::xml_schema::unsigned_int_pimpl unsigned_int_p_;
    ::protocol::deposit_pimpl deposit_p_;
  };
}

#endif // XSDE_OMIT_PAGGR

// Begin epilogue.
//
//
// End epilogue.

#include <xsde/cxx/post.hxx>

#endif // PROTOCOL_PIMPL_HXX
