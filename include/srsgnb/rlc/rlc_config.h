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

#include "srsgnb/support/srsgnb_assert.h"
#include <cstdint>
#include <memory>
#include <string>

namespace srsgnb {

/// RLC NR modes
enum class rlc_mode { tm, um_bidir, um_unidir_ul, um_unidir_dl, am };

/// RLC UM NR sequence number field
enum class rlc_um_sn_size : uint16_t { size6bits = 6, size12bits = 12 };
constexpr uint16_t to_number(rlc_um_sn_size sn_size)
{
  return static_cast<uint16_t>(sn_size);
}

/// RLC AM NR sequence number field
enum class rlc_am_sn_size : uint16_t { size12bits = 12, size18bits = 18 };

/// \brief Converts sequence number field to numeric its value
/// \param sn_size sequence number size
/// \return numeric value of the sequence number field
constexpr uint16_t to_number(rlc_am_sn_size sn_size)
{
  return static_cast<uint16_t>(sn_size);
}

/// \brief Returns the value range of the sequence numbers
/// \param sn_size Length of the sequence number field in bits
/// \return cardinality of sn_size
constexpr uint32_t cardinality(uint16_t sn_size)
{
  srsgnb_assert(sn_size < 32, "Cardinality of SN size {} bit exceeds return type 'uint32_t'", sn_size);
  return (1 << sn_size);
}

/// \brief Returns the UM_Window_Size and AM_Window_Size
/// Ref: 3GPP TS 38.322 Sec. 7.2
/// \param sn_size Length of the sequence number field in bits
/// \return size of the window
constexpr uint32_t window_size(uint16_t sn_size)
{
  return cardinality(sn_size - 1);
}

/// RLC AM NR segmentation info
enum class rlc_dc_field : unsigned { control = 0b00, data = 0b01 };
constexpr unsigned to_number(rlc_dc_field dc)
{
  return static_cast<unsigned>(dc);
}

/// RLC AM NR segmentation info
enum class rlc_si_field : unsigned {
  full_sdu       = 0b00,
  first_segment  = 0b01,
  last_segment   = 0b10,
  middle_segment = 0b11
};

constexpr uint16_t to_number(rlc_si_field si_field)
{
  return static_cast<uint16_t>(si_field);
}

enum class rlc_control_pdu_type : unsigned { status_pdu = 0b000 };
constexpr uint16_t to_number(rlc_control_pdu_type type)
{
  return static_cast<uint16_t>(type);
}

/// \brief Configurable Rx parameters for RLC AM
/// Ref: 3GPP TS 38.322 Section 7
struct rlc_rx_am_config {
  rlc_am_sn_size sn_field_length; ///< Number of bits used for sequence number

  // Timers Ref: 3GPP TS 38.322 Section 7.3
  int32_t t_reassembly;      ///< Timer used by rx to detect PDU loss (ms)
  int32_t t_status_prohibit; ///< Timer used by rx to prohibit tx of status PDU (ms)
};

/// \brief Configurable Tx parameters for RLC AM
/// Ref: 3GPP TS 38.322 Section 7
struct rlc_tx_am_config {
  rlc_am_sn_size sn_field_length; ///< Number of bits used for sequence number

  // Timers Ref: 3GPP TS 38.322 Section 7.3
  int32_t t_poll_retx; ///< Poll retx timeout (ms)

