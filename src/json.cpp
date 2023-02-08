#include "../inc/json.h"

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(json);

using namespace hangang_view;

/**
 * @brief due to float isn't supported by zephyr json parser yet,
 * integer is used and real float value can calculated by dividing it by 100
 * 
 */
struct json_obj_descr symbol_descr[] = {
    JSON_OBJ_DESCR_PRIM(struct json::symbol, name, JSON_TOK_STRING),
    JSON_OBJ_DESCR_PRIM(struct json::symbol, price, JSON_TOK_NUMBER),
    JSON_OBJ_DESCR_PRIM(struct json::symbol, percentile, JSON_TOK_NUMBER),
};

struct json_obj_descr symbols_descr[] = {
    JSON_OBJ_DESCR_OBJ_ARRAY(struct json::symbols, symbols, 30, symbols_len,
                             symbol_descr, ARRAY_SIZE(symbol_descr)),
};

int json::ParseSymbolJson(char* payload, int payload_len,
                          struct json::symbols* data) {
  LOG_DBG("start parse %d byte of data", payload_len);
  return json_arr_parse(payload, payload_len, symbols_descr, data);
}