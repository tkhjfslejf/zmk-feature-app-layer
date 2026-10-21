#include <stdint.h>

#include <zephyr/logging/log.h>
#include <zmk/event_manager.h>
#include <zmk/keymap.h>
#include <raw_hid/events.h>

LOG_MODULE_REGISTER(app_layer_autoswitch, CONFIG_ZMK_LOG_LEVEL);

/*
 * Protocol:
 * byte 0: magic      = 0xA7
 * byte 1: layer_id
 */

#define APP_LAYER_MAGIC   0xA7

static int raw_hid_received_event_listener(const zmk_event_t *eh)
{
    struct raw_hid_received_event *event = as_raw_hid_received_event(eh);

    if (event == NULL || event->data == NULL || event->length < 2) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    const uint8_t *data = event->data;

    if (data[0] != APP_LAYER_MAGIC) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    zmk_keymap_layer_id_t target = data[1];

    /*
     * Layer IDs are checked against the compiled keymap.
     * This example assumes layer reordering is disabled.
     */
    if (target >= ZMK_KEYMAP_LAYERS_LEN) {
        LOG_ERR("Target layer ID %u does not exist", target);
        return ZMK_EV_EVENT_BUBBLE;
    }

    zmk_keymap_layer_index_t active_layer = zmk_keymap_highest_layer_active();

    if (active_layer != target){
        int ret = zmk_keymap_layer_to(target);
    
        if (ret < 0) {
            LOG_ERR("Failed to switch to layer %u: %d", target, ret);
        } else {
            LOG_INF("Application layer switched to %u", target);
        }
    }else{
        LOG_INF("The current layer is already %u,No need to switch", target);
    }
    
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(app_layer_autoswitch, raw_hid_received_event_listener);
ZMK_SUBSCRIPTION(app_layer_autoswitch, raw_hid_received_event);
