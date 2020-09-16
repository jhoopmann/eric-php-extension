/*
 * Copyright © LfSt Bayern, Muenchen, IuK 111, 2006
 */

#ifndef ERICDEF_H_
#define ERICDEF_H_

#include "platform.h"

/**
 * @file
 * @brief Konstanten und Definitionen für Übergabeparameter
 */

/**
 * @brief Definition der maximalen Länge des
 *        Fusstextes in #eric_druck_parameter_t + Nullterminierer 
 */
#define ERIC_MAX_LAENGE_FUSSTEXT (30)

/**
 * @brief Definition des Standard Testmerkers. Bei der Verwendung dieses Testmerkers werden die
 *        Fälle in der Clearingstelle aussortiert und verworfen. Es findet keine Verarbeitung
 *        im Finanzamt statt.
 */
#define ERIC_TESTMERKER_CLEARINGSTELLE "700000004"

/**
 * @brief Definition des Testmerkers für das ECC. Bei der Verwendung dieses Testmerkers werden die
 *        Fälle in der Landeskopfstelle bzw dem ECC aussortiert und verworfen. Es findet keine Verarbeitung
 *        im Finanzamt statt.
 */
#define ERIC_TESTMERKER_ECC "700000001"

/* UF-8 Code für EURO Symbol C-Version */
#define EURO (unsigned char)0x20AC 


#endif /*ERICDEF_H_*/
