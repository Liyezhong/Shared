/****************************************************************************/
/*! \file fmTemperatureSensor.c
 * 
 *  \brief Functions reading the temperature sensor.
 *
 *   $Version: $ 0.1
 *   $Date:    $ 02.07.2010
 *   $Author:  $ Martin Scherer
 *
 *  \b Description:
 *
 *  This file controls the read out of data from a temperature sensor. It
 *  converts the temperature values to 0.01 steps in degree Celsius. 
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 * 
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice 
 *  does not evidence any actual or intended publication.
 */
/****************************************************************************/

#include <stdio.h>
#include "Global.h"
#include "halLayer.h"
#include "BaseModule.h"
#include "fmTemperature.h"
#include "fmTemperatureSensor.h"

//****************************************************************************/
// Private Constants and Macros 
//****************************************************************************/
#define SWAP(X,Y) {X=X^Y;Y=X^Y;X=X^Y;}//!< Swap the value of two variables

#define TEMP_SENSOR_MAX 200           //!< Maximal sensor temperature in degree Celsius 

#define TEMP_SENSOR_VOLTAGE     3000  //!< ADC supply voltage used for the NTC 10K3A1I

#ifdef ASB5_VER_A
#define TEMP_SENSOR_RESISTANCE  10000 //!< Pullup resistance connected to NTC 10K3A1I
#endif

// For new ASB board
#ifdef ASB15_VER_B
#define TEMP_SENSOR_RESISTANCE  2700 //!< Pullup resistance connected to NTC 10K3A1I
#endif

/*! Conversion table for a thermocouple of type K (IEC 584) in microvolts */
static Int16 tempSensorTableTypeK[TEMP_SENSOR_MAX] = {
       0,   39,   79,  119,  158,  198,  238,  277,  317,  357, //   0 to   9 degree Celsius
     397,  437,  477,  517,  557,  597,  637,  677,  718,  758, //  10 to  19 degree Celsius
     798,  838,  879,  919,  960, 1000, 1041, 1081, 1122, 1162, //  20 to  29 degree Celsius
    1203, 1244, 1285, 1325, 1366, 1407, 1448, 1489, 1529, 1570, //  30 to  39 degree Celsius
    1611, 1652, 1693, 1734, 1776, 1817, 1858, 1899, 1940, 1981, //  40 to  49 degree Celsius
    2022, 2064, 2105, 2146, 2188, 2229, 2270, 2312, 2353, 2394, //  50 to  59 degree Celsius
    2436, 2477, 2519, 2560, 2601, 2643, 2684, 2726, 2767, 2809, //  60 to  69 degree Celsius
    2850, 2892, 2933, 2975, 3016, 3058, 3100, 3141, 3183, 3224, //  70 to  79 degree Celsius
    3266, 3307, 3349, 3390, 3432, 3473, 3515, 3550, 3598, 3639, //  80 to  89 degree Celsius
    3681, 3722, 3764, 3805, 3847, 3888, 3930, 3971, 4012, 4054, //  90 to  99 degree Celsius
    4095, 4137, 4178, 4219, 4261, 4302, 4343, 4384, 4426, 4467, // 100 to 109 degree Celsius
    4508, 4549, 4590, 4632, 4673, 4714, 4755, 4796, 4837, 4878, // 110 to 119 degree Celsius
    4920, 4961, 5002, 5043, 5084, 5124, 5165, 5206, 5247, 5288, // 120 to 129 degree Celsius
    5328, 5369, 5410, 5450, 5491, 5532, 5572, 5613, 5653, 5694, // 130 to 139 degree Celsius
    5735, 5775, 5815, 5856, 5896, 5937, 5977, 6017, 6058, 6098, // 140 to 149 degree Celsius
    6138, 6179, 6219, 6259, 6299, 6340, 6380, 6420, 6460, 6500, // 150 to 159 degree Celsius
    6540, 6580, 6620, 6660, 6701, 6741, 6781, 6821, 6861, 6901, // 160 to 169 degree Celsius
    6941, 6981, 7021, 7060, 7100, 7140, 7180, 7220, 7260, 7300, // 170 to 179 degree Celsius
    7340, 7380, 7420, 7460, 7500, 7540, 7579, 7619, 7659, 7699, // 180 to 189 degree Celsius
    7739, 7779, 7819, 7859, 7899, 7939, 7979, 8019, 8059, 8099  // 190 to 199 degree Celsius
};

