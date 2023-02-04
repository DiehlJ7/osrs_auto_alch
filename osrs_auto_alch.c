#include <stdlib.h>
#include <string.h>
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include "version.h"
#include "tools.h"

typedef enum {
    EventTypeInput,
} EventType;

typedef struct {
    union {
        InputEvent input;
    };
    EventType type;
} UsbMouseEvent;

bool btn_left_autofire = false;
uint32_t autofire_delay = 300;
uint32_t rand_delay = 300;
uint32_t r = 0;

static void osrs_auto_alch_render_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);
    char autofire_delay_str[12];
    char range_start[12];
    char range_end[12];
    //std::string pi = "pi is " + std::to_string(3.1415926);
    itoa((autofire_delay + r), autofire_delay_str, 10);
    itoa((autofire_delay), range_start, 10);
    itoa((autofire_delay + rand_delay), range_end, 10);
    //sprintf(autofire_delay_str, "%lu", autofire_delay);

    canvas_clear(canvas);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 0, 10, "OSRS Alchemy Helper");
    canvas_draw_str(canvas, 80, 34, btn_left_autofire ? "[active]" : "[inactive]");

    canvas_set_font(canvas, FontSecondary);
    // canvas_draw_str(canvas, 90, 10, "v0.5");
    canvas_draw_str(canvas, 0, 22, "Press [ok] for auto left clicking");
    canvas_draw_str(canvas, 0, 34, "Delay [ms]:");
    canvas_draw_str(canvas, 50, 34, autofire_delay_str);
    
    canvas_draw_str(canvas, 0, 46, "Range [ms]:");
    canvas_draw_str(canvas, 50, 46, range_start);
    canvas_draw_str(canvas, 65, 46, " - ");
    canvas_draw_str(canvas, 75, 46, range_end);
    
    canvas_draw_str(canvas, 0, 63, "Press [back] to exit");
}

static void osrs_auto_alch_input_callback(InputEvent* input_event, void* ctx) {
    FuriMessageQueue* event_queue = ctx;

    UsbMouseEvent event;
    event.type = EventTypeInput;
    event.input = *input_event;
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

int32_t osrs_auto_alch_app(void* p) {
    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(UsbMouseEvent));
    furi_check(event_queue);
    ViewPort* view_port = view_port_alloc();

    FuriHalUsbInterface* usb_mode_prev = furi_hal_usb_get_config();
#ifndef OSRS_AUTO_ALCH_SCREENSHOT
    furi_hal_usb_unlock();
    furi_check(furi_hal_usb_set_config(&usb_hid, NULL) == true);
#endif

    view_port_draw_callback_set(view_port, osrs_auto_alch_render_callback, NULL);
    view_port_input_callback_set(view_port, osrs_auto_alch_input_callback, event_queue);

    // Open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    UsbMouseEvent event;
    //srand(time(NULL));

    while(1) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 50);

        if(event_status == FuriStatusOk) {
            if(event.type == EventTypeInput) {
                if(event.input.key == InputKeyBack) {
                    break;
                }

                if(event.input.type != InputTypeRelease) {
                    continue;
                }

                switch(event.input.key) {
                    case InputKeyOk:
                        btn_left_autofire = !btn_left_autofire;
                        break;
                    case InputKeyLeft:
                        if(autofire_delay > 0) {
                            autofire_delay -= 10;
                        }
                        break;
                    case InputKeyRight:
                        autofire_delay += 10;
                        break;
                    case InputKeyUp:
                        rand_delay += 10;
                        break;
                    case InputKeyDown:
                        if(rand_delay > 0) {
                            rand_delay -= 10;
                        }
                        break;
                    default:
                        break;
                }
            }
        }

        if(btn_left_autofire) {
            r = rand() % (rand_delay + 1);
            furi_hal_hid_mouse_press(HID_MOUSE_BTN_LEFT);
            // TODO: Don't wait, but use the timer directly to just don't send the release event (see furi_hal_cortex_delay_us)
            furi_delay_us((autofire_delay + r)* 500);
            furi_hal_hid_mouse_release(HID_MOUSE_BTN_LEFT);
            furi_delay_us((autofire_delay + r) * 500);
        }

        view_port_update(view_port);
    }

    furi_hal_usb_set_config(usb_mode_prev, NULL);

    // remove & free all stuff created by app
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);

    return 0;
}
