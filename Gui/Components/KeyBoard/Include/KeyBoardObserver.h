/****************************************************************************/
/*! \file KeyBoardObserver.h
 *
 *  \brief Keyboard Observer definition.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 2012-01-12
 *   $Author:  $ N.Kamath
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
#ifndef KEYBOARDOBSERVER_H
#define KEYBOARDOBSERVER_H




namespace KeyBoard{

//!< Forward declaration
class CKeyBoard;
/****************************************************************************/
/**
 * \brief Defines the observer design pattern
 */
/****************************************************************************/
class CKeyBoardObserver {

public:
    /****************************************************************************/
    /**
     * \brief Constructor
     */
    /****************************************************************************/
    CKeyBoardObserver() {}

    /****************************************************************************/
    /**
     * \brief Destructor
     */
    /****************************************************************************/
    virtual ~CKeyBoardObserver() {}

    /****************************************************************************/
    /**
     * \brief This function is called by the concrete subject i.e the Keyboard object,
     *        to notify the observer when Ok button on Keyboard is clicked
     */
    /****************************************************************************/
    virtual void Update() = 0;

    /****************************************************************************/
    /**
     * \brief This function is called by the concrete subject i.e the Keyboard object,
     *        to notify the observer when ESC button on Keyboard is clicked
     */
    /****************************************************************************/
    virtual void UpdateOnESC() = 0;
};

}

#endif // KEYBOARDOBSERVER_H
