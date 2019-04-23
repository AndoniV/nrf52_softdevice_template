/*******************************************************************************
 @author    A Villarreal
 @date      23/04/19
 @file      ble.c
 @brief     Provides a BLE interface for appication
 ******************************************************************************/

#include "app_error.h"
#include "app_timer.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_log.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "peer_manager.h"
#include "app_error.h"
#include "ble/app_ble.h"

// DEFINITIONS ****************************************************************/

/** The advertising interval (in units of 0.625ms) */
#define APP_BLE_ADVERTISING_INTERVAL_UNITS             (300)
/** The advertising duration (in units of 10ms), set forever */
#define APP_BLE_APP_ADVERTISING_DURATION_UNITS         (0)
/** Connection interval limits */
#define APP_BLE_MIN_CONN_INTERVAL_UNITS                MSEC_TO_UNITS(100, UNIT_1_25_MS)
#define APP_BLE_MAX_CONN_INTERVAL_UNITS                MSEC_TO_UNITS(200, UNIT_1_25_MS)
/** Slave latency */
#define APP_BLE_SLAVE_LATENCY                          (0)
/** Connection supervisory timeout */
#define APP_BLE_CONN_SUPERVISORY_TIMEOUT_UNITS         MSEC_TO_UNITS(4000, UNIT_10_MS)
/** Time from initiating an event (connect or start of notification) to the
    first time sd_ble_gap_conn_param_update is called */
#define APP_BLE_FIRST_CONN_PARAMS_UPDATE_DELAY_TICKS   APP_TIMER_TICKS(5000)
/** Time between each call to sd_ble_gap_conn_param_update after the first call */
#define APP_BLE_NEXT_CONN_PARAMS_UPDATE_DELAY_TICKS    APP_TIMER_TICKS(30000)
/** Number of attempts before giving up the connection parameter negotiation */
#define APP_BLE_MAX_CONN_PARAMS_UPDATE_ATTEMPTS        (3)
/** Memory block size for queued writes. This is larger than the maximum size
    of the characteristic that will be sent (512) to handle overhead. It has
    been chosen via trial-and-error using Gluecose */
#define APP_BLE_QUEUED_WRITE_MEM_BLOCK_SIZE            (604)

// DECLARATIONS ***************************************************************/

NRF_BLE_GATT_DEF(s_app_ble_gatt_obj);
NRF_BLE_QWR_DEF(s_app_ble_queued_write_obj);
BLE_ADVERTISING_DEF(s_app_ble_advertising_obj);

/** BLE connection handle */
static uint16_t s_app_ble_connection_handle = BLE_CONN_HANDLE_INVALID;

/** Buffer to hold data received on queued writes */
static uint8_t s_app_ble_queued_write_buf[APP_BLE_QUEUED_WRITE_MEM_BLOCK_SIZE];

static void app_ble_stack_init(void);
static void app_ble_gap_init(void);
static void app_ble_gatt_init(void);
static void app_ble_services_init(void);
static void app_ble_advertising_init(void);
static void app_ble_connection_params_init(void);
static void app_ble_peer_mgr_init(void);

static void app_ble_stack_gap_evt_callback(ble_evt_t const *p_ble_evt, void *p_context);
static void app_ble_advertising_evt_callback(ble_adv_evt_t ble_adv_evt);
static void app_ble_connection_params_evt_callback(ble_conn_params_evt_t *p_evt);
static void app_ble_connection_params_error_callback(uint32_t nrf_error);
static void app_ble_queued_write_error_callback(uint32_t nrf_error);

// IMPLEMENTATION *************************************************************/

/**
 *	Initialize the module
 *	@retval ERR_SUCCESS: 	Success
 *  @retval ERR_ERROR: 		Failed to initialise
 */
AppResult app_ble_init(void)
{
	app_ble_stack_init();
	app_ble_gap_init();
	app_ble_gatt_init();
	app_ble_services_init();
	app_ble_advertising_init();
	app_ble_connection_params_init();
	app_ble_peer_mgr_init();

	NRF_LOG_INFO("[APP_BLE] Initialised");

	return ERR_SUCCESS;
}

