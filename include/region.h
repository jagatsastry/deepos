/*
 * region.h
 *
 *  Created on: March 19, 2011
 *      Author: cds
 */

#ifndef __MM_REGION_H
#define __MM_REGION_H

#include <defs.h>

enum mm_phy_types {
	MM_PHY_UNKNOWN=0,
	MM_PHY_USABLE,
	MM_PHY_RESERVED,
	MM_PHY_ACPI_RECLAIMABLE,
	MM_PHY_ACPI_NVS,
	MM_PHY_BAD,
	MM_PHY_MAX
};

struct mm_phy_reg {

	uint64_t base;
	uint64_t len;
	uint32_t type;
} __attribute__((packed)) ;

#endif /*** __MM_REGION_H ***/

