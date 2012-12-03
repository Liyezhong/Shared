/****************************************************************************/
/*! \file DerivedRxCommand.h
 *
 *  \brief DerivedRxCommand command definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 11.04.2011
 *   $Author:  $ Y.Novak
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 */
/****************************************************************************/
#ifndef NETWORKBASE_DERIVEDRXCOMMAND_H
#define NETWORKBASE_DERIVEDRXCOMMAND_H

#include <Global/Include/AbstractFactory.h>
#include <NetworkComponents/Include/ProtocolRxCommand.h>

namespace NetworkBase {

/// XML attributes processed by this command:
const QStringList DerivedRxCommandAttr = QStringList() << "att1" << "att2" << "att3";  ///< test command attributes
const QStringList DerivedRxCommandAttr1 = QStringList() << "att1" << "att2" << "att3" << "nofitems";  ///< test command attributes
const QStringList DerivedRxCommandDataAttr = QStringList() << "satt1" << "satt2" << "satt3" << "satt4"; ///< test command attributes


/****************************************************************************/
/**
 * \brief ProtocolRxCommand is virtual. This is its test realization 1.
 */
/****************************************************************************/
class DerivedRxCommand : public ProtocolRxCommand
{
    Q_OBJECT

private:
    DECLARE_REG_CLASS(ProtocolRxCommand, DerivedRxCommand);   ///< Declaration of static registration instance.

public:
    /// constructor
    DerivedRxCommand() :
                    ProtocolRxCommand() {}
    /// constructor
    ///  \param[in] attr = list of data attributes of this command
    DerivedRxCommand(const QStringList &attr) :
                    ProtocolRxCommand(attr) {}
    /// constructor
    ///  \param[in] attr = list of data attributes of this command
    ///  \param[in] dattr = list of data attributes of each data element of this command
    DerivedRxCommand(const QStringList &attr, const QStringList &dattr) :
                    ProtocolRxCommand(attr, dattr) {}
    /// destructor
    virtual ~DerivedRxCommand() {}
    bool Execute();

}; // command
/****************************************************************************/
/**
 * \brief ProtocolRxCommand is virtual. This is its test realization 2.
 */
/****************************************************************************/
class DerivedRxCommandOne : public DerivedRxCommand
{
    Q_OBJECT

private:
    DECLARE_REG_CLASS(ProtocolRxCommand, DerivedRxCommandOne);   ///< Declaration of static registration instance.

public:
    /// constructor
    DerivedRxCommandOne() :
                    DerivedRxCommand::DerivedRxCommand(DerivedRxCommandAttr) {}
    /// destructor
    ~DerivedRxCommandOne() {}
}; // command
/****************************************************************************/
/**
 * \brief ProtocolRxCommand is virtual. This is its test realization 3.
 */
/****************************************************************************/
class DerivedRxCommandTwo : public DerivedRxCommand
{
    Q_OBJECT

private:
    DECLARE_REG_CLASS(ProtocolRxCommand, DerivedRxCommandTwo);   ///< Declaration of static registration instance.

public:
    /// constructor
    DerivedRxCommandTwo() :
                    DerivedRxCommand::DerivedRxCommand(DerivedRxCommandAttr1, DerivedRxCommandDataAttr) {}
    /// destructor
    ~DerivedRxCommandTwo() {}
}; // command
/****************************************************************************/
} // namespace

#endif