/*! Conversion table for a thermocouple of type T (IEC 584) in microvolts */
static Int16 tempSensorTableTypeT[TEMP_SENSOR_MAX] = {
       0,   39,   78,  117,  156,  195,  234,  273,  312,  352, //   0 to   9 degree Celsius
     391,  431,  470,  510,  549,  589,  629,  669,  709,  749, //  10 to  19 degree Celsius
     790,  830,  870,  911,  951,  992, 1033, 1074, 1114, 1155, //  20 to  29 degree Celsius
    1196, 1238, 1279, 1320, 1362, 1403, 1445, 1486, 1528, 1570, //  30 to  39 degree Celsius
    1612, 1654, 1696, 1738, 1780, 1823, 1865, 1908, 1950, 1993, //  40 to  49 degree Celsius
    2036, 2079, 2122, 2165, 2108, 2251, 2294, 2338, 2381, 2425, //  50 to  59 degree Celsius
    2468, 2512, 2556, 2600, 2643, 2687, 2732, 2776, 2820, 2864, //  60 to  69 degree Celsius
    2909, 2953, 2998, 3043, 3087, 3132, 3177, 3222, 3267, 3312, //  70 to  79 degree Celsius
    3358, 3403, 3448, 3494, 3539, 3585, 3631, 3677, 3722, 3768, //  80 to  89 degree Celsius
    3814, 3860, 3907, 3953, 3999, 4046, 4092, 4138, 4185, 4232, //  90 to  99 degree Celsius
    4279, 4325, 4372, 4419, 4466, 4513, 4561, 4608, 4655, 4702, // 100 to 109 degree Celsius
    4750, 4798, 4845, 4893, 4941, 4988, 5036, 5084, 5132, 5180, // 110 to 119 degree Celsius
    5228, 5277, 5325, 5373, 5422, 5470, 5519, 5567, 5616, 5665, // 120 to 129 degree Celsius
    5714, 5763, 5812, 5861, 5910, 5959, 6008, 6057, 6107, 6156, // 130 to 139 degree Celsius
    6206, 6255, 6305, 6355, 6404, 6454, 6504, 6554, 6604, 6654, // 140 to 149 degree Celsius
    6704, 6754, 6805, 6855, 6905, 6956, 7006, 7057, 7107, 7158, // 150 to 159 degree Celsius
    7209, 7260, 7310, 7361, 7412, 7463, 7515, 7566, 7617, 7668, // 160 to 169 degree Celsius
    7720, 7771, 7823, 7874, 7926, 7977, 8029, 8081, 8133, 8185, // 170 to 179 degree Celsius
    8237, 8289, 8341, 8393, 8445, 8497, 8550, 8602, 8654, 8707, // 180 to 189 degree Celsius
    8759, 8812, 8865, 8917, 8970, 9023, 9076, 9129, 9182, 9235  // 190 to 199 degree Celsius
};

