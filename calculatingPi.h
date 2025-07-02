#ifndef _VRE_APP_WIZARDTEMPLATE_
#define	_VRE_APP_WIZARDTEMPLATE_

#include "vmio.h"
#include "vmgraph.h"
#include "vmstdlib.h"
#include "string.h"
#include "stdint.h"		

VMINT		layer_hdl[1];

void handle_sysevt(VMINT message, VMINT param);
void handle_keyevt(VMINT event, VMINT keycode);
static void fill_white(void);
VMINT string_width(VMWCHAR *whead, VMWCHAR *wtail);
void display_text_line(VMUINT8 *disp_buf, VMSTR str, VMINT x, VMINT y, VMINT width, VMINT height, VMINT betlines, VMINT startLine, VMINT color, VMBOOL fix_pos);
float nk_inv_sqrt(float number);
float nk_sqrt(float x);
float semicircle(float x);
float integral(float (*func)(float), float a, float b, int n);
int main22();
void display_text_line1(VMUINT8 *disp_buf, VMSTR str, VMINT x, VMINT y, VMINT width, VMINT height, VMINT betlines, VMINT startLine, VMINT color, VMBOOL fix_pos);
VMINT adjust_x_for_last_char(VMINT line_width, VMWCHAR last_char, VMINT default_x, VMINT screen_width);

#endif

