/* select_road_surface_strip_cel and its helpers live in
 * road_surface_cel_selection.h as static __inline: the function has three
 * call sites, all inside per-strip loops, and its prologue/epilogue cost a
 * third of each call. */
#include "road_surface_cel_selection.h"
