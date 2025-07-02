#include "calculatingPi.h"

VMUINT8* screenbuf = NULL;	
VMINT ncharacter_height = -1;
VMINT nscreen_width = -1;
VMINT nscreen_height = -1;
VMINT filledDsplByLines = 0;
VMINT drv;
VMBOOL first = VM_FALSE;
VMBOOL third = VM_FALSE;

VMINT layer_hdl[1];
int radius = 1;

void vm_main(void) {
    vm_reg_sysevt_callback(handle_sysevt);
    vm_reg_keyboard_callback(handle_keyevt);
    vm_font_set_font_size(VM_SMALL_FONT);
    ncharacter_height = vm_graphic_get_character_height();
    nscreen_width = vm_graphic_get_screen_width();
    nscreen_height = vm_graphic_get_screen_height();
    drv = vm_get_removable_driver();
    if (drv < 0) { drv = vm_get_system_driver(); }
}

void handle_sysevt(VMINT message, VMINT param) {
    switch (message) {
        case VM_MSG_CREATE:
        case VM_MSG_ACTIVE:
            layer_hdl[0] = vm_graphic_create_layer(0, 0, nscreen_width, nscreen_height, -1);
            vm_graphic_set_clip(0, 0, nscreen_width, nscreen_height);
            fill_white();
            break;
        case VM_MSG_PAINT:
            vm_switch_power_saving_mode(turn_off_mode);
            screenbuf = vm_graphic_get_layer_buffer(layer_hdl[0]);
            if(first == VM_FALSE) {
              first = VM_TRUE;
              display_text_line(screenbuf, "Please wait, calculating...", 0, filledDsplByLines, nscreen_width, nscreen_height, 2, 1, VM_COLOR_BLACK, VM_FALSE);
              filledDsplByLines = 0;
              main22();
            }
            break;
        case VM_MSG_INACTIVE:
        case VM_MSG_QUIT:
            if (layer_hdl[0] != -1)
                vm_graphic_delete_layer(layer_hdl[0]);
            break;
    }
}

void handle_keyevt(VMINT event, VMINT keycode) {
    if (event == VM_KEY_EVENT_UP && keycode == VM_KEY_RIGHT_SOFTKEY) {
        if (layer_hdl[0] != -1) {
            vm_graphic_delete_layer(layer_hdl[0]);
            layer_hdl[0] = -1;
        }
        vm_exit_app();
    }
    if (event == VM_KEY_EVENT_UP && keycode == VM_KEY_LEFT_SOFTKEY) {
        main22();
    }

    if (event == VM_KEY_EVENT_UP && keycode == VM_KEY_NUM1) {

        display_text_line(screenbuf,
                  "Tai labai ilgas tekstas, kuris turėtų būti automatiškai laužomas į kelias eilutes, jei netelpa ekrano plotyje.",
                  0, 0,
                  nscreen_width, nscreen_height,
                  2, 0,
                  VM_COLOR_BLACK,
                  VM_FALSE);

    }

}

static void fill_white(void) {
    vm_graphic_color color;
    color.vm_color_565 = VM_COLOR_WHITE;
    vm_graphic_setcolor(&color);
    vm_graphic_fill_rect_ex(layer_hdl[0], 0, 0, nscreen_width, nscreen_height);
    vm_graphic_flush_layer(layer_hdl, 1);
    filledDsplByLines = 0;
}

VMINT string_width(VMWCHAR *whead, VMWCHAR *wtail) {

    VMWCHAR *wtemp = NULL;
    VMINT width = 0;
    if (whead == NULL || wtail == NULL) return 0;
    wtemp = (VMWCHAR *)vm_malloc((wtail - whead) * 2 + 2);
    if (wtemp == NULL) return 0;
    memset(wtemp, 0, (wtail - whead) * 2 + 2);
    memcpy(wtemp, whead, (wtail - whead) * 2);
    width = vm_graphic_get_string_width(wtemp);
    vm_free(wtemp);
    return width;
}

