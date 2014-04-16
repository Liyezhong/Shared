/****************************************************************************/
/*! \file CreatorFunctor.h
 *
 *  \brief Definition file for functors needed by command classes.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-03-11
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

#ifndef NETWORKBASE_CREATORFUNCTOR_H
#define NETWORKBASE_CREATORFUNCTOR_H

#include <QDataStream>
#include <Global/Include/GlobalDefines.h>

namespace NetworkBase {

/****************************************************************************/
/**
 * \brief Base functor class for creating and deserializing commands and acknowledges.
 *
 * This kind of functor is used to "link" creation and deserialization to specific methods.
 * \warning We use explicit calls to \ref CreateAndDeserialize not overloading () operator.
 */
/****************************************************************************/
class CreatorFunctor {
private:
    /****************************************************************************/
    /*!
     *  \brief Disable copy and assignment operator.
     *
     */
    /****************************************************************************/
    Q_DISABLE_COPY(CreatorFunctor)
protected:
public:
    /****************************************************************************/
    /**
     * \brief Default constructor.
     */
    /****************************************************************************/
    inline CreatorFunctor() {
    }
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CreatorFunctor() {
    }
    /****************************************************************************/
    /**
     * \brief Create instance an deserialize it.
     *
     * \iparam       Ref         Command reference.
     * \iparam  DataStream  Data stream from which deserialization is done.
     */
    /****************************************************************************/
    virtual void CreateAndDeserialize(Global::tRefType Ref, QDataStream &DataStream) = 0;
}; // end class CreatorFunctor

/****************************************************************************/
/**
 * \brief Base functor class for creating and deserializing commands and acknowledges.
 *
 * This kind of functor is used to "link" creation and deserialization to specific methods.
 * This interface can be used, if the caller is aware of the command class type.
 * \warning We use explicit calls to \ref CreateAndDeserialize not overloading () operator.
 */
/****************************************************************************/
template<class TheClass> class CommandCreatorFunctor : public CreatorFunctor {
private:
    /****************************************************************************/
    CommandCreatorFunctor(const CommandCreatorFunctor &);                       ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const CommandCreatorFunctor & operator = (const CommandCreatorFunctor &);   ///< Not implemented.
protected:
public:
    /****************************************************************************/
    /**
     * \brief Default constructor.
     */
    /****************************************************************************/
    inline CommandCreatorFunctor() {
    }
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~CommandCreatorFunctor() {
    }
    /****************************************************************************/
    /**
     * \brief Create instance an deserialize it.
     *
     * \iparam       Ref         Command reference.
     * \iparam  DataStream  Data stream from which deserialization is done.
     */
    /****************************************************************************/
    virtual void CreateAndDeserialize(Global::tRefType Ref, QDataStream &DataStream) = 0;
    /****************************************************************************/
    /**
     * \brief Copies the reference of an instance and calls the handler.
     *
     * \warning You must ensure correct deletion of the returned pointer!
     *
     * \iparam       Ref         Command reference.
     * \iparam       Instance    Reference to the original instance.
     */
    /****************************************************************************/
    virtual void CopyAndCall(Global::tRefType Ref, const TheClass &Instance) = 0;
}; // end class CommandCreatorFunctor

/****************************************************************************/
/**
 * \brief Template functor class for specific creating and deserializing.
 *
 * This kind of functor is used to "link" creation and deserialization to specific methods.
 * \warning We use explicit calls to \ref CreateAndDeserialize not overloading () operator.
 */
/****************************************************************************/
template<class TheClass, class TheProcessor> class TemplateCreatorFunctor : public CommandCreatorFunctor<TheClass> {
private:
    TheProcessor            *m_pTheProcessor;                                           ///< Pointer to processor class.
    void (TheProcessor::    *m_FunctionPointer)(Global::tRefType, const TheClass &);    ///< Pointer to member function.
    /****************************************************************************/
    TemplateCreatorFunctor();                                                   ///< Not implemented.
    TemplateCreatorFunctor(const TemplateCreatorFunctor &);                     ///< Not implemented.
    /****************************************************************************/
    /*!
     *  \brief       Not implemented.
     *
     *  \return
     */
    /****************************************************************************/
    const TemplateCreatorFunctor & operator = (const TemplateCreatorFunctor &); ///< Not implemented.
public:
    /****************************************************************************/
    /**
     * \brief Constructor.
     *
     * \iparam   pTheProcessor       Processor class.
     * \iparam   FunctionPointer     The function pointer.
     */
    /****************************************************************************/
    TemplateCreatorFunctor(TheProcessor *pTheProcessor,
                           void(TheProcessor::*FunctionPointer)(Global::tRefType, const TheClass &)) :
        m_pTheProcessor(pTheProcessor),
        m_FunctionPointer(FunctionPointer)
    {
    }
    /****************************************************************************/
    /**
     * \brief Destructor.
     */
    /****************************************************************************/
    virtual ~TemplateCreatorFunctor() {
    }
    /****************************************************************************/
    /**
     * \brief Create instance an deserialize it.
     *
     * \warning You must ensure correct deletion of the returned pointer!
     *
     * \iparam       Ref         Command reference.
     * \iparam  DataStream  Data stream from which deserialization is done.
     */
    /****************************************************************************/
    virtual void CreateAndDeserialize(Global::tRefType Ref, QDataStream &DataStream) {
//        qDebug() << "Gui::CreatorFunctor::CreateAndDeserialize";
        // create temporary instance
        TheClass Instance;
        // stream data into it
        DataStream >> (Instance);
        // pass new created instance to consumer
        (*m_pTheProcessor.*m_FunctionPointer)(Ref, Instance);
    }
    /****************************************************************************/
    /**
     * \brief Copies the reference of an instance and calls the handler.
     *
     * \warning You must ensure correct deletion of the returned pointer!
     *
     * \iparam       Ref         Command reference.
     * \iparam       Instance    Reference to the original instance.
     */
    /****************************************************************************/
    virtual void CopyAndCall(Global::tRefType Ref, const TheClass &Instance) {
        (*m_pTheProcessor.*m_FunctionPointer)(Ref, Instance);
    }
}; // end class TemplateCreatorFunctor

} // namespace NetLayer

#endif // NETWORKBASE_CREATORFUNCTOR_H
