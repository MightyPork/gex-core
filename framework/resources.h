//
// Created by MightyPork on 2017/11/24.
//

#ifndef GEX_RESOURCES_H
#define GEX_RESOURCES_H

#include "platform.h"
#include "unit.h"
#include "rsc_enum.h"

#if DEBUG_RSC
#define rsc_dbg(fmt, ...) dbg("[RSC] "fmt, ##__VA_ARGS__)
#else
#define rsc_dbg(fmt, ...)
#endif

#define CHECK_SUC() do { if (!suc) return false; } while (0)

void rsc_init_registry(void);

error_t rsc_claim_pin(Unit *unit, char port_name, uint8_t pin);
error_t rsc_claim(Unit *unit, Resource rsc);
error_t rsc_claim_range(Unit *unit, Resource rsc0, Resource rsc1);
/**
 * Claim GPIOs by bitmask and port name, atomically.
 * Tear down the unit on failure.
 *
 * @param unit - claiming unit
 * @param port_name - port (eg. 'A')
 * @param pins - pins, bitmask
 * @return success
 */
error_t rsc_claim_gpios(Unit *unit, char port_name, uint16_t pins);

void rsc_teardown(Unit *unit);
void rsc_free(Unit *unit, Resource rsc);
void rsc_free_range(Unit *unit, Resource rsc0, Resource rsc1);

const char * rsc_get_name(Resource rsc);

/** Get rsc owner name */
const char * rsc_get_owner_name(Resource rsc);

/** Print all available resource names */
void rsc_print_all_available(IniWriter *iw);

#endif //GEX_RESOURCES_H
