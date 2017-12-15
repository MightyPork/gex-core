//
// Created by MightyPork on 2017/12/02.
//

#ifndef GEX_SYSTEM_SETTINGS_H
#define GEX_SYSTEM_SETTINGS_H

#include "platform.h"
#include "utils/ini_writer.h"
#include "utils/payload_parser.h"
#include "utils/payload_builder.h"

struct system_settings {
    bool visible_vcom;

    // Support flags put here for scoping, but not atcually part of the persistent settings
    volatile bool editable; //!< True if we booted with the LOCK jumper removed
    volatile bool modified; //!< True if user did any change to the settings (checked when the LOCK jumper is replaced)
};

extern struct system_settings SystemSettings;

/**
 * Load defaults
 */
void systemsettings_init(void);

/**
 * Write system settings to the pack context
 */
void systemsettings_save(PayloadBuilder *pb);

/**
 * Load system settings from the unpack context
 */
bool systemsettings_load(PayloadParser *pp);

/**
 * Write system settings to INI
 */
void systemsettings_write_ini(IniWriter *iw);

/**
 * Load system settings from INI kv pair
 *
 * @return true on success
 */
bool systemsettings_read_ini(const char *restrict key, const char *restrict value);

#endif //GEX_SYSTEM_SETTINGS_H
