//
// Created by MightyPork on 2017/11/24.
//

#include "platform.h"
#include "unit.h"
#include "resources.h"

static bool rsc_initialized = false;

// This takes quite a lot of space, we could use u8 and IDs instead if needed
struct resouce_slot {
    const char *name;
    Unit *owner;
} __attribute__((packed));

static struct resouce_slot resources[R_RESOURCE_COUNT];

// here are the resource names for better debugging (could also be removed if absolutely necessary)
const char *const rsc_names[] = {
#define X(res_name) #res_name,
    XX_RESOURCES
#undef X
};

/**
 * Initialize the resources registry
 */
void rsc_init_registry(void)
{
    for (int i = 0; i < R_RESOURCE_COUNT; i++) {
        resources[i].owner = &UNIT_PLATFORM;
    }
    rsc_initialized = true;
}

/**
 * Claim a resource for a unit
 *
 * @param unit - claiming unit
 * @param rsc - resource to claim
 * @return true on successful claim
 */
bool rsc_claim(Unit *unit, Resource rsc)
{
    assert_param(rsc_initialized);
    assert_param(rsc > R_NONE && rsc < R_RESOURCE_COUNT);
    assert_param(unit != NULL);

    if (resources[rsc].owner) {
        //TODO properly report to user
        dbg("ERROR!! Unit %s failed to claim resource %s, already held by %s!",
            unit->name, rsc_names[rsc], resources[rsc].owner->name);

        unit->status = E_RESOURCE_NOT_AVAILABLE;
        return false;
    }

    resources[rsc].owner = unit;
    return true;
}

/**
 * Claim a range of resources for a unit (useful for GPIO)
 *
 * @param unit - claiming unit
 * @param rsc0 - first resource to claim
 * @param rsc1 - last resource to claim
 * @return true on complete claim, false if any failed (none are claimed in that case)
 */
bool rsc_claim_range(Unit *unit, Resource rsc0, Resource rsc1)
{
    assert_param(rsc_initialized);
    assert_param(rsc0 > R_NONE && rsc0 < R_RESOURCE_COUNT);
    assert_param(rsc1 > R_NONE && rsc1 < R_RESOURCE_COUNT);
    assert_param(unit != NULL);

    for (int i = rsc0; i <= rsc1; i++) {
        if (!rsc_claim(unit, (Resource) i)) return false;
    }

    return true;
}

/**
 * Free a resource for other use
 *
 * @param unit - owning unit; if not null, free only resources claimed by this unit
 * @param rsc - resource to free
 */
void rsc_free(Unit *unit, Resource rsc)
{
    assert_param(rsc_initialized);
    assert_param(rsc > R_NONE && rsc < R_RESOURCE_COUNT);

    if (unit == NULL || resources[rsc].owner == unit) {
        resources[rsc].owner = NULL;
    }
}

/**
 * Free a range of resources (useful for GPIO)
 *
 * @param unit - owning unit; if not null, free only resources claimed by this unit
 * @param rsc0 - first resource to free
 * @param rsc1 - last resource to free
 */
void rsc_free_range(Unit *unit, Resource rsc0, Resource rsc1)
{
    assert_param(rsc_initialized);
    assert_param(rsc0 > R_NONE && rsc0 < R_RESOURCE_COUNT);
    assert_param(rsc1 > R_NONE && rsc1 < R_RESOURCE_COUNT);

    for (int i = rsc0; i <= rsc1; i++) {
        if (unit == NULL || resources[i].owner == unit) {
            resources[i].owner = NULL;
        }
    }
}

/**
 * Tear down a unit - release all resources owned by the unit
 *
 * @param unit - unit to tear down; free only resources claimed by this unit
 */
void rsc_teardown(Unit *unit)
{
    assert_param(rsc_initialized);
    assert_param(unit != NULL);

    for (int i = R_NONE+1; i < R_RESOURCE_COUNT; i++) {
        if (resources[i].owner == unit) {
            resources[i].owner = NULL;
        }
    }
}
