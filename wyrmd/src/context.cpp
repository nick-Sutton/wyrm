/**
 * Copyright [2026] [Nicholas Sutton]
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 */

#include "context.hpp"
#include "net.hpp"

/**
 * Builds the Description table that maps RigidBody Id's to their Description
 * @param desc A collection of NatNet Descriptions
 * @param ctx the wyrm context that stores the descriptions map
 */
void BuildDescriptionTable(sDataDescriptions* desc, WyrmContext& ctx) {
    for (int i = 0; i < desc->nDataDescriptions; i++) {
        if (desc->arrDataDescriptions[i].type == Descriptor_RigidBody) {
            WyrmDescription d = ToWyrmDescription(
                *desc->arrDataDescriptions[i].Data.RigidBodyDescription
            );
            ctx.descriptions[d.id()] = d;
            ctx.description_writer->write(d);  // publishing description data
        }
    }
}