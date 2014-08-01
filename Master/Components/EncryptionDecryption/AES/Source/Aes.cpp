/****************************************************************************/
/*! \file Aes.cpp
 *
 *  \brief AES implementation for CTR mode
 *
 *  This is an adaption of the original NIST reference implementation and
 *  implements AES-128 encryption used for CTR mode encryption/decryption.
 *  Each key is used only once, so the counter block can always start with
 *  null (consisting of null bytes only). Because of CTR mode, no decryption
 *  has to be implemented.
 *
 *  $Version:   $ 1.0
 *  $Date:      $ 2012-11-26
 *  $Author:    $ Raju
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 *
 */
/****************************************************************************/

//lint -e1401
// a number of private members like m_roundkeys is not initialized
// by the constructor but through AES::init(); the tests with the help
// of the initialized flag guarantees that all is OK

#include "EncryptionDecryption/AES/Include/Aes.h"
#include "EncryptionDecryption/AES/Source/BoxesRef.dat"

namespace EncryptionDecryption {


/****************************************************************************/
/*!
 * \brief default constructor
 */
/****************************************************************************/
AES::AES(): initialized(false)
{

}

/****************************************************************************/
/*!
 * \brief initialize key
 *
 * Only one initialization is allowed.
 *
 * \iparam key - the key
 */
/****************************************************************************/
void AES::init(QByteArray& key)
{
    initialized = true;

    RijndaelKeySched(reinterpret_cast<quint8*>(key.data()));
    m_streamlen = 0;
    memset(&m_ctrblock[0], 0, AES_SIZE);
}

/****************************************************************************/
/*!
 * \brief CTR en/decryption
 *
 * Encryption and decryption are identical since they are the same XOR
 * with a keystream.
 *
 * \iparam data - QByteArray to be en/decrypted
 */
/****************************************************************************/
void AES::AesCtr(QByteArray& data)
{
    int datlen = data.size();
    quint8 *dp = reinterpret_cast<quint8*>(data.data());

    while(datlen > 0)
    {
        if(!m_streamlen)        // create new keystream item
        {
            // increment CTR block by 1, first byte is MSB
            quint8 *ctrptr = &m_ctrblock[AES_SIZE-1];
            while(!++*ctrptr)
            {
                --ctrptr;
            }

            // encrypt CTR block
            memcpy(m_keystream, m_ctrblock, AES_SIZE);
            RijndaelEncrypt(m_keystream);

            m_streamlen = AES_SIZE;
            m_keyptr = &m_keystream[0];
        }

        int processed = m_streamlen < datlen ? m_streamlen : datlen;

        // do the "XOR en/decryption"
        for(int counter = (processed-1); counter >=0; counter--)
        {
            *dp++ ^= *m_keyptr++;
        }

        m_streamlen -= processed;
        datlen -= processed;
    }
}

/****************************************************************************/
/*!
 * \brief multiply two elements of GF(2^m) needed for MixColumn
 *
 * \iparam elementone - first element
 * \iparam elementtwo - second element
 *
 * \return - product (byte)
 */
/****************************************************************************/
quint8 AES::Mul(quint8 elementone, quint8 elementtwo)
{
    if(elementone && elementtwo)
    {
        int logtablevalue = Logtable[elementone] + Logtable[elementtwo];
        while(logtablevalue > 254)
        {
            logtablevalue -= 255;
        }
        return Alogtable[logtablevalue];
    }
    else
    {
        return 0;
    }
}

/****************************************************************************/
/*!
 * \brief Xor corresponding text input and round key input bytes
 *
 * \oparam state - the state
 * \iparam roundkey - one round key
 */
/****************************************************************************/
void AES::KeyAddition(quint8 state[4][4], const quint8 roundkey[4][4])
{
    register int row;
    register int column;

    for(row = 3; row >= 0; row--)
    {
        for(column = 3; column >= 0; column--)
        {
            state[row][column] ^= roundkey[row][column];
        }
    }
}

/****************************************************************************/
/*!
 * \brief ShiftRow operation
 *
 * row 0 remains unchanged,
 * the other three rows are shifted a variable amount
 *
 * \iparam state - the state
 */
/****************************************************************************/
void AES::ShiftRow(quint8 state[4][4])
{
    quint8 tmp[4];
    register int row;
    register int column;

    for(row = 3; row >= 0; row--)
    {
        for(column = 3; column >= 0; column--)
        {
            tmp[column] = state[row][(column+row) & 3];
        }
        for(column = 3; column >= 0; column--)
        {
            state[row][column] = tmp[column];
        }
    }
}

/****************************************************************************/
/*!
 * \brief Byte substitution
 *
 * replace every byte of the input by the byte at that place
 * in the nonlinear S-box
 *
 * \iparam state - the state
 * \iparam box - the substitution box from BoxesRef.dat
 */
/****************************************************************************/
void AES::Substitution(quint8 state[4][4], const quint8 box[256])
{
    int row;
    int column;

    for(row = 3; row >= 0; row--)
    {
        for(column = 3; column >= 0; column--)
        {
            state[row][column] = box[state[row][column]] ;
        }
    }
}

/****************************************************************************/
/*!
 * \brief MixColumn operation
 *
 * Mix the four bytes of every column in a linear way.
 *
 * \iparam state - the state
 */
/****************************************************************************/
void AES::MixColumn(quint8 state[4][4])
{
    quint8 mixedarray[4][4];
    register int row;
    register int column;

    for(column = 3; column >= 0; column--)
    {
        for(row = 3; row >= 0; row--)
        {
            mixedarray[row][column] = Mul(2, state[row][column])
                    ^ Mul(3, state[(row+1)&3][column])
                    ^ state[(row+2)&3][column]
                    ^ state[(row+3)&3][column];
        }
    }

    for(row = 3; row >= 0; row--)
    {
        for(column = 3; column >= 0; column--)
        {
            state[row][column] = mixedarray[row][column];
        }
    }
}

/****************************************************************************/
/*!
 * \brief AES key schedule from a "linear" stored key
 * 
 * m_roundkeys is filled
 *
 * \iparam key - key stored columnwise
 */
/****************************************************************************/
void AES::RijndaelKeySched(const quint8 key[AES_SIZE])
{
    int row;
    int column;
    int tmpcounter;
    int rconpointer = 0;
    quint8 keyvalues[4][4];
    quint8 tempkey[4][4];

    for(row = (AES_SIZE-1); row >= 0; row--)
    {
        keyvalues[row&3][row>>2] = (quint8) key[row];
    }

    for(column = 3; column >= 0; column--)
    {
        for(row = 3; row >= 0; row--)
        {
            tempkey[row][column] = keyvalues[row][column];
        }
    }

    tmpcounter = 0;

    /* copy values into round key array */

    for(column = 0; (column < 4) && (tmpcounter < (ROUNDS+1)*4); ++column, ++tmpcounter)
    {
        for(row = 3; row >= 0; row--)
        {
            m_roundkeys[tmpcounter>>2][row][tmpcounter&3] = tempkey[row][column];
        }
    }

    while(tmpcounter < (ROUNDS+1)*4)
    {
        /* while not enough round key material calculated
         * calculate new values */

        for(row = 3; row >= 0; row--)
        {
            tempkey[row][0] ^= Sub[tempkey[(row+1)&3][3]];
        }
        tempkey[0][0] ^= rcon[rconpointer++];

        for(column = 1; column < 4; ++column)
        {
            for(row = 0; row < 4; ++row)
            {
                tempkey[row][column] ^= tempkey[row][column-1];
            }
        }

        /* copy values into round key array */
        for(column = 0; (column < 4) && (tmpcounter < (ROUNDS+1)*4); column++, tmpcounter++)
        {
            for(row = 3; row >= 0; row--)
            {
                m_roundkeys[tmpcounter>>2][row][tmpcounter&3] = tempkey[row][column];
            }
        }
    }
}

/****************************************************************************/
/*!
 * \brief encrypt one square block (4*4 byte)
 *
 * \iparam block -  plaintext stored columnwise
 */
/****************************************************************************/
void AES::RijndaelEncrypt(quint8 block[AES_SIZE])
{
    quint8 box[4][4];
    int counter;
    int roundcounter;

    /* fill block into square array a */

    for(counter = 3; counter >= 0; counter--)
    {
        for(roundcounter = 3; roundcounter >= 0; roundcounter--)
        {
            box[roundcounter][counter] = block[(counter<<2)+roundcounter];
        }
    }

    /* begin with a key addition */
    KeyAddition(box, m_roundkeys[0]);

    /* ROUNDS-1 ordinary rounds */
    for(roundcounter = 1; roundcounter < ROUNDS; ++roundcounter)
    {
        Substitution(box, Sub);
        ShiftRow(box);
        MixColumn(box);
        KeyAddition(box, m_roundkeys[roundcounter]);
    }

    /* Last round is special: there is no MixColumn */

    Substitution(box, Sub);
    ShiftRow(box);
    KeyAddition(box, m_roundkeys[ROUNDS]);

    /* copy square back to block */

    for(counter = 0; counter < 4; ++counter)
    {
        for(roundcounter = 0; roundcounter < 4; ++roundcounter)
        {
            block[(counter<<2)+roundcounter] = box[roundcounter][counter];
        }
    }
}

}       // end namespace EncryptionDecryption
