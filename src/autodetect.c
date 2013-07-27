/*
 * frdm-mntr autodetect module.
*/

#if defined __linux
#include "autodetect-linux.c"
#else
#error "Missing autodetect implementation for this platform, sorry!"
#endif