  // Configurable Parameters. Ref: 3GPP TS 38.322 Section 7.4
  uint32_t max_retx_thresh; ///< Max number of retx
  int32_t  poll_pdu;        ///< Insert poll bit after this many PDUs
  int32_t  poll_byte;       ///< Insert poll bit after this much data (KB)
};

/// \brief Configurable parameters for RLC AM
/// Ref: 3GPP TS 38.322 Section 7
struct rlc_am_config {
  rlc_rx_am_config rx; ///< Rx configuration
  rlc_tx_am_config tx; ///< Tx configuration
};

/// \brief Configurable Rx parameters for RLC UM
/// Ref: 3GPP TS 38.322 v15.3.0 Section 7
struct rlc_rx_um_config {
  rlc_um_sn_size sn_field_length; ///< Number of bits used for sequence number
  int32_t        t_reassembly_ms; ///< Timer used by rx to detect PDU loss (ms)
};

/// \brief Configurable Tx parameters for RLC UM
/// Ref: 3GPP TS 38.322 v15.3.0 Section 7
struct rlc_tx_um_config {
  rlc_um_sn_size sn_field_length; ///< Number of bits used for sequence number
};

/// \brief Configurable parameters for RLC UM
/// Ref: 3GPP TS 38.322 v15.3.0 Section 7
struct rlc_um_config {
  rlc_rx_um_config rx; ///< Rx configuration
  rlc_tx_um_config tx; ///< Tx configuration
};

/// Configuration of RLC bearer.
struct rlc_config {
  rlc_mode      mode;
  rlc_um_config um;
  rlc_am_config am;
};

} // namespace srsgnb

namespace fmt {

template <>
struct formatter<srsgnb::rlc_mode> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsgnb::rlc_mode mode, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    constexpr static const char* options[] = {"TM", "UM Bi-dir", "UM Uni-dir-UL", "UM Uni-dir-DL", "AM"};
    return format_to(ctx.out(), "{}", options[static_cast<unsigned>(mode)]);
  }
};

template <>
struct formatter<srsgnb::rlc_um_sn_size> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsgnb::rlc_um_sn_size sn_size, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{} bit", to_number(sn_size));
  }
};

template <>
struct formatter<srsgnb::rlc_am_sn_size> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsgnb::rlc_am_sn_size sn_size, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{} bit", to_number(sn_size));
  }
};

template <>
struct formatter<srsgnb::rlc_dc_field> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsgnb::rlc_dc_field dc, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    constexpr static const char* options[] = {"Control PDU", "Data PDU"};
    return format_to(ctx.out(), "{}", options[to_number(dc)]);
  }
};

template <>
struct formatter<srsgnb::rlc_si_field> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsgnb::rlc_si_field si, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    constexpr static const char* options[] = {"full", "first", "last", "middle"};
    return format_to(ctx.out(), "{}", options[to_number(si)]);
  }
};

template <>
struct formatter<srsgnb::rlc_control_pdu_type> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsgnb::rlc_control_pdu_type type, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    constexpr static const char* options[] = {"Control PDU"};
    return format_to(ctx.out(), "{}", options[to_number(type)]);
  }
};

// RLC UM TX config formatter
template <>
struct formatter<srsgnb::rlc_tx_um_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsgnb::rlc_tx_um_config cfg, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "sn_field_length={}", cfg.sn_field_length);
  }
};

// RLC UM RX config formatter
template <>
struct formatter<srsgnb::rlc_rx_um_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsgnb::rlc_rx_um_config cfg, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "sn_field_length={}, t_reassembly_ms={}", cfg.sn_field_length, cfg.t_reassembly_ms);
  }
};

// RLC AM TX config formatter
template <>
struct formatter<srsgnb::rlc_tx_am_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsgnb::rlc_tx_am_config cfg, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "sn_field_length={}, t_poll_retx={}, max_retx_thresh={}, poll_pdu={}, poll_byte={}",
                     cfg.sn_field_length,
                     cfg.t_poll_retx,
                     cfg.max_retx_thresh,
                     cfg.poll_pdu,
                     cfg.poll_byte);
  }
};

// RLC AM RX config formatter
template <>
struct formatter<srsgnb::rlc_rx_am_config> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsgnb::rlc_rx_am_config cfg, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "sn_field_length={}, t_reassembly={}, t_status_prohibit={}",
                     cfg.sn_field_length,
                     cfg.t_reassembly,
                     cfg.t_status_prohibit);
  }
};
} // namespace fmt
