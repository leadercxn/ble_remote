/* Copyright (c) 2016 SENSORO Co.,Ltd. All Rights Reserved.
 *
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "ble_srv_common.h"
#include "sdk_common.h"

#include "app_error.h"

#include "ble_strm.h"

static bool is_cccd_configured(ble_strm_t* p_strm, uint16_t cccd_handle)
{
    // Check if the CCCDs are configured.
    uint8_t  cccd_val_buf[BLE_CCCD_VALUE_LEN];
    ble_gatts_value_t gatts_value;

    // Initialize value struct.
    memset(&gatts_value, 0, sizeof(gatts_value));

    gatts_value.len     = BLE_CCCD_VALUE_LEN;
    gatts_value.offset  = 0;
    gatts_value.p_value = cccd_val_buf;

    uint32_t err_code = sd_ble_gatts_value_get(p_strm->conn_handle,
                                               cccd_handle,
                                               &gatts_value);
    if(err_code != NRF_SUCCESS)
    {
        if(p_strm->error_handler != NULL)
        {
            p_strm->error_handler(err_code);
        }
        return false;
    }

    return ble_srv_is_indication_enabled(cccd_val_buf);
}

/**@brief Connect event handler.
 *
 * @param[in]   p_strm      Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_strm_t* p_strm, ble_evt_t* p_ble_evt)
{
    p_strm->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

/**@brief Disconnect event handler.
 *
 * @param[in]   p_strm      Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_strm_t* p_strm, ble_evt_t* p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_strm->conn_handle = BLE_CONN_HANDLE_INVALID;
}


/**@brief Authorized Read/Write event handler
 *
 * @param[in]  p_strm     Service structure.
 * @param[in]  p_ble_evt  Event received from the BLE stack.
 */
static void on_rw_auth_req(ble_strm_t* p_strm, ble_evt_t* p_ble_evt)
{
    uint8_t        type;
    uint16_t       handle;
    uint16_t       len;
    uint8_t*       p_data;

    ble_gatts_evt_rw_authorize_request_t*    p_authorize_request;
    ble_gatts_evt_write_t*                   p_evt_write;
    ble_gatts_evt_read_t*                    p_evt_read;

    p_authorize_request     = &(p_ble_evt->evt.gatts_evt.params.authorize_request);
    p_evt_write             = &(p_authorize_request->request.write);
    p_evt_read              = &(p_authorize_request->request.read);

    type                    = p_authorize_request->type;

    if(type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)
    {
        handle   = p_evt_write->handle;
        len      = p_evt_write->len;
        p_data   = p_evt_write->data;

        //        if( p_evt_write->op == BLE_GATTS_OP_PREP_WRITE_REQ )
        //        {
        //            trace_log("BLE_GATTS_OP_PREP_WRITE_REQ\r\n");
        //            ble_gatts_w_authorize_reply(p_strm->conn_handle, BLE_GATT_STATUS_SUCCESS);
        //        }
        //        else if (p_evt_write->op == BLE_GATTS_OP_EXEC_WRITE_REQ_NOW)
        //        {
        //            trace_log("BLE_GATTS_OP_EXEC_WRITE_REQ_NOW\r\n");
        //            p_strm->data_write_handler(p_strm,data,len);
        //        }

        /*如果handle 等于数据特征的value_hanlde*/
        if(handle == p_strm->data_char_handle.value_handle)
        {
            //            trace_log("write_handler \r\n");
            p_strm->write_handler(p_strm, BLE_STRM_EVT_DATA_WRITE, handle, p_data, len);
        }

        /*如果handle 等于*/
        else if(handle == p_strm->pwd_char_handle.value_handle)
        {
            p_strm->write_handler(p_strm, BLE_STRM_EVT_PASSWORD_WRITE, handle, p_data, len);
        }
        else if(handle == p_strm->test_char_handle.value_handle)
        {
            p_strm->write_handler(p_strm, BLE_STRM_EVT_TEST_WRITE, handle, p_data, len);
        }
        else if(handle == p_strm->cmd_char_handle.value_handle)
        {
            p_strm->write_handler(p_strm, BLE_STRM_EVT_CMD_WRITE, handle, p_data, len);
        }
        else
        {
            /*Do nothing*/
        }
    }
    else if(type == BLE_GATTS_AUTHORIZE_TYPE_READ)
    {
        handle   = p_evt_read->handle;

        if(handle == p_strm->data_char_handle.value_handle)
        {
            p_strm->read_handler(p_strm, BLE_STRM_EVT_DATA_READ, handle);
        }
    }
    else
    {
        /*Do nothing*/
    }
}

