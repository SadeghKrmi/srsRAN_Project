/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsgnb {

/// \c maxNrofTAGs, TS 38.331.
const unsigned MAX_NOF_TAGS = 4;

/// \c TAG-Id, TS 38.331.
enum tag_id_t { MAX_TAG_ID = 3, INVALID_TAG_ID = MAX_NOF_TAGS };

/// Converts a uint into a \ref enum tag_id_t .
inline tag_id_t uint_to_tag_id(unsigned tag_id)
{
  return static_cast<tag_id_t>(tag_id);
}

/// \c TimeAlignmentTimer, TS 38.331.
enum class time_alignment_timer {
  ms500    = 500,
  ms750    = 750,
  ms1280   = 1280,
  ms1920   = 1920,
  ms2560   = 2560,
  ms5120   = 5120,
  ms10240  = 10240,
  infinity = 0
};

/// Return the value of \ref time_alignment_timer.
inline std::underlying_type<time_alignment_timer>::type time_alignemnt_timer_to_value(time_alignment_timer timer)
{
  return static_cast<std::underlying_type<periodic_bsr_timer>::type>(timer);
}

/// \c TAG, or Time Alignment Group, TS 38.331.
struct tag {
  tag_id_t             tag_id;
  time_alignment_timer ta_timer;
};

} // namespace srsgnb
