
#include "du_manager_impl.h"
#include "procedures/initial_du_setup_procedure.h"

namespace srsgnb {

du_manager_impl::du_manager_impl(const du_manager_config_t& cfg_) : cfg(cfg_), ue_mng(cfg), main_ctrl_loop(128)
{
  // start F1 setup procedure
  main_ctrl_loop.schedule<initial_du_setup_procedure>(cfg, du_ctx);
}

void du_manager_impl::handle_ul_ccch_indication(const ul_ccch_indication_message& msg)
{
  // Switch DU Manager exec context
  cfg.du_mng_exec->execute([this, msg = std::move(msg)]() {
    // Start UE create procedure
    ue_mng.handle_ue_create_request(msg);
  });
}

size_t du_manager_impl::nof_ues()
{
  // TODO: This is temporary code.
  static std::mutex              mutex;
  static std::condition_variable cvar;
  size_t                         result = MAX_DU_NOF_UES;
  cfg.du_mng_exec->execute([this, &result]() {
    std::unique_lock<std::mutex> lock(mutex);
    result = ue_mng.get_ues().size();
    cvar.notify_one();
  });
  {
    std::unique_lock<std::mutex> lock(mutex);
    while (result == MAX_DU_NOF_UES) {
      cvar.wait(lock);
    }
  }
  return result;
}

} // namespace srsgnb
