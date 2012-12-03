/****************************************************************************/
/*! \file Main.h
 *
 *  \brief Include file for main routine.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 04.07.2011
 *  $Author:    $ Y.Novak
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

#ifndef MAIN_H
#define MAIN_H

/****************************************************************************/
/**
 * \brief Main function.
 *
 * The Axeda Controller is created, initialized and started.
 * Qt Application object is created and started.
 * As soon as Axeda Controller quits, the Qt Application is terminated.
 *
 * \param[in]   argc    Argument count.
 * \param[in]   argv    Argument list.
 * \return              Return code.
 */
/****************************************************************************/
int main(int argc, char *argv[]);

#endif // MAIN_H
