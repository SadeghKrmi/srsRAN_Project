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

#include "nr_cgi.h"
#include "s_nssai.h"
#include "srsgnb/adt/optional.h"

namespace srsgnb {

// See TS 38.463 Section 9.3.1.21: PDU Session ID valid values: (0..255)
constexpr static uint16_t MAX_NOF_PDU_SESSIONS = 256;

/// \brief PDU Session ID.
/// \remark See TS 38.463 Section 9.3.1.21: PDU Session ID valid values: (0..255)
enum class pdu_session_id_t : uint16_t { min = 0, max = MAX_NOF_PDU_SESSIONS - 1, invalid = MAX_NOF_PDU_SESSIONS };

/// Convert PDU Session ID type to integer.
constexpr inline uint16_t pdu_session_id_to_uint(pdu_session_id_t id)
{
  return static_cast<uint16_t>(id);
}

/// Convert integer to PDU Session ID type.
constexpr inline pdu_session_id_t uint_to_pdu_session_id(uint16_t idx)
{
  return static_cast<pdu_session_id_t>(idx);
}

// See TS 38.463 Section 9.3.1.24: QoS Flow ID valid values: (0..63)
constexpr static uint8_t MAX_NOF_QOS_FLOWS = 64;

/// \brief QoS Flow ID.
/// \remark See TS 38.463 Section 9.3.1.21: QoS Flow ID valid values: (0..63)
enum class qos_flow_id_t : uint8_t { min = 0, max = MAX_NOF_QOS_FLOWS - 1, invalid = MAX_NOF_QOS_FLOWS };

/// Convert QoS Flow ID type to integer.
constexpr inline uint8_t qos_flow_id_to_uint(qos_flow_id_t id)
{
  return static_cast<uint8_t>(id);
}

/// Convert integer to QoS Flow ID type.
constexpr inline qos_flow_id_t uint_to_qos_flow_id(uint8_t idx)
{
  return static_cast<qos_flow_id_t>(idx);
}

struct slice_support_item_t {
  s_nssai_t s_nssai;
};

struct nr_cgi_support_item_t {
  nr_cell_global_id_t nr_cgi;
};

struct non_dyn_5qi_descriptor_t {
  uint16_t           five_qi;
  optional<uint8_t>  qos_prio_level;
  optional<uint16_t> averaging_win;
  optional<uint16_t> max_data_burst_volume;
};

struct ng_ran_qos_support_item_t {
  non_dyn_5qi_descriptor_t non_dyn_5qi_descriptor;
};

struct supported_plmns_item_t {
  std::string                            plmn_id;
  std::vector<slice_support_item_t>      slice_support_list;
  std::vector<nr_cgi_support_item_t>     nr_cgi_support_list;
  std::vector<ng_ran_qos_support_item_t> ng_ran_qos_support_list;
};

struct sdap_config_t {
  pdu_session_id_t           pdu_session = pdu_session_id_t::invalid;
  std::string                sdap_hdr_dl;
  std::string                sdap_hdr_ul;
  bool                       default_drb                 = false;
  std::vector<qos_flow_id_t> mapped_qos_flows_to_add     = {};
  std::vector<qos_flow_id_t> mapped_qos_flows_to_release = {};
};

struct rohc_profiles_t {
  bool profile0x0001 = false;
  bool profile0x0002 = false;
  bool profile0x0003 = false;
  bool profile0x0004 = false;
  bool profile0x0006 = false;
  bool profile0x0101 = false;
  bool profile0x0102 = false;
  bool profile0x0103 = false;
  bool profile0x0104 = false;
};

struct rohc_t {
  rohc_profiles_t    profiles;
  bool               drb_continue_rohc_present = false;
  optional<uint16_t> max_cid;
};

struct ul_only_rohc_profiles_t {
  bool profile0x0006 = false;
};

struct ul_only_rohc_t {
  ul_only_rohc_profiles_t profiles;
  bool                    drb_continue_rohc_present = false;
  optional<uint16_t>      max_cid;
};

struct hdr_compress_t {
  optional<rohc_t>         rohc;
  optional<ul_only_rohc_t> ul_only_rohc;
};

struct drb_t {
  hdr_compress_t    hdr_compress;
  optional<int16_t> discard_timer;
  optional<uint8_t> pdcp_sn_size_ul;
  optional<uint8_t> pdcp_sn_size_dl;
  bool              integrity_protection_present   = false;
  bool              status_report_required_present = false;
  bool              out_of_order_delivery_present  = false;
};

struct primary_path_t {
  optional<uint8_t> cell_group;
  optional<uint8_t> lc_ch;
};

struct more_than_one_rlc_t {
  primary_path_t    primary_path;
  optional<int32_t> ul_data_split_thres;
  optional<bool>    pdcp_dupl;
};

struct pdcp_config_t {
  optional<drb_t>               drb;
  optional<more_than_one_rlc_t> more_than_one_rlc;
  optional<uint16_t>            t_reordering;
  bool                          ciphering_disabled_present = false;
};

struct security_result_t {
  std::string confidentiality_protection_result;
  std::string integrity_protection_result;
};

} // namespace srsgnb