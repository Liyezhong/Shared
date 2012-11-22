/****************************************************************************/
/** @file Aes.c
 *
 *  @brief AES implementation for CTR mode
 *
 *  This is an adaption of the original NIST reference implementation and
 *  implements AES-128 encryption used for CTR mode encryption/decryption.
 *  Each key is used only once, so the counter block can always start with
 *  null (consisting of null bytes only). Because of CTR mode, no decryption
 *  has to be implemented.
 *
 *  $Version:   $ 0.1
 *  $Date:      $ 2011-06-17
 *  $Author:    $ R.Wobst
 *
 *  @b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 *  last modified by owner: @(#) Jul 02 2011, 17:20:35
 *
 */
/****************************************************************************/

//lint -e1401
// a number of private members like m_roundkeys is not initialized
// by the constructor but through AES::init(); the tests with the help
// of the initialized flag guarantees that all is OK

#include "ImportExport/AES/Include/Aes.h"
#include <assert.h>

namespace ImportExport {

#include "ImportExport/AES/Source/BoxesRef.dat"

// ********** PUBLIC *************************

/**
 * @brief default constructor
 *
 * @param key - the key
 */

AES::AES(): initialized(false) {}

/**
 * @brief initialize key
 *
 * Only one initialization is allowed.
 *
 * @param key - the key
 */

void AES::init(QByteArray& key)
{
    Q_ASSERT(!initialized);
    initialized = true;

    assert(key.size() == AES_SIZE);
    RijndaelKeySched(reinterpret_cast<quint8*>(key.data()));
    m_streamlen = 0;
    memset(&m_ctrblock[0], 0, AES_SIZE);
}


/**
 * @brief CTR en/decryption
 *
 * Encryption and decryption are identical since they are the same XOR
 * with a keystream.
 *
 * @param data - QByteArray to be en/decrypted
 */

void AES::AesCtr(QByteArray& data)
{
    Q_ASSERT(initialized);

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

        for(int i=processed; i--;)
        {
            *dp++ ^= *m_keyptr++;
        }

        m_streamlen -= processed;
        datlen -= processed;
    }
}

// ********** PRIVATE *************************

/**
 * @brief multiply two elements of GF(2^m) needed for MixColumn
 *
 * @param -  a, b input elements
 * @return - product (byte)
 */

quint8 AES::Mul(quint8 a, quint8 b)
  {
   if(a && b)
     {
      int i = Logtable[a] + Logtable[b];
      while(i > 254) i -= 255;
      return Alogtable[i];
     }
   else return 0;
  }


/**
 * @brief Xor corresponding text input and round key input bytes
 *
 * @param a - the state (in,out)
 * @param rk - one round key
 */

void AES::KeyAddition(quint8 a[4][4], const quint8 rk[4][4])
{
    register int i, j;

    for(i=4; i--;)
    {
        for(j=4; j--;)
        {
            a[i][j] ^= rk[i][j];
        }
    }
}


/**
 * @brief ShiftRow operation
 *
 * row 0 remains unchanged,
 * the other three rows are shifted a variable amount
 *
 * @param a - the state
 */

void AES::ShiftRow(quint8 a[4][4])
{
    quint8 tmp[4];
    register int i, j;

    for(i=4; i--;)
    {
        for(j=4; j--;) tmp[j] = a[i][(j+i) & 3];
        for(j=4; j--;) a[i][j] = tmp[j];
    }
}


/**
 * @brief Byte substitution
 *
 * replace every byte of the input by the byte at that place
 * in the nonlinear S-box
 *
 * @param a - the state
 * @param box - the substitution box from BoxesRef.dat
 */

void AES::Substitution(quint8 a[4][4], const quint8 box[256])
{
    int i, j;

    for(i=4; i--;)
    {
        for(j=4; j--;)
        {
            a[i][j] = box[a[i][j]] ;
        }
    }
}


/**
 * @brief MixColumn operation
 *
 * Mix the four bytes of every column in a linear way.
 *
 * @param a - the state
 */

void AES::MixColumn(quint8 a[4][4])
{
    quint8 b[4][4];
    register int i, j;

    for(j=4; j--;)
    {
        for(i=4; i--;)
        {
            b[i][j] = Mul(2, a[i][j])
                ^ Mul(3, a[(i+1)&3][j])
                ^ a[(i+2)&3][j]
                ^ a[(i+3)&3][j];
        }
    }

    for(i=4; i--;)
    {
        for(j=4; j--;)
        {
            a[i][j] = b[i][j];
        }
    }
}


/**
 * @brief AES key schedule from a "linear" stored key
 * 
 * m_roundkeys is filled
 *
 * @param key - key stored columnwise
 */

void AES::RijndaelKeySched(const quint8 key[AES_SIZE])
{
    int i, j, t, rconpointer = 0;
    quint8 k[4][4], tk[4][4];

    for(i=AES_SIZE; i--;)
    {
        k[i&3][i>>2] = (quint8) key[i];
    }

    for(j=4; j--;)
    {
        for(i=4; i--;)
        {
            tk[i][j] = k[i][j];
        }
    }

    t = 0;

    /* copy values into round key array */

    for(j = 0; (j < 4) && (t < (ROUNDS+1)*4); ++j, ++t)
    {
        for(i=4; i--;)
        {
            m_roundkeys[t>>2][i][t&3] = tk[i][j];
        }
    }

    while(t < (ROUNDS+1)*4)
    {
        /* while not enough round key material calculated
         * calculate new values */

        for(i=4; i--;)
        {
            tk[i][0] ^= S[tk[(i+1)&3][3]];
        }
        tk[0][0] ^= rcon[rconpointer++];

        for(j = 1; j < 4; ++j)
        {
            for(i = 0; i < 4; ++i)
            {
                tk[i][j] ^= tk[i][j-1];
            }
        }

        /* copy values into round key array */
        for(j = 0; (j < 4) && (t < (ROUNDS+1)*4); j++, t++)
        {
            for(i=4; i--;)
            {m_roundkeys[t>>2][i][t&3] = tk[i][j];
            }
        }
    }
}


/**
 * @brief encrypt one square block (4*4 byte)
 *
 * @param[in,out] block -  plaintext stored columnwise
 */

void AES::RijndaelEncrypt(quint8 block[AES_SIZE])
{
    quint8 a[4][4];
    int i, r;

    /* fill block into square array a */

    for(i=4; i--;)
    {
        for(r=4; r--;)
        {
            a[r][i] = block[(i<<2)+r];
        }
    }

    /* begin with a key addition */
    KeyAddition(a, m_roundkeys[0]);

    /* ROUNDS-1 ordinary rounds */
    for(r=1; r < ROUNDS; ++r)
    {
        Substitution(a, S);
        ShiftRow(a);
        MixColumn(a);
        KeyAddition(a, m_roundkeys[r]);
    }

    /* Last round is special: there is no MixColumn */

    Substitution(a, S);
    ShiftRow(a);
    KeyAddition(a, m_roundkeys[ROUNDS]);

    /* copy square back to block */

    for(i=0; i < 4; ++i)
    {
        for(r=0; r < 4; ++r)
        {
            block[(i<<2)+r] = a[r][i];
        }
    }
}

}       // end namespace ImportExport