AppResult app_ble_advertising_start()
{
    // s_gble_flags.peers_deleted = false;
    ret_code_t nrf_result;

    nrf_result = ble_advertising_start(&s_app_ble_advertising_obj, BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(nrf_result);

    NRF_LOG_INFO("[APP_BLE] Advertising started");

    return ((AppResult)nrf_result);
}

/**
 * Initialise the BLE stack, i.e., the SoftDevice and BLE event callback
 */
static void app_ble_stack_init(void)
{
	ret_code_t nrf_result;

	nrf_result = nrf_sdh_enable_request();
    APP_ERROR_CHECK(nrf_result);

    // Configure the BLE stack using the default settings. Fetch the start
    // address of the application RAM
    uint32_t ram_start = 0;
    nrf_result = nrf_sdh_ble_default_cfg_set(1, &ram_start);
    APP_ERROR_CHECK(nrf_result);

    // Enable BLE stack
    nrf_result = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(nrf_result);

    // Register a handler for BLE GAP events
    NRF_SDH_BLE_OBSERVER(s_ble_stack_gap_evt_observer, 3, app_ble_stack_gap_evt_callback, NULL);
}

/**
 * Initialise the GAP parameters
 */
static void app_ble_gap_init(void)
{
	ret_code_t nrf_result;
	const uint8_t device_name[] = "ANDONI BLE TEST";

    ble_gap_conn_sec_mode_t sec_mode;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

	nrf_result = sd_ble_gap_device_name_set(&sec_mode, device_name, sizeof(device_name));
    APP_ERROR_CHECK(nrf_result);

    nrf_result = sd_ble_gap_appearance_set(BLE_APPEARANCE_GENERIC_COMPUTER);
    APP_ERROR_CHECK(nrf_result);

    ble_gap_conn_params_t gap_conn_params = {0};
    gap_conn_params.min_conn_interval = APP_BLE_MIN_CONN_INTERVAL_UNITS;
    gap_conn_params.max_conn_interval = APP_BLE_MAX_CONN_INTERVAL_UNITS;
    gap_conn_params.slave_latency = APP_BLE_SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout = APP_BLE_CONN_SUPERVISORY_TIMEOUT_UNITS;
    nrf_result = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(nrf_result);
}

/**
 * Initialise the GATT
 */
static void app_ble_gatt_init(void)
{
    ret_code_t nrf_result = nrf_ble_gatt_init(&s_app_ble_gatt_obj, NULL);
    APP_ERROR_CHECK(nrf_result);
}

/**
 * Initialise all services
 */
static void app_ble_services_init(void)
{
    // Initialise Queued Write Module used by services
    nrf_ble_qwr_init_t qwr_init = {0};
    qwr_init.error_handler = app_ble_queued_write_error_callback;
    qwr_init.mem_buffer.len = APP_BLE_QUEUED_WRITE_MEM_BLOCK_SIZE;
    qwr_init.mem_buffer.p_mem = s_app_ble_queued_write_buf;
    qwr_init.callback = NULL;
    ret_code_t nrf_result = nrf_ble_qwr_init(&s_app_ble_queued_write_obj, &qwr_init);
    APP_ERROR_CHECK(nrf_result);

    // Initialise custom services
}

/**
 * Initialise advertising
 */
static void app_ble_advertising_init(void)
{
    ble_advertising_init_t init = {0};
    init.advdata.name_type = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance = false;
    init.advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    init.config.ble_adv_fast_enabled = true;
    init.config.ble_adv_fast_interval = APP_BLE_ADVERTISING_INTERVAL_UNITS;
    init.config.ble_adv_fast_timeout = APP_BLE_APP_ADVERTISING_DURATION_UNITS;
    init.evt_handler = app_ble_advertising_evt_callback;
    ret_code_t nrf_result = ble_advertising_init(&s_app_ble_advertising_obj, &init);
    APP_ERROR_CHECK(nrf_result);
    ble_advertising_conn_cfg_tag_set(&s_app_ble_advertising_obj, 1);
}

/**
 * Initialise connection parameters
 */
static void app_ble_connection_params_init(void)
{
    ble_conn_params_init_t cp_init = {0};
    cp_init.p_conn_params = NULL;
    cp_init.first_conn_params_update_delay = APP_BLE_FIRST_CONN_PARAMS_UPDATE_DELAY_TICKS;
    cp_init.next_conn_params_update_delay = APP_BLE_NEXT_CONN_PARAMS_UPDATE_DELAY_TICKS;
    cp_init.max_conn_params_update_count = APP_BLE_MAX_CONN_PARAMS_UPDATE_ATTEMPTS;
    cp_init.start_on_notify_cccd_handle = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail = false;
    cp_init.evt_handler = app_ble_connection_params_evt_callback;
    cp_init.error_handler = app_ble_connection_params_error_callback;
    ret_code_t nrf_result = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(nrf_result);
}

/**
 * Initialise the peer manager
 */
static void app_ble_peer_mgr_init(void)
{
    ret_code_t nrf_result = pm_init();
    APP_ERROR_CHECK(nrf_result);
}

// CALLBACKS ******************************************************************/

/**
 * Called when a BLE stack GAP event occurs for the application
 */
static void app_ble_stack_gap_evt_callback(ble_evt_t const *p_ble_evt, void *p_context)
{
    ret_code_t nrf_result = NRF_SUCCESS;
    switch (p_ble_evt->header.evt_id)
    {
    // ========================================================================/
    case BLE_GAP_EVT_DISCONNECTED:
        NRF_LOG_INFO("[APP_BLE] Stack event received: BLE_GAP_EVT_DISCONNECTED");
        break;
    // ========================================================================/
    case BLE_GAP_EVT_CONNECTED:
        NRF_LOG_INFO("[APP_BLE] Stack event received: BLE_GAP_EVT_CONNECTED");
        // nrf_result = bsp_indication_set(BSP_INDICATE_CONNECTED);
        APP_ERROR_CHECK(nrf_result);
        // s_gble_connection_handle = p_ble_evt->evt.gap_evt.conn_handle;
        // nrf_result = nrf_ble_qwr_conn_handle_assign(&s_gble_queued_write_obj, s_gble_connection_handle);
        // APP_ERROR_CHECK(nrf_result);
        break;
    // ========================================================================/
    case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
    {
        NRF_LOG_INFO("[APP_BLE] Stack event received: BLE_GAP_EVT_PHY_UPDATE_REQUEST");
        // ble_gap_phys_t const phys =
        // {
        //     .rx_phys = BLE_GAP_PHY_AUTO,
        //     .tx_phys = BLE_GAP_PHY_AUTO,
        // };
        // nrf_result = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
        // APP_ERROR_CHECK(nrf_result);
        break;
    }
    // ========================================================================/
    case BLE_GATTC_EVT_TIMEOUT:
        NRF_LOG_INFO("[APP_BLE] Stack event received: BLE_GATTC_EVT_TIMEOUT");

        // Disconnect on GATT Client timeout event
        // nrf_result = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        // APP_ERROR_CHECK(nrf_result);
        break;
    // ========================================================================/
    case BLE_GATTS_EVT_TIMEOUT:
        NRF_LOG_INFO("[APP_BLE] Stack event received: BLE_GATTS_EVT_TIMEOUT");

        // Disconnect on GATT Server timeout event
        // nrf_result = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        // APP_ERROR_CHECK(nrf_result);
        break;
    // ========================================================================/
    default:
        break;
    } // switch: event ID
}

/**
 * Called when a BLE advertising event occurs for the application
 */
static void app_ble_advertising_evt_callback(ble_adv_evt_t ble_adv_evt)
{
    ret_code_t nrf_result = NRF_SUCCESS;
    switch (ble_adv_evt)
    {
    // ========================================================================/
    case BLE_ADV_EVT_FAST:
        NRF_LOG_INFO("[APP_BLE] Advertising event received: BLE_ADV_EVT_FAST");
        // nrf_result = bsp_indication_set(BSP_INDICATE_ADVERTISING);
        APP_ERROR_CHECK(nrf_result);
        break;
    // ========================================================================/
    case BLE_ADV_EVT_IDLE:
        NRF_LOG_INFO("[APP_BLE] Advertising event received: BLE_ADV_EVT_IDLE");
        // TODO EK 17/12/18: Sleep?
        break;
    // ========================================================================/
    default:
        break;
    }
}

/**
 * Called when an event occurs in the Connection Parameters Module for the
 * application. This is only used for error handling during initialisation
 */
static void app_ble_connection_params_evt_callback(ble_conn_params_evt_t *p_evt)
{
    switch (p_evt->evt_type)
    {
    // ========================================================================/
    case BLE_CONN_PARAMS_EVT_SUCCEEDED:
        NRF_LOG_INFO("[GBLE] Connection parameters event received: BLE_CONN_PARAMS_EVT_SUCCEEDED");
        break;
    // ========================================================================/
    case BLE_CONN_PARAMS_EVT_FAILED:
        NRF_LOG_INFO("[GBLE] Connection parameters event received: BLE_CONN_PARAMS_EVT_FAILED");
        ret_code_t nrf_result = sd_ble_gap_disconnect(s_app_ble_connection_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(nrf_result);
        break;
    // ========================================================================/
    default:
        break;
    } // switch: event type
}

/**
 * Called when an error occurs in the Connection Parameters Module
 */
static void app_ble_connection_params_error_callback(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**
 * Called when an error occurs in the Queued Write Module
 */
static void app_ble_queued_write_error_callback(uint32_t nrf_error)
{
}
