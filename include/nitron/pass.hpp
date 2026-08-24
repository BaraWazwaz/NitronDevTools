#pragma once

namespace nitron
{

#define NITRON_IN_PROGRESS                                               \
static_assert(false, "This functionality does not have implementation, " \
                     "working on this feature is still ongoing")

#define NITRON_REQUIRES_SPECIALIZATION                                       \
static_assert(false, "This functionality does not have implementation, " \
                     "provide specialization for the template to use it")

} // namespace nitron
