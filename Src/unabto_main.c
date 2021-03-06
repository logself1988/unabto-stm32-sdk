/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <ctype.h>

#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "string.h"
#include <modules/util/read_hex.h>
#include "unabto_main.h"
#include "cmsis_os.h"
#include "unabto_main.h"
#include "demo_application.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define NABTO_THREAD_PRIO    ( tskIDLE_PRIORITY + 4 )

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t display_state = 1;
bool do_factory_reset = false;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  uNabto server thread
  * @param arg: None
  * @retval None
  */
static void unabto_thread()
{
  const char* device_id = "<DEVICE ID>";
  const char* pre_shared_key = "<KEY>";

  // Init uNabto
  nabto_main_setup* nms = unabto_init_context();
  nms->id = strdup(device_id);

  nms->secureAttach = true;
  nms->secureData = true;
  nms->cryptoSuite = CRYPT_W_AES_CBC_HMAC_SHA256;

  if (!unabto_read_psk_from_hex(pre_shared_key, nms->presharedKey, 16)) {
    NABTO_LOG_ERROR(("Invalid cryptographic key specified", pre_shared_key));
    return;
  }

  if (!unabto_init()) {
    NABTO_LOG_FATAL(("Failed at nabto_main_init"));
  }

  // Init demo application
  demo_init(do_factory_reset);
  demo_application_set_device_name("STM32F746G-DISCO");
  demo_application_set_device_product("ACME 9002 Heatpump");
  demo_application_set_device_icon_("img/chip-small.png");

  // Main loop
  for (;;) {
    unabto_tick();
    osDelay(10);
    demo_application_tick();
  }
}

/**
  * @brief  Start the uNabto server
  * @param  None
  * @retval None
  */
void unabto_start()
{
  sys_thread_new("uNabto", unabto_thread, NULL, DEFAULT_THREAD_STACKSIZE, NABTO_THREAD_PRIO);
}

/**
  * @brief  Reset all AppMyProduct settings to factory default when
  *         unabto thread is started.
  * @param  None
  * @retval None
  */
void unabto_do_factory_reset()
{
  do_factory_reset = true;
}

