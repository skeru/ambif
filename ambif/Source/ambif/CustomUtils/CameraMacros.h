//* Copyright 2015 Stefano Cherubin
//* stefano1{dot}cherubin{at}mail{dot}polimi{dot}it
//* ISPG - Image and Sound Processing Group
//* Politecnico di Milano

#pragma once

#define CAMERA_ZOOM_MAX 20000.0f
#define CAMERA_ZOOM_MIN 500.0f
#define CAMERA_ZOOM_STEP 500.0f

#define CAMERA_ARM_LENGTH 20000.0f

/* distance = (current_zoom - zoom min) / range
* camera_angle = K1 * d^2 + alfa0
*/
#define CAMERA_RANGE (float)(CAMERA_ZOOM_MAX - CAMERA_ZOOM_MIN)

//* camera angle when zoom is max
#define CAMERA_ALFA0 -75.0f

//* camera angle when zoom is min
#define CAMERA_ALFA_MAX -15.0f


//* Zoom (%)  range [0;1] - 0 means closer
#define CAMERA_ZOOM_PERCENT(currentZoom) ((float)((currentZoom) - CAMERA_ZOOM_MIN)/CAMERA_RANGE)

//* helper macro: do not use
#define CAMERA_ANGLE_K1 (CAMERA_ALFA_MAX - CAMERA_ALFA0)
//* Defines pitch rotation based on arm LENGTH
#define CAMERA_ANGLE(currentZoom) ((CAMERA_ZOOM_PERCENT((currentZoom)) * CAMERA_ZOOM_PERCENT((currentZoom)))*CAMERA_ANGLE_K1 + CAMERA_ALFA0)
//* Camera Arm lenght. zoomPercent 0 means zoom close to player
#define CAMERA_ZOOM_VALUE(zoomPercent) ((float)(CAMERA_RANGE * (zoomPercent)) + CAMERA_ZOOM_MIN)