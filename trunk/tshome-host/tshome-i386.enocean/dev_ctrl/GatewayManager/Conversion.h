
#ifndef CONVERSION_H
#define CONVERSION_H

#include "KNXCondef.h"

#include "enocean/enocean.h"

#ifdef __cplusplus
extern "C"
{
#endif

	void protocol_set_knx_callbacks(
		OnSearchResponse search_response,
		OnReadResponse knx_read_res,
		OnWriteResponse knx_write_res,
		OnKnxConnect knx_connect,
		OnKnxDisconnect knx_disconnect,
		OnRWError error_fun);

	void protocol_set_wifi_callbacks(
			OnWifiConnect conn_fun,
			OnWifiDisconnect disconn_fun);

	void protocol_set_ctrl_response(OnCtrlResponse ctrl_response);

	// Start manager thread.
	int protocol_start_manager();

	// Stop manager thread.
	void protocol_stop_manager();

	int protocol_search_server(char *strErr);

	int protocol_add_gateway(int gateway_type, int key, char *local_ip, char* ip, int port);
	void protocol_remove_gateway(int gateway_type, int key);

	// connect all wifi and knx gateways
	// and all com ports
	int protocol_connect_all();

	// disconnect all wifi and knx gateways
	// and all com ports
	void protocol_disconnect_all();

	void protocol_remove_all_gateways();

	// web mode use
	int protocol_gateway_open(int gateway_id, int on_off);
	int protocol_is_gateway_open(int gateway_id, int *on_off);

	void protocol_connect_gateway(int gateway_type, int key);
	int protocol_connect_gateway_sync(int gateway_type, int key);
	void protocol_disconnect_gateway(int gateway_type, int key);
	int protocol_is_gateway_connected(int gateway_type, int key);

	// up level request
	int protocol_request(void * buf, int len);
	
	/* enocean interface, added by gengh on 2012/05/22 */
#ifdef DEV_ENOCEAN_SUPPORT
    void protocol_enocean_init(enocean_callback_status_t callback);
    int protocol_enocean_device_onoff(uint32 u32DeviceID, int rocker, boolean on);
#endif
	/* enocean end */


	int protocol_ifrared_open_all(int on_off);
	int protocol_ifrared_open (int channel, int on_off);
	int protocol_is_ifrared_open (int channel, int *on_off);
	int protocol_serial_open_all(int on_off);
	int protocol_serial_open(int number, int on_off);
	int protocol_is_serial_open(int number, int *on_off);

#ifdef __cplusplus
}
#endif

#endif