void display_text_line(VMUINT8 *disp_buf, VMSTR str, VMINT x, VMINT y, VMINT width, VMINT height, VMINT betlines, VMINT startLine, VMINT color, VMBOOL fix_pos) {

    VMWCHAR *ucstr;
    VMWCHAR *ucshead;
    VMWCHAR *ucstail;
    VMINT is_end = FALSE;
    VMINT nheight = y;
    VMINT nline_height;
    VMINT nlines = 0;

    if (y == 0) {
       fill_white();
       nheight = nheight + 1;
    }
    if (str == NULL || disp_buf == NULL || betlines < 0) return;

    nline_height = vm_graphic_get_character_height() + betlines;

    if (third == VM_TRUE && fix_pos == VM_FALSE) {
        nheight += nline_height;
        third = VM_FALSE;
    }

    if (fix_pos == VM_TRUE) {
        vm_graphic_fill_rect(screenbuf, 0, filledDsplByLines, nscreen_width, nline_height, VM_COLOR_WHITE, VM_COLOR_WHITE);
        third = VM_TRUE;
    }

    ucstr = (VMWCHAR*)vm_malloc(2 * (strlen(str) + 1));
    if (ucstr == NULL) return;
    if (0 != vm_ascii_to_ucs2(ucstr, 2 * (strlen(str) + 1), str)) {
        vm_free(ucstr);
        return;
    }

    ucshead = ucstr;
    ucstail = ucshead + 1;

    while (!is_end) {
        //if (nheight > y + height) {
        //if (nheight + y > height) {
        if (nheight > height) {
            fill_white();
            nheight = 1;
        }

        while (1) {
            if (string_width(ucshead, ucstail) <= width) {
                ucstail++;
            } else {
                nlines++;
                ucstail--;
                break;
            }
            if (0 == vm_wstrlen(ucstail)) {
                is_end = TRUE;
                nlines++;
                break;
            }
        }

        if (nlines >= startLine) {
            vm_graphic_textout(disp_buf, x, nheight, ucshead, (ucstail - ucshead), (VMUINT16)(color));
            vm_graphic_flush_layer(layer_hdl, 1);
            if (!fix_pos) nheight += nline_height;
            filledDsplByLines = nheight;
        }

        ucshead = ucstail;
        ucstail++;
    }
    vm_free(ucstr);
}

float nk_inv_sqrt(float number) {
    float x2;
    const float threehalfs = 1.5f;
    union { uint32_t i; float f; } conv = {0};
    conv.f = number;
    x2 = number * 0.5f;
    conv.i = 0x5f375A84 - (conv.i >> 1);
    conv.f = conv.f * (threehalfs - (x2 * conv.f * conv.f));
    return conv.f;
}

float nk_sqrt(float x) {
    return x * nk_inv_sqrt(x);
}

float semicircle(float x) {
    return nk_sqrt(1.0f - x * x);
}

float integral(float (*func)(float), float a, float b, int n) {
    float dx = (b - a) / n;
    float sum = 0.0f;
    int i;
    for (i = 0; i < n; i++) {
        sum += func(a + i * dx) + func(a + (i + 1) * dx);
    }
    return sum * dx / 2.0f;
}

int main22() {
    VMCHAR text[100];
    float a = -1.0f;
    float b = 1.0f;
    //int n = 1000000;
    int n = 500000;

    VMUINT32 start = vm_get_tick_count();
    float result = integral(semicircle, a, b, n);
    float calculatedPi = 2.0f * result;
    VMUINT32 end = vm_get_tick_count();
    VMUINT32 elapsed = end - start;

    sprintf(text, "Cycles: %d Pi: %.6f", n, calculatedPi);
    display_text_line(screenbuf, text, 0, filledDsplByLines, nscreen_width, nscreen_height, 2, 1, VM_COLOR_BLACK, VM_FALSE);

    sprintf(text, "Time: %lu ms", elapsed);
    display_text_line(screenbuf, text, 0, filledDsplByLines, nscreen_width, nscreen_height, 2, 1, VM_COLOR_BLACK, VM_FALSE);

    return 0;
}

