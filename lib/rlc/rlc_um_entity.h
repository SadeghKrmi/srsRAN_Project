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

#include "rlc_base_entity.h"
#include "rlc_rx_um_entity.h"
#include "rlc_tx_um_entity.h"

namespace srsgnb {

class rlc_um_entity : public rlc_base_entity
{
public:
  rlc_um_entity(du_ue_index_t                        du_index,
                lcid_t                               lcid,
                const rlc_um_config&                 config,
                rlc_rx_upper_layer_data_notifier&    rx_upper_dn,
                rlc_tx_upper_layer_data_notifier&    tx_upper_dn,
                rlc_tx_upper_layer_control_notifier& tx_upper_cn,
                rlc_tx_lower_layer_notifier&         tx_lower_dn,
                timer_manager&                       timers) :
    rlc_base_entity(du_index, lcid)
  {
    tx = std::make_unique<rlc_tx_um_entity>(du_index, lcid, config.tx, tx_upper_dn, tx_upper_cn, tx_lower_dn);
    logger.log_info("RLC TX UM configured: sn_field_length={}", config.tx);

    rx = std::make_unique<rlc_rx_um_entity>(du_index, lcid, config.rx, rx_upper_dn, timers);
    logger.log_info("RLC RX UM configured: {}", config.rx);
  }
};

} // namespace srsgnb