static void on_hvc(ble_strm_t* p_strm, ble_evt_t* p_ble_evt)
{
    uint16_t handle   = p_ble_evt->evt.gatts_evt.params.hvc.handle;
    if(handle == p_strm->data_char_handle.value_handle)
    {
        p_strm->hvc_handler(p_strm, p_strm->data_char_handle);
    }
    else if(handle == p_strm->test_char_handle.value_handle)
    {
        p_strm->hvc_handler(p_strm, p_strm->test_char_handle);
    }
    else
    {
        /*Do nothing*/
    }
}

void ble_strm_on_ble_evt(ble_strm_t* p_strm, ble_evt_t* p_ble_evt)
{

    switch(p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_strm, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_strm, p_ble_evt);
            break;

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST :
            on_rw_auth_req(p_strm, p_ble_evt);
            break;

        case BLE_GATTS_EVT_HVC:
            on_hvc(p_strm, p_ble_evt);
            break;

        default:
            break;
    }
}

/**@brief Add data characteristic.
 *
 * @param[in]   p_strm        Service structure.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t strm_data_char_add(ble_strm_t* p_strm)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    ble_gatts_attr_md_t cccd_md;

    memset(&cccd_md, 0, sizeof(cccd_md));

    // According to BAS_SPEC_V10, the read operation on cccd should be possible without
    // authentication.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read     = 1;
    char_md.char_props.write    = 1;
    char_md.char_props.indicate = 1;
    char_md.p_char_user_desc    = NULL;
    char_md.p_char_pf           = NULL;
    char_md.p_user_desc_md      = NULL;
    char_md.p_cccd_md           = &cccd_md;
    char_md.p_sccd_md           = NULL;

    ble_uuid.type = p_strm->uuid_type;
    ble_uuid.uuid = STRM_UUID_DATA_CHAR;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 1;
    attr_md.wr_auth    = 1;
    attr_md.vlen       = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = 0;
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = 20;

    return sd_ble_gatts_characteristic_add(p_strm->service_handle, &char_md,
                                           &attr_char_value,
                                           &p_strm->data_char_handle);
}

/**@brief Add password characteristic.
 *
 * @param[in]   p_strm        Service structure.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t strm_pwd_char_add(ble_strm_t* p_strm)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 0;
    char_md.char_props.write  = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
    char_md.p_sccd_md         = NULL;

    ble_uuid.type = p_strm->uuid_type;
    ble_uuid.uuid = STRM_UUID_PWD_CHAR;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 1;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = 16;
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = 16;


    return sd_ble_gatts_characteristic_add(p_strm->service_handle, &char_md,
                                           &attr_char_value,
                                           &p_strm->pwd_char_handle);
}

/**@brief Add test characteristic.
 *
 * @param[in]   p_strm        Service structure.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t strm_test_char_add(ble_strm_t* p_strm)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    ble_gatts_attr_md_t cccd_md;

    memset(&cccd_md, 0, sizeof(cccd_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read     = 0;
    char_md.char_props.write    = 1;
    char_md.char_props.indicate = 1;
    char_md.p_char_user_desc    = NULL;
    char_md.p_char_pf           = NULL;
    char_md.p_user_desc_md      = NULL;
    char_md.p_cccd_md           = &cccd_md;
    char_md.p_sccd_md           = NULL;

    ble_uuid.type = p_strm->uuid_type;
    ble_uuid.uuid = STRM_UUID_TEST_CHAR;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 1;
    attr_md.vlen       = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = 0;
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = 20;

    return sd_ble_gatts_characteristic_add(p_strm->service_handle, &char_md,
                                           &attr_char_value,
                                           &p_strm->test_char_handle);
}

/**@brief Add cmd characteristic.
 *
 * @param[in]   p_strm        Service structure.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t strm_cmd_char_add(ble_strm_t* p_strm)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read     = 0;
    char_md.char_props.write    = 1;
    char_md.char_props.indicate = 0;
    char_md.p_char_user_desc    = NULL;
    char_md.p_char_pf           = NULL;
    char_md.p_user_desc_md      = NULL;
    char_md.p_cccd_md           = NULL;
    char_md.p_sccd_md           = NULL;

    ble_uuid.type = p_strm->uuid_type;
    ble_uuid.uuid = STRM_UUID_CMD_CHAR;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 1;
    attr_md.vlen       = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = 0;
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = 20;

    return sd_ble_gatts_characteristic_add(p_strm->service_handle, &char_md,
                                           &attr_char_value,
                                           &p_strm->cmd_char_handle);
}

static uint32_t strm_version_char_add(ble_strm_t* p_strm, const ble_strm_init_t* p_strm_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read     = 1;
    char_md.char_props.write    = 0;
    char_md.char_props.indicate = 0;
    char_md.p_char_user_desc    = NULL;
    char_md.p_char_pf           = NULL;
    char_md.p_user_desc_md      = NULL;
    char_md.p_cccd_md           = NULL;
    char_md.p_sccd_md           = NULL;

    ble_uuid.type = p_strm->uuid_type;
    ble_uuid.uuid = STRM_UUID_VERSION_CHAR;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);

    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = strlen((char *)p_strm_init->version);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = strlen((char *)p_strm_init->version);
    attr_char_value.p_value      = p_strm_init->version;

    return sd_ble_gatts_characteristic_add(p_strm->service_handle, &char_md,
                                           &attr_char_value,
                                           &p_strm->cmd_char_handle);
}

uint32_t ble_strm_init(ble_strm_t* p_strm, const ble_strm_init_t* p_strm_init)
{
    uint32_t   err_code;
    ble_uuid_t service_uuid;
    ble_uuid128_t base_uuid =
    {
        .uuid128 = STRM_BASE_UUID
    };
    uint8_t uuid_type;

    err_code = sd_ble_uuid_vs_add( &base_uuid, &uuid_type );
    APP_ERROR_CHECK( err_code );

    // Initialize service structure
    p_strm->uuid_type           = uuid_type;
    p_strm->conn_handle         = BLE_CONN_HANDLE_INVALID;
    p_strm->write_handler       = p_strm_init->write_handler;
    p_strm->read_handler        = p_strm_init->read_handler;
    p_strm->hvc_handler         = p_strm_init->hvc_handler;
    p_strm->error_handler       = p_strm_init->error_handler;

    service_uuid.type           = uuid_type;
    service_uuid.uuid           = STRM_UUID_SERVICE;

    /*添加服务*/
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &service_uuid, &p_strm->service_handle);
    VERIFY_SUCCESS(err_code);

    /*添加数据特征*/
    err_code = strm_data_char_add(p_strm);
    VERIFY_SUCCESS(err_code);

    /*添加密码特征*/
    err_code = strm_pwd_char_add(p_strm);
    VERIFY_SUCCESS(err_code);

    /*添加测试特征*/
    err_code = strm_test_char_add(p_strm);
    VERIFY_SUCCESS(err_code);

    // add cmd char
    err_code = strm_cmd_char_add(p_strm);
    VERIFY_SUCCESS(err_code);

    err_code = strm_version_char_add(p_strm, p_strm_init);
    VERIFY_SUCCESS(err_code);

    return NRF_SUCCESS;
}

/**@brief Function for characteristic indication
 *
 * @param
 *
 * @retval
 */
uint32_t ble_strm_char_indicate(ble_strm_t*               p_strm,
                                ble_gatts_char_handles_t  char_handle,
                                uint8_t*                  p_data,
                                uint16_t                  length)
{
    uint32_t err_code = NRF_SUCCESS;
    bool cccd_configured = false;

    if(p_strm->conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        /*如果使能了Indication*/
        cccd_configured = is_cccd_configured(p_strm, char_handle.cccd_handle);
        if(cccd_configured)
        {
            ble_gatts_hvx_params_t hvx_params;
            memset(&hvx_params, 0, sizeof(hvx_params));

            hvx_params.handle   = char_handle.value_handle;
            hvx_params.type     = BLE_GATT_HVX_INDICATION;
            hvx_params.offset   = 0;
            hvx_params.p_len    = &length;
            hvx_params.p_data   = p_data;

            sd_ble_gatts_hvx(p_strm->conn_handle, &hvx_params);
        }
        else
        {
            err_code = NRF_ERROR_INVALID_STATE;
        }
    }
    else
    {
        err_code = NRF_ERROR_INVALID_STATE;
    }
    return err_code;
}