#ifdef ASB5_VER_A
/*! Conversion table for the Betatherm NTC 10K3A1I thermistor in ohms */
static Int16 tempSensorTable10K3A1I[TEMP_SENSOR_MAX] = {
    32651, 31031, 29500, 28054, 26687, 25395, 24172, 23016, 21921, 20885, //   0 to   9 degree Celsius
    19903, 18973, 18092, 17257, 16465, 15714, 15001, 14324, 13682, 13073, //  10 to  19 degree Celsius
    12493, 11943, 11420, 10923, 10450, 10000,  9572,  9165,  8777,  8408, //  20 to  29 degree Celsius
     8056,  7721,  7402,  7097,  6807,  6530,  6266,  6014,  5774,  5544, //  30 to  39 degree Celsius
     5325,  5116,  4916,  4724,  4542,  4367,  4200,  4040,  3887,  3741, //  40 to  49 degree Celsius
     3601,  3467,  3339,  3216,  3098,  2985,  2877,  2773,  2674,  2579, //  50 to  59 degree Celsius
     2487,  2399,  2315,  2234,  2157,  2082,  2011,  1942,  1876,  1813, //  60 to  69 degree Celsius
     1752,  1693,  1637,  1582,  1530,  1480,  1432,  1385,  1341,  1298, //  70 to  79 degree Celsius
     1256,  1216,  1178,  1141,  1105,  1070,  1037,  1005,   974,   945, //  80 to  89 degree Celsius
      916,   888,   862,   836,   811,   787,   764,   741,   720,   699, //  90 to  99 degree Celsius
      678,   659,   640,   622,   604,   587,   571,   555,   539,   524, // 100 to 109 degree Celsius
      510,   496,   482,   469,   457,   444,   432,   421,   410,   399, // 110 to 119 degree Celsius
      397,   387,   378,   368,   359,   350,   341,   333,   325,   317, // 120 to 129 degree Celsius 
      309,   302,   295,   287,   281,   274,   268,   261,   255,   249, // 130 to 139 degree Celsius 
      244,   238,   232,   227,   222,   217,   212,   207,   203,   198, // 140 to 149 degree Celsius 
      194,   190,   186,   182,   178,   174,   170,   166,   163,   159, // 150 to 159 degree Celsius 
      156,   153,   150,   147,   144,   141,   138,   135,   132,   129, // 160 to 169 degree Celsius 
      127,   124,   122,   119,   117,   115,   113,   110,   108,   106, // 170 to 179 degree Celsius 
      104,   102,   100,    98,    96,    95,    93,    91,    89,    88, // 180 to 189 degree Celsius 
       86,    85,    83,    81,    80,    79,    77,    76,    74,    73  // 190 to 199 degree Celsius      
};
#endif

#ifdef ASB15_VER_B
/*! Conversion table for the Betatherm NTC 10K3A1I thermistor in ohms */
static Int16 tempSensorTable10K3A1I[TEMP_SENSOR_MAX] = {
    32767, 32177, 30523, 28965, 27497, 26114, 24809, 23578, 22416, 21319, //   0 to   9 degree Celsius
    20283, 19304, 18378, 17503, 16676, 15893, 15151, 14449, 13784, 13154, //  10 to  19 degree Celsius
    12557, 11990, 11453, 10943, 10459, 10000,  9564,  9149,  8755,  8380, //  20 to  29 degree Celsius 
     8024,  7685,  7362,  7055,  6763,  6484,  6219,  5966,  5725,  5495, //  30 to  39 degree Celsius 
     5276,  5067,  4867,  4677,  4495,  4321,  4155,  3996,  3844,  3699, //  40 to  49 degree Celsius 
     3561,  3428,  3301,  3179,  3063,  2951,  2845,  2742,  2644,  2550, //  50 to  59 degree Celsius 
     2460,  2374,  2291,  2211,  2135,  2062,  1992,  1924,  1859,  1797, //  60 to  69 degree Celsius 
     1737,  1679,  1624,  1571,  1520,  1470,  1423,  1378,  1334,  1291, //  70 to  79 degree Celsius 
     1251,  1212,  1174,  1138,  1102,  1069,  1036,  1005,   974,   945, //  80 to  89 degree Celsius 
      917,   890,   864,   838,   814,   790,   767,   745,   724,   704, //  90 to  99 degree Celsius 
      684,   665,   646,   628,   611,   594,   578,   562,   547,   532, // 100 to 109 degree Celsius
      518,   504,   490,   478,   465,   453,   441,   430,   419,   408, // 110 to 119 degree Celsius 
      397,   387,   378,   368,   359,   350,   341,   333,   325,   317, // 120 to 129 degree Celsius 
      309,   302,   295,   287,   281,   274,   268,   261,   255,   249, // 130 to 139 degree Celsius 
      244,   238,   232,   227,   222,   217,   212,   207,   203,   198, // 140 to 149 degree Celsius 
      194,   190,   186,   182,   178,   174,   170,   166,   163,   159, // 150 to 159 degree Celsius 
      156,   153,   150,   147,   144,   141,   138,   135,   132,   129, // 160 to 169 degree Celsius 
      127,   124,   122,   119,   117,   115,   113,   110,   108,   106, // 170 to 179 degree Celsius 
      104,   102,   100,    98,    96,    95,    93,    91,    89,    88, // 180 to 189 degree Celsius 
       86,    85,    83,    81,    80,    79,    77,    76,    74,    73  // 190 to 199 degree Celsius
};
#endif