void display_text_line1(VMUINT8 *disp_buf, VMSTR str, VMINT x, VMINT y, VMINT width, VMINT height, VMINT betlines, VMINT startLine, VMINT color, VMBOOL fix_pos) {

    VMWCHAR *ucstr, *ucshead, *ucstail;
    VMINT is_end = FALSE;
    VMINT nheight = y;
    VMINT nline_height;
    VMINT nlines = 0;
    VMINT prev_line_width = 0;

    if (!str || !disp_buf || betlines < 0) return;

    // Išvalyti ekraną jei nuo viršaus
    if (y == 0) {
        fill_white();
        nheight += 1;
    }

    nline_height = vm_graphic_get_character_height() + betlines;

    if (third == VM_TRUE && fix_pos == VM_FALSE) {
        nheight += nline_height;
        third = VM_FALSE;
    }

    if (fix_pos == VM_TRUE) {
        vm_graphic_fill_rect(screenbuf, 0, filledDsplByLines, nscreen_width, nline_height, VM_COLOR_WHITE, VM_COLOR_WHITE);
        third = VM_TRUE;
    }

    // ASCII -> UCS2 konversija
    ucstr = (VMWCHAR*)vm_malloc(2 * (strlen(str) + 1));
    if (!ucstr) return;

    if (vm_ascii_to_ucs2(ucstr, 2 * (strlen(str) + 1), str) != 0) {
        vm_free(ucstr);
        return;
    }

    ucshead = ucstr;
    ucstail = ucshead + 1;

    while (!is_end) {
        if (nheight > y + height) {
            fill_white();
            nheight = 1;
        }

        while (1) {
            if (*ucstail == 0) {
                is_end = TRUE;
                break;
            }

            if (string_width(ucshead, ucstail) <= width) {
                ucstail++;
            } else {
                ucstail--;
                break;
            }
        }

        if (nlines >= startLine) {
            // Išlyginimas paskutinio simbolio atveju

VMINT current_line_width = string_width(ucshead, ucstail);
VMWCHAR last_char = *(ucstail - 1);
VMINT adjust_x = adjust_x_for_last_char(current_line_width, last_char, x, nscreen_width);

            if (prev_line_width > 0 && prev_line_width < nscreen_width && current_line_width >= nscreen_width) {
                VMWCHAR last_char = *(ucstail - 1);
                VMWCHAR last_char_buf[2] = { last_char, 0 };
                VMINT last_char_width = vm_graphic_get_string_width(last_char_buf);

                if (last_char_width > ncharacter_height / 2) {
                    adjust_x -= last_char_width / 2;
                    if (adjust_x < 0) adjust_x = 0;
                }
            }

            vm_graphic_textout(disp_buf, adjust_x, nheight, ucshead, ucstail - ucshead, (VMUINT16)color);
            vm_graphic_flush_layer(layer_hdl, 1);

            if (!fix_pos) nheight += nline_height;
            filledDsplByLines = nheight;
            prev_line_width = current_line_width;
        }

        ucshead = ucstail;
        if (*ucstail != 0) ucstail++;
        nlines++;
    }

    vm_free(ucstr);
}

/**
 * Koreguoja X, kad paskutinio simbolio plotis galėtų šiek tiek išlįsti už dešinės ekrano ribos.
 *
 * @param line_width      Dabartinės eilutės plotis (px)
 * @param last_char       Paskutinis simbolis eilutėje
 * @param default_x       Pradinis X (dažniausiai 0)
 * @param screen_width    Ekrano plotis (px)
 * @return                Koreguotas X – gali būti mažesnis, bet ne < 0
 */
VMINT adjust_x_for_last_char(VMINT line_width, VMWCHAR last_char, VMINT default_x, VMINT screen_width) {
    VMINT char_width = vm_graphic_get_character_width(last_char);

    // Jei eilutė jau siekia kraštą, ir simbolis platus – leidžiam jam šiek tiek išlįsti
    if (line_width >= screen_width && char_width > (ncharacter_height / 2)) {
        VMINT overflow = line_width + (char_width / 2) - screen_width;
        if (overflow > 0) {
            VMINT adjust_x = default_x - overflow;
            if (adjust_x < 0) adjust_x = 0;
            return adjust_x;
        }
    }

    return default_x;
}
