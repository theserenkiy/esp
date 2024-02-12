/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_log.h"

#include "esp_bt.h"
#include "bt_app_core.h"
#include "bt_app_av.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"
#include "esp_a2dp_api.h"
#include "esp_avrc_api.h"

#include "driver/dac_continuous.h"

/* device name */
#define LOCAL_DEVICE_NAME    "ESP_SPEAKER"
#define USE_INTERNAL_DAC 	1

static QueueHandle_t s_bt_app_task_queue = NULL;  /* handle of work queue */
static TaskHandle_t s_bt_app_task_handle = NULL;  /* handle of application task  */

/* event for stack up */
enum {
	BT_APP_EVT_STACK_UP = 0,
};

/********************************
 * STATIC FUNCTION DECLARATIONS
 *******************************/

/* GAP callback function */
static void bt_app_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param);
/* handler for bluetooth stack enabled events */
static void bt_av_hdl_stack_evt(uint16_t event, void *p_param);

/*******************************
 * STATIC FUNCTION DEFINITIONS
 ******************************/

static void bt_app_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
	uint8_t *bda = NULL;

	switch (event) {
	/* when authentication completed, this event comes */
		case ESP_BT_GAP_AUTH_CMPL_EVT: {
			if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
				ESP_LOGI(BT_AV_TAG, "authentication success: %s", param->auth_cmpl.device_name);
				esp_log_buffer_hex(BT_AV_TAG, param->auth_cmpl.bda, ESP_BD_ADDR_LEN);
			} else {
				ESP_LOGE(BT_AV_TAG, "authentication failed, status: %d", param->auth_cmpl.stat);
			}
			break;
		}

		/* when GAP mode changed, this event comes */
		case ESP_BT_GAP_MODE_CHG_EVT:
			ESP_LOGI(BT_AV_TAG, "ESP_BT_GAP_MODE_CHG_EVT mode: %d", param->mode_chg.mode);
			break;
		/* when ACL connection completed, this event comes */
		case ESP_BT_GAP_ACL_CONN_CMPL_STAT_EVT:
			bda = (uint8_t *)param->acl_conn_cmpl_stat.bda;
			ESP_LOGI(BT_AV_TAG, "ESP_BT_GAP_ACL_CONN_CMPL_STAT_EVT Connected to [%02x:%02x:%02x:%02x:%02x:%02x], status: 0x%x",
					bda[0], bda[1], bda[2], bda[3], bda[4], bda[5], param->acl_conn_cmpl_stat.stat);
			break;
		/* when ACL disconnection completed, this event comes */
		case ESP_BT_GAP_ACL_DISCONN_CMPL_STAT_EVT:
			bda = (uint8_t *)param->acl_disconn_cmpl_stat.bda;
			ESP_LOGI(BT_AV_TAG, "ESP_BT_GAP_ACL_DISC_CMPL_STAT_EVT Disconnected from [%02x:%02x:%02x:%02x:%02x:%02x], reason: 0x%x",
					bda[0], bda[1], bda[2], bda[3], bda[4], bda[5], param->acl_disconn_cmpl_stat.reason);
			break;
		/* others */
		default: {
			ESP_LOGI(BT_AV_TAG, "event: %d", event);
			break;
		}
	}
}