/*! Conversion table for the Exsense NTC GT103F3950A thermistor in ohms */
static Int16 tempSensorTableGT103F[TEMP_SENSOR_MAX] = {
 32767, 31932, 30301, 28765, 27316, 25950, 24662, 23446, 22298, 21214, //   0 to   9 degree Celsius
 20189, 19221, 18305, 17440, 16620, 15845, 15110, 14415, 13755, 13131, //  10 to  19 degree Celsius
 12538, 11976, 11443, 10937, 10456, 10000,  9566,  9154,  8762,  8390, //  20 to  29 degree Celsius
  8035,  7698,  7377,  7071,  6780,  6503,  6238,  5986,  5746,  5517, //  30 to  39 degree Celsius
  5298,  5089,  4890,  4700,  4518,  4345,  4179,  4020,  3868,  3723, //  40 to  49 degree Celsius 
  3585,  3452,  3325,  3203,  3087,  2975,  2868,  2766,  2667,  2573, //  50 to  59 degree Celsius
  2483,  2396,  2313,  2233,  2157,  2083,  2013,  1945,  1880,  1817, //  60 to  69 degree Celsius
  1757,  1699,  1643,  1590,  1539,  1489,  1441,  1396,  1351,  1309, //  70 to  79 degree Celsius
  1268,  1228,  1190,  1154,  1118,  1084,  1052,  1020,   989,   960, //  80 to  89 degree Celsius
   932,   904,   878,   852,   827,   804,   780,   758,   737,   716, //  90 to  99 degree Celsius
   696,   676,   658,   639,   622,   605,   588,   573,   557,   542, // 100 to 109 degree Celsius
   528,   514,   500,   487,   474,   462,   450,   439,   427,   416, // 110 to 119 degree Celsius
   406,   396,   386,   376,   367,   358,   349,   340,   332,   324, // 120 to 129 degree Celsius  
   316,   309,   301,   294,   287,   281,   274,   268,   261,   255, // 130 to 139 degree Celsius  
   250,   244,   238,   233,   228,   223,   218,   213,   208,   203, // 140 to 149 degree Celsius  
   199,   195,   190,   186,   182,   178,   175,   171,   167,   164, // 150 to 159 degree Celsius  
   160,   157,   154,   151,   148,   145,   142,   139,   136,   133, // 160 to 169 degree Celsius  
   131,   128,   125,   123,   121,   118,   116,   114,   111,   109, // 170 to 179 degree Celsius  
   107,   105,   103,   101,    99,    97,    96,    94,    92,    90, // 180 to 189 degree Celsius  
    89,    87,    86,    84,    83,    81,    80,    78,    77,    75  // 190 to 199 degree Celsius
};


/*! Conversion table for the Analog Devices AD595 in millivolts */
//static Int16 tempSensorTableAD595[TEMP_SENSOR_MAX] = {
static const Int16 tempSensorTableAD595[TEMP_SENSOR_MAX] = {    //  Todo: how to reduce RAM size
       3,   13,   22,   32,   42,   52,   62,   72,   81,   91, //   0 to   9 degree Celsius
     101,  111,  121,  131,  141,  151,  160,  170,  180,  190, //  10 to  19 degree Celsius
     200,  210,  220,  230,  240,  250,  260,  270,  280,  290, //  20 to  29 degree Celsius
     300,  310,  320,  330,  340,  351,  361,  371,  381,  391, //  30 to  39 degree Celsius
     401,  411,  421,  432,  442,  452,  462,  472,  483,  493, //  40 to  49 degree Celsius
     503,  513,  523,  534,  544,  554,  564,  574,  585,  595, //  50 to  59 degree Celsius
     605,  615,  626,  636,  646,  656,  667,  677,  687,  697, //  60 to  69 degree Celsius
     708,  718,  728,  738,  749,  759,  769,  779,  790,  800, //  70 to  79 degree Celsius
     810,  820,  831,  841,  851,  861,  872,  882,  892,  902, //  80 to  89 degree Celsius
     913,  923,  933,  943,  954,  964,  974,  984,  995, 1005, //  90 to  99 degree Celsius
    1015, 1025, 1035, 1046, 1056, 1066, 1076, 1086, 1097, 1107, // 100 to 109 degree Celsius
    1117, 1127, 1137, 1148, 1158, 1168, 1178, 1188, 1199, 1209  // 110 to 119 degree Celsius
};

