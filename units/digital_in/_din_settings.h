//
// Created by MightyPork on 2018/02/03.
//

#ifndef GEX_F072_DIN_SETTINGS_H
#define GEX_F072_DIN_SETTINGS_H

#ifndef DIN_INTERNAL
#error bad include!
#endif

#include "unit_base.h"

/** Allocate data structure and set defaults */
error_t DIn_preInit(Unit *unit);

/** Load from a binary buffer stored in Flash */
void DIn_loadBinary(Unit *unit, PayloadParser *pp);

/** Write to a binary buffer for storing in Flash */
void DIn_writeBinary(Unit *unit, PayloadBuilder *pb);

// ------------------------------------------------------------------------

/** Parse a key-value pair from the INI file */
error_t DIn_loadIni(Unit *unit, const char *key, const char *value);

/** Generate INI file section for the unit */
void DIn_writeIni(Unit *unit, IniWriter *iw);

#endif //GEX_F072_DIN_SETTINGS_H