static void bt_av_hdl_a2d_evt(uint16_t event, void *p_param)
{
	ESP_LOGD(BT_AV_TAG, "%s event: %d", __func__, event);

	esp_a2d_cb_param_t *a2d = NULL;

	switch (event) {
	/* when connection state changed, this event comes */
	case ESP_A2D_CONNECTION_STATE_EVT: {
		a2d = (esp_a2d_cb_param_t *)(p_param);
		uint8_t *bda = a2d->conn_stat.remote_bda;
		ESP_LOGI(BT_AV_TAG, "A2DP connection state: %s, [%02x:%02x:%02x:%02x:%02x:%02x]",
			s_a2d_conn_state_str[a2d->conn_stat.state], bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
		if (a2d->conn_stat.state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
			esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
			bt_i2s_driver_uninstall();
			bt_i2s_task_shut_down();
		} else if (a2d->conn_stat.state == ESP_A2D_CONNECTION_STATE_CONNECTED){
			esp_bt_gap_set_scan_mode(ESP_BT_NON_CONNECTABLE, ESP_BT_NON_DISCOVERABLE);
			bt_i2s_task_start_up();
		} else if (a2d->conn_stat.state == ESP_A2D_CONNECTION_STATE_CONNECTING) {
			bt_i2s_driver_install();
		}
		break;
	}
	/* when audio stream transmission state changed, this event comes */
	case ESP_A2D_AUDIO_STATE_EVT: {
		a2d = (esp_a2d_cb_param_t *)(p_param);
		ESP_LOGI(BT_AV_TAG, "A2DP audio state: %s", s_a2d_audio_state_str[a2d->audio_stat.state]);
		s_audio_state = a2d->audio_stat.state;
		if (ESP_A2D_AUDIO_STATE_STARTED == a2d->audio_stat.state) {
			s_pkt_cnt = 0;
		}
		break;
	}
	/* when audio codec is configured, this event comes */
	case ESP_A2D_AUDIO_CFG_EVT: {
		a2d = (esp_a2d_cb_param_t *)(p_param);
		ESP_LOGI(BT_AV_TAG, "A2DP audio stream configuration, codec type: %d", a2d->audio_cfg.mcc.type);
		/* for now only SBC stream is supported */
		if (a2d->audio_cfg.mcc.type == ESP_A2D_MCT_SBC) {
			int sample_rate = 16000;
			int ch_count = 2;
			char oct0 = a2d->audio_cfg.mcc.cie.sbc[0];
			if (oct0 & (0x01 << 6)) {
				sample_rate = 32000;
			} else if (oct0 & (0x01 << 5)) {
				sample_rate = 44100;
			} else if (oct0 & (0x01 << 4)) {
				sample_rate = 48000;
			}

			if (oct0 & (0x01 << 3)) {
				ch_count = 1;
			}
		#if USE_INTERNAL_DAC
			dac_continuous_disable(tx_chan);
			dac_continuous_del_channels(tx_chan);
			dac_continuous_config_t cont_cfg = {
				.chan_mask = DAC_CHANNEL_MASK_ALL,
				.desc_num = 8,
				.buf_size = 2048,
				.freq_hz = sample_rate,
				.offset = 127,
				.clk_src = DAC_DIGI_CLK_SRC_DEFAULT,   // Using APLL as clock source to get a wider frequency range
				.chan_mode = (ch_count == 1) ? DAC_CHANNEL_MODE_SIMUL : DAC_CHANNEL_MODE_ALTER,
			};
			/* Allocate continuous channels */
			dac_continuous_new_channels(&cont_cfg, &tx_chan);
			/* Enable the continuous channels */
			dac_continuous_enable(tx_chan);
		#else
			i2s_channel_disable(tx_chan);
			i2s_std_clk_config_t clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(sample_rate);
			i2s_std_slot_config_t slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, ch_count);
			i2s_channel_reconfig_std_clock(tx_chan, &clk_cfg);
			i2s_channel_reconfig_std_slot(tx_chan, &slot_cfg);
			i2s_channel_enable(tx_chan);
		#endif
			ESP_LOGI(BT_AV_TAG, "Configure audio player: %x-%x-%x-%x",
					 a2d->audio_cfg.mcc.cie.sbc[0],
					 a2d->audio_cfg.mcc.cie.sbc[1],
					 a2d->audio_cfg.mcc.cie.sbc[2],
					 a2d->audio_cfg.mcc.cie.sbc[3]);
			ESP_LOGI(BT_AV_TAG, "Audio player configured, sample rate: %d", sample_rate);
		}
		break;
	}
	/* when a2dp init or deinit completed, this event comes */
	case ESP_A2D_PROF_STATE_EVT: {
		a2d = (esp_a2d_cb_param_t *)(p_param);
		if (ESP_A2D_INIT_SUCCESS == a2d->a2d_prof_stat.init_state) {
			ESP_LOGI(BT_AV_TAG, "A2DP PROF STATE: Init Complete");
		} else {
			ESP_LOGI(BT_AV_TAG, "A2DP PROF STATE: Deinit Complete");
		}
		break;
	}
	/* When protocol service capabilities configured, this event comes */
	case ESP_A2D_SNK_PSC_CFG_EVT: {
		a2d = (esp_a2d_cb_param_t *)(p_param);
		ESP_LOGI(BT_AV_TAG, "protocol service capabilities configured: 0x%x ", a2d->a2d_psc_cfg_stat.psc_mask);
		if (a2d->a2d_psc_cfg_stat.psc_mask & ESP_A2D_PSC_DELAY_RPT) {
			ESP_LOGI(BT_AV_TAG, "Peer device support delay reporting");
		} else {
			ESP_LOGI(BT_AV_TAG, "Peer device unsupport delay reporting");
		}
		break;
	}
	/* when set delay value completed, this event comes */
	case ESP_A2D_SNK_SET_DELAY_VALUE_EVT: {
		a2d = (esp_a2d_cb_param_t *)(p_param);
		if (ESP_A2D_SET_INVALID_PARAMS == a2d->a2d_set_delay_value_stat.set_state) {
			ESP_LOGI(BT_AV_TAG, "Set delay report value: fail");
		} else {
			ESP_LOGI(BT_AV_TAG, "Set delay report value: success, delay_value: %u * 1/10 ms", a2d->a2d_set_delay_value_stat.delay_value);
		}
		break;
	}
	/* when get delay value completed, this event comes */
	case ESP_A2D_SNK_GET_DELAY_VALUE_EVT: {
		a2d = (esp_a2d_cb_param_t *)(p_param);
		ESP_LOGI(BT_AV_TAG, "Get delay report value: delay_value: %u * 1/10 ms", a2d->a2d_get_delay_value_stat.delay_value);
		/* Default delay value plus delay caused by application layer */
		esp_a2d_sink_set_delay_value(a2d->a2d_get_delay_value_stat.delay_value + APP_DELAY_VALUE);
		break;
	}
	/* others */
	default:
		ESP_LOGE(BT_AV_TAG, "%s unhandled event: %d", __func__, event);
		break;
	}
}