/*! Conversion table for the PT1000 resistance thermometer in 0.1 ohm steps */
static Int16 tempSensorTablePT1000[TEMP_SENSOR_MAX] = {
    10000, 10039, 10078, 10117, 10156, 10195, 10234, 10273, 10312, 10351, //   0 to   9 degree Celsius
    10390, 10429, 10468, 10507, 10546, 10585, 10624, 10663, 10702, 10740, //  10 to  19 degree Celsius
    10779, 10818, 10857, 10896, 10935, 10973, 11012, 11051, 11090, 11129, //  20 to  29 degree Celsius
    11167, 11206, 11245, 11283, 11322, 11361, 11400, 11438, 11477, 11515, //  30 to  39 degree Celsius
    11554, 11593, 11631, 11670, 11708, 11747, 11786, 11824, 11863, 11901, //  40 to  49 degree Celsius
    11940, 11978, 12017, 12055, 12094, 12132, 12171, 12209, 12247, 12286, //  50 to  59 degree Celsius
    12324, 12363, 12401, 12439, 12478, 12516, 12554, 12593, 12631, 12669, //  60 to  69 degree Celsius
    12708, 12746, 12784, 12822, 12861, 12899, 12937, 12975, 13013, 13052, //  70 to  79 degree Celsius
    13090, 13128, 13166, 13204, 13242, 13280, 13318, 13357, 13395, 13433, //  80 to  89 degree Celsius
    13471, 13509, 13547, 13585, 13623, 13661, 13699, 13737, 13775, 13813, //  90 to  99 degree Celsius
    13851, 13888, 13926, 13964, 14002, 14040, 14078, 14116, 14154, 14191, // 100 to 109 degree Celsius
    14229, 14267, 14305, 14343, 14380, 14418, 14456, 14494, 14531, 14569, // 110 to 119 degree Celsius
    14607, 14644, 14682, 14720, 14757, 14795, 14833, 14870, 14908, 14946, // 120 to 129 degree Celsius
    14983, 15021, 15058, 15096, 15133, 15171, 15208, 15246, 15283, 15321, // 130 to 139 degree Celsius
    15358, 15396, 15433, 15471, 15508, 15546, 15583, 15620, 15658, 15695, // 140 to 149 degree Celsius
    15733, 15770, 15807, 15845, 15882, 15919, 15956, 15994, 16031, 16068, // 150 to 159 degree Celsius
    16105, 16143, 16180, 16217, 16254, 16291, 16329, 16366, 16403, 16440, // 160 to 169 degree Celsius
    16477, 16514, 16551, 16589, 16626, 16663, 16700, 16737, 16774, 16811, // 170 to 179 degree Celsius
    16848, 16885, 16922, 16959, 16996, 17033, 17070, 17107, 17143, 17180, // 180 to 189 degree Celsius
    17217, 17254, 17291, 17328, 17365, 17402, 17438, 17475, 17512, 17549  // 190 to 199 degree Celsius
};                   

//****************************************************************************/
// Private Type Definitions 
//****************************************************************************/

//****************************************************************************/
// Private Variables 
//****************************************************************************/

//****************************************************************************/
// Private Function Prototypes 
//****************************************************************************/


/*****************************************************************************/
/*! 
 *  \brief   Reads a temperature sensor
 *
 *      This method reads an analog input port. The fetched value is then
 *      converted into a temperature using the conversion table of the sensor
 *      type that is passed to this function.
 * 
 *  \iparam  Handle = Handle of the analog input
 *  \iparam  Type = Type of the temperature sensor
 *  \iparam  ColdJunction = Cold junction temperature (only for thermocouples)
 *  \oparam  Temperature = Temperature in 0.01 degree Celsius steps
 *
 *  \return  NO_ERROR or (negative) error code
 *
 ****************************************************************************/
 
