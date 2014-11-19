/****************************************************************************/
/*! \file Global/Include/Commands/CmdDataChanged.h
 *
 *  \brief Definition file for class CmdDataChanged.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-03-29
 *  $Author:    $ J.Bugariu
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

//lint -esym(1511, QWidget::show)
//  No polymorphic behaviour desired at this point

#ifndef GLOBAL_CMDDATACHANGED_H
#define GLOBAL_CMDDATACHANGED_H

#include <Global/Include/Commands/Command.h>

namespace Global {

/****************************************************************************/
/**
 * \brief Class for telling that data in the data manager has changed.
 *
 * \warning This class is not thread safe!
 */
/****************************************************************************/
class CmdDataChanged : public Global::Command {
    friend class TestCommands;
private:
    /****************************************************************************/
    CmdDataChanged();       ///< Not implemented.
    /****************************************************************************/
    /**
     * \brief Copy constructor.
     *
     * \param[in]   rOther  Instance to copy from.
     */
    /****************************************************************************/
    void CopyFrom(const CmdDataChanged &rOther);
protected:
public:
    static QString  NAME;                   ///< Command name.
    QString         m_DataContainerName;    ///< Name of changed data container.
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   DataContainerName   Name of data container which changed.
     */
    /****************************************************************************/
    CmdDataChanged(const QString &DataContainerName);
    /****************************************************************************/
    /**
     * \brief Copy constructor.
     *
     * \param[in]   rOther  Instance to copy from.
     */
    /****************************************************************************/
    CmdDataChanged(const CmdDataChanged &rOther);
    /****************************************************************************/
    /**
     * \brief Assignment operator.
     *
     * \param[in]   rOther  Instance to copy from.
     * \return              Const reference to self.
     */
    /****************************************************************************/
    const CmdDataChanged & operator = (const CmdDataChanged &rOther);
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdDataChanged();
    /****************************************************************************/
    /**
     * \brief Get command name.
     *
     * \return  Command name.
     */
    /****************************************************************************/
    virtual QString GetName() const {
        return NAME;
    }
    /****************************************************************************/
    /**
     * \brief Get data container name.
     *
     * \return  Data container name.
     */
    /****************************************************************************/
    QString GetDataContainerName() const {
        return m_DataContainerName;
    }
}; // end class CmdDataChanged

/****************************************************************************/
/**
 * \brief Base functor class for DataChanged command.
 *
 * This kind of functor is used to "link" specific DataChanged commands to
 * the according processing routines.
 * \warning We use explicit calls to \ref Process not overloading () operator.
 */
/****************************************************************************/
class CmdDataChangedFunctor {
private:
    /****************************************************************************/
    CmdDataChangedFunctor(const CmdDataChangedFunctor &);                       ///< Not implemented.
    const CmdDataChangedFunctor & operator = (const CmdDataChangedFunctor &);   ///< Not implemented.
protected:
public:
    /****************************************************************************/
    /**
     * \brief Default constructor.
     */
    /****************************************************************************/
    inline CmdDataChangedFunctor() {
    }
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CmdDataChangedFunctor() {
    }
    /****************************************************************************/
    /**
     * \brief Process the desired acknowledge.
     */
    /****************************************************************************/
    virtual void Process() = 0;
}; // end class CmdDataChangedFunctor

/****************************************************************************/
/**
 * \brief Template functor class for DataChanged command.
 *
 * This kind of functor is used to "link" specific DataChanged commands to
 * the according processing routines.
 * \warning We use explicit calls to \ref Process not overloading () operator.
 */
/****************************************************************************/
template<class TDataChangedProcessorClass> class TCDataChangedProcessFunctor : public CmdDataChangedFunctor {
private:
    TDataChangedProcessorClass          *m_pDataChangedProcessor;       ///< Pointer to acknowledge processor instance.
    void (TDataChangedProcessorClass::  *m_FunctionPointer)();          ///< Pointer to member function.
    /****************************************************************************/
    TCDataChangedProcessFunctor();                                                          ///< Not implemented.
    TCDataChangedProcessFunctor(const TCDataChangedProcessFunctor &);                       ///< Not implemented.
    const TCDataChangedProcessFunctor & operator = (const TCDataChangedProcessFunctor &);   ///< Not implemented.
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \param[in]   pDataChangedProcessor   Pointer to DataChanged processor instance to which the function pointer belongs.
     * \param[in]   FunctionPointer         The function pointer.
     */
    /****************************************************************************/
    TCDataChangedProcessFunctor(TDataChangedProcessorClass *pDataChangedProcessor,
                                void(TDataChangedProcessorClass::*FunctionPointer)()) :
        m_pDataChangedProcessor(pDataChangedProcessor),
        m_FunctionPointer(FunctionPointer)
    {
    }
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~TCDataChangedProcessFunctor() {
    }
    /****************************************************************************/
    /**
     * \brief Process the desired acknowledge.
     */
    /****************************************************************************/
    virtual void Process() {
        // process DataChanged
        (*m_pDataChangedProcessor.*m_FunctionPointer)();
    }
}; // end class TCDataChangedProcessFunctor


} // end namespace Global

#endif // GLOBAL_CMDDATACHANGED_H