void bt_app_task(void *arg)
{
	bt_app_msg_t msg;

	for (;;) {
		/* receive message from work queue and handle it */
		if (pdTRUE == xQueueReceive(s_bt_app_task_queue, &msg, (TickType_t)portMAX_DELAY)) {
			ESP_LOGD(BT_APP_CORE_TAG, "%s, signal: 0x%x, event: 0x%x", __func__, msg.sig, msg.event);

			switch (msg.sig) {
			case BT_APP_SIG_WORK_DISPATCH:
				bt_app_work_dispatched(&msg);
				break;
			default:
				ESP_LOGW(BT_APP_CORE_TAG, "%s, unhandled signal: %d", __func__, msg.sig);
				break;
			} /* switch (msg.sig) */

			if (msg.param) {
				free(msg.param);
			}
		}
	}
}

/*******************************
 * MAIN ENTRY POINT
 ******************************/

void app_main(void)
{
	/* initialize NVS — it is used to store PHY calibration data */
	nvs_flash_init();
	esp_bt_controller_mem_release(ESP_BT_MODE_BLE);

	esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
	esp_bt_controller_init(&bt_cfg);
	esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT);
	esp_bluedroid_init();
	esp_bluedroid_enable();
	
	/* set default parameters for Legacy Pairing (use fixed pin code 1234) */
	esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_FIXED;
	esp_bt_pin_code_t pin_code;
	pin_code[0] = '1';
	pin_code[1] = '2';
	pin_code[2] = '3';
	pin_code[3] = '4';
	esp_bt_gap_set_pin(pin_type, 4, pin_code);

	s_bt_app_task_queue = xQueueCreate(10, sizeof(bt_app_msg_t));
	xTaskCreate(bt_app_task, "BtAppTask", 3072, NULL, 10, &s_bt_app_task_handle);

	/* bluetooth device name, connection mode and profile set up */
	//bt_app_work_dispatch(bt_av_hdl_stack_evt, BT_APP_EVT_STACK_UP, NULL, 0, NULL);

	esp_bt_dev_set_device_name(LOCAL_DEVICE_NAME);
	esp_bt_gap_register_callback(bt_app_gap_cb);

	// assert(esp_avrc_ct_init() == ESP_OK);
	// esp_avrc_ct_register_callback(bt_app_rc_ct_cb);
	// assert(esp_avrc_tg_init() == ESP_OK);
	// esp_avrc_tg_register_callback(bt_app_rc_tg_cb);

	// esp_avrc_rn_evt_cap_mask_t evt_set = {0};
	// esp_avrc_rn_evt_bit_mask_operation(ESP_AVRC_BIT_MASK_OP_SET, &evt_set, ESP_AVRC_RN_VOLUME_CHANGE);
	// assert(esp_avrc_tg_set_rn_evt_cap(&evt_set) == ESP_OK);

	esp_a2d_sink_init();
	esp_a2d_register_callback(&bt_app_a2d_cb);
	esp_a2d_sink_register_data_callback(bt_app_a2d_data_cb);

	/* Get the default value of the delay value */
	esp_a2d_sink_get_delay_value();

	/* set discoverable and connectable mode, wait to be connected */
	esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
}