Error_t tempSensorRead (Handle_t Handle, TempSensorType_t Type, UInt16 ColdJunction, UInt16* Temperature)
{
    Error_t Error;
    UInt8 i;
    Int32 AdcValue;
#if 0 
    Int16 AdcValueRead;
#endif
    Int16 AdcValue1, AdcValue2, AdcValue3;
    Int16 *Table;
    Int16 Voltage;
       
    if (ColdJunction / 100 >= TEMP_SENSOR_MAX) {
        dbgPrint("CJ:%d ", ColdJunction);
        return (E_TEMP_SENSOR_OUT_OF_RANGE);
    }

#if 0   
    if ((Error = halAnalogRead (Handle, &AdcValueRead)) < 0) {
        return (Error);
    }
    AdcValue = AdcValueRead;
#endif

#if 1    
 // Median filtering
    if ((Error = halAnalogRead (Handle, &AdcValue1)) < 0) {
        dbgPrint("AD:Err1[%d] ", Error);
        return (Error);
    }
    if ((Error = halAnalogRead (Handle, &AdcValue2)) < 0) {
        dbgPrint("AD:Err2[%d] ", Error);
        return (Error);
    }
    if ((Error = halAnalogRead (Handle, &AdcValue3)) < 0) {
        dbgPrint("AD:Err3[%d] ", Error);
        return (Error);
    }
    
     if (AdcValue1>AdcValue2) {
         SWAP(AdcValue1,AdcValue2);
     }
     if (AdcValue1>AdcValue3) {
         SWAP(AdcValue1,AdcValue3);
     }
     if (AdcValue2>AdcValue3) {
         SWAP(AdcValue2,AdcValue3);
     }
     
     AdcValue = AdcValue2;

#endif     
    
    if (Type == TYPEK) {
        Table = tempSensorTableTypeK;
        Voltage = Table[ColdJunction / 100] + ((ColdJunction % 100) * (Table[ColdJunction / 100 + 1]  - Table[ColdJunction / 100])) / 100;
        AdcValue += Voltage;
    }
    else if (Type == NTC10K3A1I) {
        AdcValue = (TEMP_SENSOR_RESISTANCE * (Int32) AdcValue) / (TEMP_SENSOR_VOLTAGE - AdcValue);
        Table = tempSensorTable10K3A1I;
    }
    else if (Type == AD595) {
        //Table = tempSensorTableAD595;
        //  Todo: how to reduce RAM size
        Table = (Int16*)tempSensorTableAD595;
    }
    else if (Type == TYPET) {
        Table = tempSensorTableTypeT;
        Voltage = Table[ColdJunction / 100] + ((ColdJunction % 100) * (Table[ColdJunction / 100 + 1]  - Table[ColdJunction / 100])) / 100;
        AdcValue += Voltage;
    }
    else if (Type == PT1000) {
        Table = tempSensorTablePT1000;
    }
    else if (Type == NTCGT103F) {
        AdcValue = (TEMP_SENSOR_RESISTANCE * (Int32) AdcValue) / (TEMP_SENSOR_VOLTAGE - AdcValue);
        Table = tempSensorTableGT103F;
    }
    else {
        return (E_TEMP_SENSOR_NOT_SUPPORTED);
    };
    
    for (i = 0; i < TEMP_SENSOR_MAX - 2; i++) {
        if (AdcValue >= Table[i] && AdcValue <= Table[i + 1]) {
            *Temperature = 100 * i + (100 * (AdcValue - Table[i])) / (Table[i+1] - Table[i]);
            return (NO_ERROR);
        }
        else if (AdcValue <= Table[i] && AdcValue >= Table[i + 1]) {
            *Temperature = 100 * i + (100 * (Table[i] - AdcValue)) / (Table[i] - Table[i+1]);
            return (NO_ERROR);
        }
    }
    
    return (E_TEMP_SENSOR_OUT_OF_RANGE);
}


//****************************************************************************/
