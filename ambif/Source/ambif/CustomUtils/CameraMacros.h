// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#pragma once

#define CAMERA_ZOOM_MAX 20000.0f
#define CAMERA_ZOOM_MIN 500.0f
#define CAMERA_ZOOM_STEP 500.0f

#define CAMERA_ARM_LENGTH 5000.0f

/* distance = (current_zoom - zoom min) / range
* camera_angle = K1 * d^2 + alfa0
*/
#define CAMERA_RANGE (float)(CAMERA_ZOOM_MAX - CAMERA_ZOOM_MIN)
//* camera angle when zoom is max
#define CAMERA_ALFA0 -70.0f
//* camera angle when zoom is min
#define CAMERA_ALFA_MAX -30.0f


//* Zoom (%)  range [0;1]
#define CAMERA_ZOOM_PERCENT(currentZoom) ((float)((currentZoom) - CAMERA_ZOOM_MIN)/CAMERA_RANGE)

//* helper macro: do not use
#define CAMERA_ANGLE_K1 (CAMERA_ALFA_MAX - CAMERA_ALFA0)
//* Defines pitch rotation based on arm LENGTH
#define CAMERA_ANGLE(currentZoom) ((CAMERA_ZOOM_PERCENT((currentZoom)) * CAMERA_ZOOM_PERCENT((currentZoom)))*CAMERA_ANGLE_K1 + CAMERA_ALFA0)